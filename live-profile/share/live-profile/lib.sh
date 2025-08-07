

debug () {
    [ -n "$LIVE_PROFILE_DEBUG" ] && printf "%s\n" "$*" >&2
}

err () {
    printf "Error: %s\n" "$*" >&2
}

if [ "$UID" == 0 ]; then
    LIVE_PROFILE_DIR=/etc/live-profile
else
    LIVE_PROFILE_DIR=~/.config/live-profile
fi

live-profile:get () {
    local label="$1"

    if [ -n "$LIVE_PROFILE_SHOW_ENV" ]; then
        echo "live-profile env for command '${0##*/}':" >&2
    fi

    ## Launch plugins
    first=
    for script in "$LIVE_PROFILE_DIR"/"$label"/*.sh; do
        [ -e "$script" ] || continue
        [ -z "$first" ] && {
            first=1
            debug "live-profile for command '${0##*/}'"
        }
        debug "  ${script##*/}"
        unset run
        script_realpath=$(realpath "$script") || {
            err "failed to resolve realpath for '$script'"
            return 1
        }
        cd "${script_realpath%/*}" || {
            err "failed to cd to '${script_realpath%/*}'"
            return 1
        }
        . "$script_realpath" || {
            err "live-profile failed to source '$script'"
            return 1
        }
        declare -F run >/dev/null 2>&1 || {
            err "function 'run' not found in '$script'." >&2
            return 1
        }
        env=$(
            set -o pipefail
            {
                run 3>&1 1>&2 2>&3 </dev/null |
                    if [ -n "$LIVE_PROFILE_DEBUG" ]; then
                        sed -r 's/^/    /g'
                    else
                        cat > /dev/null
                    fi
            } 3>&1 1>&2 2>&3
           ) || {
            err "'run' function failed."
            return 1
        }
        [ -n "$env" ] || {
            debug "    .. no env"
            continue
        }
        sanitized_env=$(printf "%s" "$env" | egrep "^[A-Za-z0-9_]+=" | cut -f 1 -d ';')
        if [ "$env" != "$sanitized_env" ]; then
            err "environment is not properly sanitized."
            return 1
        fi
        debug "    .. env:"
        if [ -n "$LIVE_PROFILE_DEBUG" ]; then
            echo "$env" | sed -r 's/^/      /g' >&2
        fi
        if [ -n "$LIVE_PROFILE_SHOW_ENV" ]; then
            echo "$env" | sed -r 's/^/  /g' >&2
        fi
        echo "$env"
    done </dev/null
}
