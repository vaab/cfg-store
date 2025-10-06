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
    sprintf(tmp, "%s.tmp.XXXXXX", cache);
    /* ensure cache dir exists */
    char *dir = dirname(strdup(cache));
    if (mkdir(dir, 0755) == -1 && errno != EEXIST) {
      fprintf(stderr, "mkdir %s failed: %s\n", dir, strerror(errno));
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
    snprintf(cache, sizeof(cache), "%s%s/%s.path-env", home, CACHE_DIR, exename);

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
