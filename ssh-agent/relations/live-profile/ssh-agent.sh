

# Function to find running ssh-agent
ssh:agent:init() {
    local agent_sock old_umask SSH_AGENT_ENV
    SSH_AGENT_ENV=~/.ssh/ssh-agent-env
    if [ -e "$SSH_AGENT_ENV" ]; then
        . "$SSH_AGENT_ENV" > /dev/null
        if [ -d "/proc/$SSH_AGENT_PID" ]; then
            echo "SSH_AUTH_SOCK=$SSH_AUTH_SOCK"
            echo "SSH_AGENT_PID=$SSH_AGENT_PID"
            return 0
        fi
    fi
    old_umask=$(umask)
    umask 077
    /usr/bin/ssh-agent -s > "${SSH_AGENT_ENV}"
    umask "${old_umask}"
    . "$SSH_AGENT_ENV" -t 3600 > /dev/null
    echo "SSH_AUTH_SOCK=$SSH_AUTH_SOCK"
    echo "SSH_AGENT_PID=$SSH_AGENT_PID"
}

run() {
    [ -n "$SSH_AGENT_PID" -a -d "/proc/$SSH_AGENT_PID" ] || ssh:agent:init </dev/null
}
