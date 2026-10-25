#!/bin/sh
# SegaSonic for Anbernic's H700 handhelds (stock firmware): the game in the folder segasonic beside this
# script, with the firmware's SDL2; what it says goes to segasonic/segasonic.log
HERE=$(cd "$(dirname "$0")" && pwd)
cd "$HERE/segasonic" || exit 1
chmod +x ./SegaSonic 2>/dev/null
./SegaSonic > segasonic.log 2>&1
sync
