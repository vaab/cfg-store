# -*- mode: shell-script -*-

BLE_SH_SOURCE=
ble_paths=(/opt/apps/ble.sh/{,share/blesh/}ble.sh)
for ble_path in "${ble_paths[@]}"; do
    if [ -f "$ble_path" ]; then
        BLE_SH_SOURCE=$ble_path
        break
    fi
done

if [ -z "$BLE_SH_SOURCE" ]; then
    echo "ble.sh not found (looked in ${ble_paths[@]})" >&2
    return 0
fi

## XXXvlab: 2025-09-08 when using BASH_PROFILE=1, this script is sourced
## through `time`, which sets "$@", and ble.sh chokes on that.
set --

. "$BLE_SH_SOURCE" || {
  echo "Failed to source $BLE_SH_SOURCE" >&2
  return 1
}


_ble_contrib_fzf_base=~/dev/go/fzf/shell

ble-import -d integration/fzf-completion
ble-import -d integration/fzf-key-bindings

bleopt exec_restore_pipestatus=1


ble-face -s auto_complete fg=#323232  # Overlay2
ble-face -s syntax_comment fg=#707070  # Gray

ble-face -s argument_error bg=#f38ba8,fg=#11111b  # Red background, Crust foreground for better contrast
ble-face -s argument_option fg=#f2cdcd  # Flamingo

ble-face -s command_directory fg=#3465A4,bold  # Blue
ble-face -s filename_directory fg=#3465A4,bold # Blue

ble-face -s filename_executable fg=#4E9A06,bold  # Green
ble-face filename_link=fg=cyan # Cyan

ble-face -s command_builtin fg=#ffffff,bold  # Bright White
ble-face -s command_function fg=#F1EB9C  # LightYellow

ble-face -s command_alias fg=#b4befe,bold  # Lavender
ble-face -s command_file fg=#4E9A06,bold  # Green
ble-face -s command_file fg=#f8f8f8  # White

ble-face -s command_keyword fg=#ffffff,bold  # Bright White

ble-face -s region bg=#222  # Gray

ble-face -s syntax_error fg=#FFC0CB,underline  # Red background, Crust foreground for better contrast
ble-face -s filename_orphan fg=red,bg=black,bold  # Sky

ble-face -s syntax_varname fg=#fafad2  # LightGoldenrod



ble-face -s cmdinfo_cd_cdpath fg=#89b4fa,bg=#11111b  # Blue, Crust
ble-face -s disabled fg=#9399b2  # Overlay2
ble-face -s filename_directory_sticky fg=#11111b,bg=#a6e3a1  # Crust, Green
ble-face -s filename_ls_colors none
ble-face -s filename_other none
ble-face -s filename_setgid fg=#11111b,bg=#f9e2af  # Crust, Yellow
ble-face -s filename_setuid fg=#11111b,bg=#fab387  # Crust, Peach
ble-face -s menu_filter_input fg=#11111b,bg=#f9e2af  # Crust, Yellow
ble-face -s overwrite_mode fg=#11111b,bg=#89dceb  # Crust, Sky
ble-face -s prompt_status_line bg=#9399b2  # Overlay2
ble-face -s region_insert bg=#45475a  # Surface1
ble-face -s region_match fg=#11111b,bg=#f9e2af  # Crust, Yellow
ble-face -s region_target fg=#11111b,bg=#cba6f7  # Crust, Mauve
ble-face -s syntax_brace fg=#6c7086  # Overlay0
ble-face -s syntax_command fg=#b4befe  # Lavender
ble-face -s syntax_delimiter fg=#6c7086  # Overlay0
ble-face -s syntax_document fg=#f5e0dc,bold  # Rosewater
ble-face -s syntax_document_begin fg=#f5e0dc,bold  # Rosewater
ble-face -s syntax_escape fg=#f2cdcd  # Flamingo
ble-face -s syntax_expr fg=#cba6f7  # Mauve
ble-face -s syntax_function_name fg=#b4befe  # Lavender
ble-face -s syntax_glob fg=#fab387  # Peach
ble-face -s syntax_history_expansion fg=#b4befe  # Lavender
ble-face -s syntax_param_expansion fg=#f38ba8  # Red
ble-face -s syntax_quotation fg=#a6e3a1  # Green
ble-face -s syntax_tilde fg=#cba6f7  # Mauve
ble-face -s varname_array fg=#fab387  # Peach
ble-face -s varname_empty fg=#fab387  # Peach
ble-face -s varname_export fg=#fab387  # Peach
ble-face -s varname_expr fg=#fab387  # Peach
ble-face -s varname_hash fg=#fab387  # Peach
ble-face -s varname_number fg=#f5e0dc  # Rosewater
ble-face -s varname_readonly fg=#fab387  # Peach
ble-face -s varname_transform fg=#fab387  # Peach
ble-face -s varname_unset bg=#f38ba8,fg=#11111b  # Red background, Crust foreground for better contrast
ble-face -s vbell_erase bg=#45475a  # Surface1


## XXXvlab: https://github.com/akinomyoga/ble.sh/issues/243

ble-bind -f 'M-DEL' kill-backward-cword

## Change C-t of fzf ble.sh integration to C-f

ble-bind -m emacs -f C-t transpose-chars
ble-bind -m vi_imap -f C-t transpose-chars
ble-bind -m vi_nmap -s C-t -

ble-bind -m emacs -x C-f 'fzf-file-widget'
ble-bind -m vi_imap -x C-f 'fzf-file-widget'
ble-bind -m vi_nmap -s C-f 'i\C-f'



