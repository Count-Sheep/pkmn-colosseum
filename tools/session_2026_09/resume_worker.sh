#!/bin/bash
S=/private/tmp/claude-501/-Users-cs-Nextcloud-VSCode-Pokemon-GC/326b227e-fc3b-4cb8-b8dc-28a01eb4c933/scratchpad
rm -f $S/worker.pause
pgrep -f "[P]ython.*local_campaign.py --worker dreamworld-3080ti" >/dev/null || $S/restart_dreamworld.sh
