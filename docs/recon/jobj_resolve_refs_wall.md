# JObj reference-resolver closure

`HSD_JObjResolveRefsAll` (0x801A0744, 0x458 bytes) and
`HSD_JObjResolveRefs` (0x801A0D94, 0x228 bytes) belong to the retail
`jobj.c` translation unit. The active `hsd_jobj_residual_801A0744.c` and
`hsd_jobj_residual_801A0D94.c` files only include `src/hsd/jobj.c` for
score instrumentation; their text-only split boundaries are not evidence of
separate retail TUs. Both are `CodeCandidate`, not linked.

The 2026-09-28 report gives 99.98921% and 99.99275% fuzzy match respectively.
These near-100 scores are not acceptance. In objdiff's raw comparison the
functions are 99.66547% and 99.702896%: branch destinations shift because
the candidate is compiled inside the much larger included unit; source pool
references also differ. For example, the retail assert-string offset is
`0x210` from the pool base where the candidate uses `0x184`. The retail
`lbl_80274AA0`/`lbl_8047DB20` relocations show up as compiler-generated
pool labels in the candidate. Some out-of-line inline-helper call names also
differ (`ref_INC` versus `ref_INC_nocheck`).

The original `jobj.c` owner contains many other nonmatching functions and
unrecovered orphaned retail strings that affect pool offsets, as documented
at the top of `src/hsd/jobj.c`. No source-faithful change to just the two
resolver bodies can prove whole-object linkage while those dependencies
remain. Their current traversal and reference semantics were preserved; no
score-only reordering, forced offset, pragma, or assembly was introduced.

On 2026-09-29, a guarded `objdiff-cli diff` inspection found no opcode,
register, or instruction-order differences in either resolver. The raw
`DIFF_ARG_MISMATCH` rows are:

| Function | Retail instruction indices | Remaining dependency |
| --- | --- | --- |
| `HSD_JObjResolveRefs` | 9-10, 63-64, 105-106, 118-119 | The retail `lbl_80274AA0` pool base and `lbl_8047DB20` filename, including assert-string offset `0x210` versus candidate `0x184`. |
| `HSD_JObjResolveRefsAll` | 2, 6, 61-62, 103-104, 116-117, 179-180, 220-221 | The same pool base, filename, and offsets. |
| `HSD_JObjResolveRefsAll` | 148, 152, 156, 159, 170, 186 | Cross-split calls: retail names `ref_DEC_801A0D48`, `fn_801A0D3C`, `hsdDelete_801A0CE8`, `iref_INC_801A0C9C`, and `ref_INC`; the candidate names the corresponding whole-TU helper copies, including `ref_INC_nocheck`. |

The candidate's `.rodata`, `.data`, `.sbss`, and `.sdata2` are emitted by the
included whole `jobj.c`, while each target resolver split is text-only. Thus
promoting either current split would discard real retail TU data ownership.
The remaining evidence points to restoring the entire `jobj.c` pool and
function topology, not adjusting the already aligned resolver source. The
known unrecovered strings and nonmatching owner peers are a Decomp blocker;
inventing their source merely to force offsets would violate the rules.

Shared-lock `ninja -j2 all_source build/GC6E01/report.json`, `ninja -j2`,
and retail SHA-1 checks passed (`main.dol` and `common_rel.rel`: OK). Those
checks cover the current linked image, not acceptance of either resolver.
