#!/bin/bash
# Gracefully restart the 27B dreamworld worker (finishes its current task first).
cd /Users/cs/Nextcloud/VSCode/Pokemon-GC
P=$(pgrep -f "[P]ython.*local_campaign.py --worker dreamworld-3080ti")
[ -n "$P" ] && kill -TERM $P && while kill -0 $P 2>/dev/null; do /bin/sleep 5; done
nohup python3 -u tools/local_campaign.py --worker dreamworld-3080ti --ollama-host http://dreamworld:11434 \
  --model qwen3.6:27b --num-predict 4096 --num-ctx 16384 --think off \
  run --timeout 1200 --retries 12 --stall-rounds 4 --permute --permute-cap 5040 --permute-seconds 900 --rewrite --rewrite-seconds 300 --recycle \
  >> build/local_llm_campaign/dreamworld-3080ti.log 2>&1 &
disown
sleep 3; pgrep -f "[P]ython.*local_campaign.py --worker dreamworld-3080ti"
