#!/bin/bash
# usage: merge_check.sh <commit>...   cherry-pick, policy-scan, build, SHA1, report diff
set -e
cd /Users/cs/Nextcloud/VSCode/Pokemon-GC
S=/private/tmp/claude-501/-Users-cs-Nextcloud-VSCode-Pokemon-GC/326b227e-fc3b-4cb8-b8dc-28a01eb4c933/scratchpad
cp build/GC6E01/report.json $S/report_before_merge.json
BASE=$(git rev-parse HEAD)
for c in "$@"; do git cherry-pick "$c" >/dev/null || { echo "CONFLICT on $c"; git status --short | grep -E '^(UU|AA|DU|UD)'; exit 2; }; done
echo "== policy scan (added lines)"
git diff $BASE HEAD -- src include | grep -E '^\+' | grep -nE '#pragma +(optimization_level|optimize_for_size|scheduling|peephole|opt_propagation)|\basm\b|__asm|\.inc"|\bvolatile\b' || echo "clean"
git diff --name-only $BASE HEAD | grep -E '\.inc$|^orig/|^build/' && { echo "FORBIDDEN FILE"; exit 3; } || true
echo "== build"
python3 configure.py --no-progress >/dev/null
python3 tools/local_campaign.py build --worker Claude -- ninja 2>&1 | tail -1
shasum -a 1 -c config/GC6E01/build.sha1
python3 tools/local_campaign.py build --worker Claude -- ninja all_source build/GC6E01/report.json 2>&1 | tail -1
python3 - <<PY
import json
def load(p):
    r=json.load(open(p)); return {f['name']+'@'+str(f.get('metadata',{}).get('virtual_address','')):f.get('fuzzy_match_percent',0) for u in r['units'] for f in u.get('functions',[])}, r['measures']
(a,ma),(b,mb)=load('$S/report_before_merge.json'),load('build/GC6E01/report.json')
up=[k for k in b if k in a and b[k]>a[k]]; down=[k for k in b if k in a and b[k]<a[k]]
print('improved',len(up),'regressed',len(down),'| gone',len(set(a)-set(b)),'new',len(set(b)-set(a)))
for k in down: print('  REGRESSED',k,a[k],'->',b[k])
for m in ('matched_functions','complete_code_percent','complete_units'): print(' ',m,ma.get(m),'->',mb.get(m))
PY
COLO_DECOMP_ROOT=$PWD python3 ../pkmn-colosseum-recomp/tools/boot_status.py | head -1
