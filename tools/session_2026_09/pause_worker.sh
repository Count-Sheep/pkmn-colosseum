#!/bin/bash
# Stop the local-model worker and keep the watchdog from restarting it until resume_worker.sh.
S=/private/tmp/claude-501/-Users-cs-Nextcloud-VSCode-Pokemon-GC/326b227e-fc3b-4cb8-b8dc-28a01eb4c933/scratchpad
touch $S/worker.pause
pkill -TERM -f "[P]ython.*local_campaign.py --worker dreamworld-3080ti"
for i in $(seq 1 120); do pgrep -f "[P]ython.*local_campaign.py --worker dreamworld-3080ti" >/dev/null || exit 0; /bin/sleep 3; done
echo "worker still running after 6 min" >&2; exit 1
