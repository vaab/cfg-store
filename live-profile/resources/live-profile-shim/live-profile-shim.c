/* live-profile-shim.c */
#define _GNU_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <libgen.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <inttypes.h>
#include <sys/wait.h>
#include <pwd.h>
#include <sys/types.h>

#define TTL 2                     /* re-compute cache after N seconds */
#define CACHE_DIR "/.cache/live-profile"

static void die(const char *msg)
{
    fprintf(stderr, "%s\n", msg);
    exit(127);
}

/* ---- tiny helpers --------------------------------------------------- */
static char *path_join(const char *dir, const char *file)
{
    static char buf[PATH_MAX];
    size_t n = strlen(dir);
    /* avoid “//” if DIR already ends with ‘/’ */
    snprintf(buf, sizeof(buf), "%s%s%s",
             dir,
             (n && dir[n-1] == '/') ? "" : "/",
             file);
    return buf;
}


static bool file_is_fresh(const char *p)
{
    struct stat st;
    if (stat(p, &st) == -1) return false;
    return (time(NULL) - st.st_mtime) <= TTL;
}

static char *strdup_stripnewline(char *s)
{
    size_t n = strcspn(s, "\r\n");
    s[n] = '\0';
    return strdup(s);
}

static int mkdir_p(const char *path, mode_t mode)
{
    char tmp[PATH_MAX];
    size_t len = strlen(path);
    if (len == 0 || len >= sizeof(tmp)) {
        errno = ENAMETOOLONG;
        return -1;
    }

    memcpy(tmp, path, len + 1);
    if (len == 1 && tmp[0] == '/')
        return 0;

    if (tmp[len - 1] == '/')
        tmp[len - 1] = '\0';

    for (char *p = tmp + 1; *p; ++p) {
        if (*p == '/') {
            *p = '\0';
            if (mkdir(tmp, mode) == -1 && errno != EEXIST)
                return -1;
            *p = '/';
        }
    }

    if (mkdir(tmp, mode) == -1 && errno != EEXIST)
        return -1;

    return 0;
}

static bool probe_dir_writable(const char *dir)
{
    char templ[PATH_MAX];
    int ret = snprintf(templ, sizeof(templ), "%s/.permXXXXXX", dir);
    if (ret < 0 || (size_t)ret >= sizeof(templ)) {
        errno = ENAMETOOLONG;
        return false;
    }

    int fd = mkstemp(templ);
    if (fd == -1)
        return false;

    close(fd);
    unlink(templ);
    return true;
}

static int ensure_parent_dir(const char *path)
{
    char buf[PATH_MAX];
    int ret = snprintf(buf, sizeof(buf), "%s", path);
    if (ret < 0 || (size_t)ret >= sizeof(buf)) {
        errno = ENAMETOOLONG;
        return -1;
    }

    char *dir = dirname(buf);
    if (!dir) {
        errno = EINVAL;
        return -1;
    }

    return mkdir_p(dir, 0755);
}

static bool ensure_dir_and_probe(const char *dir)
{
    if (mkdir_p(dir, 0755) == -1)
        return false;

    return probe_dir_writable(dir);
}

static bool fallback_tmp_cache(char *cache, size_t cache_size, const char *exename)
{
    const char *user = getenv("USER");
    if (!user || !*user) {
        struct passwd *pw = getpwuid(getuid());
        user = pw ? pw->pw_name : "unknown";
    }

    char tmpl[PATH_MAX];
    int ret = snprintf(tmpl, sizeof(tmpl), "/tmp/live-profile.%s.XXXXXX", user);
    if (ret < 0 || (size_t)ret >= sizeof(tmpl)) {
        errno = ENAMETOOLONG;
        return false;
    }

    if (!mkdtemp(tmpl))
        return false;

    char tmpdir[PATH_MAX];
    ret = snprintf(tmpdir, sizeof(tmpdir), "%s.tmp", tmpl);
    if (ret < 0 || (size_t)ret >= sizeof(tmpdir)) {
        int saved = errno;
        rmdir(tmpl);
        errno = saved ? saved : ENAMETOOLONG;
        return false;
    }

    if (rename(tmpl, tmpdir) == -1) {
        int saved = errno;
        rmdir(tmpl);
        errno = saved;
        return false;
    }

    ret = snprintf(tmpl, sizeof(tmpl), "%s", tmpdir);
    if (ret < 0 || (size_t)ret >= sizeof(tmpl)) {
        int saved = errno;
        rmdir(tmpdir);
        errno = saved ? saved : ENAMETOOLONG;
        return false;
    }

    if (!probe_dir_writable(tmpl))
        return false;

    ret = snprintf(cache, cache_size, "%s/%s.path-env", tmpl, exename);
    if (ret < 0 || (size_t)ret >= cache_size) {
        errno = ENAMETOOLONG;
        return false;
    }

    return true;
}

static bool select_cache_path(char *cache, size_t cache_size, const char *home, const char *exename)
{
    char cache_dir[PATH_MAX];
    int ret = snprintf(cache_dir, sizeof(cache_dir), "%s%s", home, CACHE_DIR);
    if (ret < 0 || (size_t)ret >= sizeof(cache_dir)) {
        errno = ENAMETOOLONG;
        return false;
    }

    if (ensure_dir_and_probe(cache_dir)) {
        ret = snprintf(cache, cache_size, "%s/%s.path-env", cache_dir, exename);
        if (ret < 0 || (size_t)ret >= cache_size) {
            errno = ENAMETOOLONG;
            return false;
        }
        return true;
    }

    return fallback_tmp_cache(cache, cache_size, exename);
}

/* ---- find next git in $PATH after *our* directory ------------------- */
static char *find_next_exec(const char *self_path, const char *exename)
{
    char *path = getenv("PATH");
    if (!path) return NULL;
    struct stat self_st;
    if (stat(self_path, &self_st) == -1) return NULL;

    char *paths = strdup(path), *saveptr = NULL, *p = NULL;
    for (p = strtok_r(paths, ":", &saveptr); p; p = strtok_r(NULL, ":", &saveptr)) {
        if (!*p) continue;                       /* empty element */

        char *cand = path_join(p, exename);
        if (access(cand, X_OK) != 0) continue;

        struct stat cand_st;
        if (stat(cand, &cand_st) == -1) continue;

        /* first hit that is *us* → mark, skip */
        if (
            cand_st.st_ino == self_st.st_ino && cand_st.st_dev == self_st.st_dev) {
            continue;
        }

        free(paths);
        return strdup(cand);
    }
    free(paths);
    return NULL;
}

/* ---- read cache file ------------------------------------------------- */
static bool read_cache(const char *cache, char **git_path)
{
    FILE *fp = fopen(cache, "r");
    if (!fp) return false;

    size_t  len = 0;
    char   *line = NULL;

    if (getline(&line, &len, fp) == -1) { fclose(fp); return false; }
    *git_path = strdup_stripnewline(line);

    while (getline(&line, &len, fp) != -1) {
        char *eq = strchr(line, '=');
        if (!eq) continue;
        *eq = 0;
        setenv(line, strdup_stripnewline(eq + 1), 1);
    }
    free(line);
    fclose(fp);
    return true;
}

/* ---- write cache file -------------------------------------------- */
static int write_cache_from_helper(const char *cache, const char *git_path)
{
    if (!git_path) {
      fprintf(stderr, "git_path is NULL\n");
      return -1;        /* safety */
    }
    char tmp[PATH_MAX];
    size_t need = strlen(cache) + sizeof(".tmp.XXXXXX");
    if (need >= sizeof(tmp)) {
      fprintf(stderr, "cache path too long\n");
      return -1;
    }
    snprintf(tmp, sizeof(tmp), "%s.tmp.XXXXXX", cache);
    /* ensure cache dir exists */
    if (ensure_parent_dir(cache) == -1) {
      fprintf(stderr, "failed to prepare cache directory for %s: %s\n", cache, strerror(errno));
      return -1;
    }
    /* create temp file */
    int fd = mkstemp(tmp);                 /* open temp file             */
    if (fd == -1) {
      fprintf(stderr, "mkstemp failed in %s: %s\n", tmp, strerror(errno));
      return -1;
    }
    /* 1️⃣ our own line --------------------------------------------- */
    dprintf(fd, "%s\n", git_path);

    /* 2️⃣ fork + exec live-profile, stdout → same fd -------------- */
    pid_t pid = fork();
    if (pid == -1) { close(fd); return -1; }

    if (pid == 0) {                       /* child */
        dup2(fd, STDOUT_FILENO);          /* fd becomes stdout */
        close(fd);
        execlp("live-profile", "live-profile", (char *)NULL);
        _exit(127);                       /* exec failed */
    }

    /* parent: wait child, close, rename ----------------------------- */
    int status;
    waitpid(pid, &status, 0);
    close(fd);
    if (!WIFEXITED(status) || WEXITSTATUS(status)) {
        unlink(tmp);                       /* helper failed */
        fprintf(stderr, "live-profile failed\n");
        return -1;
    }
    return rename(tmp, cache);             /* atomic move */
}

/* ---- main ----------------------------------------------------------- */
int main(int argc, char **argv, char **envp)
{
    (void)argc; (void)envp;        /* silence -Wunused-parameter */

    const char *home = getenv("HOME");
    if (!home) die("HOME not set");
    char cache[PATH_MAX];

    const char *exename = basename(argv[0]);
    if (!select_cache_path(cache, sizeof(cache), home, exename)) {
      int err = errno;
      char msg[PATH_MAX * 2];
      snprintf(msg, sizeof(msg),
               "unable to use %s%s or /tmp fallback: %s",
               home, CACHE_DIR, strerror(err));
      die(msg);
    }

    char *next_exec = NULL;

    /* fast path: fresh cache */
    if (!file_is_fresh(cache)) {
      /* where are we? */
      char self[PATH_MAX];
      ssize_t n = readlink("/proc/self/exe", self, sizeof(self)-1);
      if (n == -1) die("readlink");
      self[n] = 0;

      next_exec = find_next_exec(self, exename);
      if (!next_exec) die("cannot find next git in PATH");

      if (write_cache_from_helper(cache, next_exec) == -1)
        die("cache rebuild failed");
    }

    if (!read_cache(cache, &next_exec))
      die("cache read failed");
        goto exec;

exec:
    argv[0] = (char*)next_exec;
    execv(next_exec, argv);
    die("execv next executable");             /* only if exec failed */
}
