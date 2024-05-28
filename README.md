# autodarkmode

A simple tool to switch between dark and light mode.

It executes a command on switch and nothing else.
Location is either specified by the user or 

# Building
```
meson setup _build
cd build
meson compile
```
the resulting binary `src/autodarkmode` can then be moved to any directory in PATH.

# Configuration
```ini
[main]
; can be 'manual' or 'gclue'
locationtype = "manual"

[manual]
; set latitude and longitude in case that locationtype is set to manual
latitude = -10.0
longitude = -5.0

[light]
; command to execute for light mode
cmd = "notify-send 'a'"

[dark]
; command to execute for dark mode
cmd = "notify-send 'b'"
```
autodarkmode expects to find this configuration in `$XDG_CONFIG_HOME/autodarkmode/config.ini`, in the case that XDG_CONFIG_HOME is empty, it will use `~/.config/autodarkmode/config.ini`
