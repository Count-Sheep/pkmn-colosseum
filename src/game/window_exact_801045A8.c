/**
 * @file window_exact_801045A8.c
 * @brief windowCheckCursor (0x801045A8 - 0x801046B8).
 *
 * Returns 1 once the window with the given menu ID has settled its cursor
 * (+0x98 and +0x99 both clear), 0 if it is gone or still busy. With
 * wait set it yields the thread until then, giving up with a log line
 * when there is no thread to yield from.
 *
 * Flags: window TU flags (-O4,p, "-opt nopeephole"); see
 * window_exact_80104318.c. Carved so it can link while the rest of the
 * window TU stays a candidate; data stays extern.
 */
#include "dolphin/types.h"
#include "game/window_search.h"

extern void GSlogWrite(const char* fmt, ...);
extern u32 GSthreadGetCurrentThread(void);
extern void _threadSwitch(void);

extern char lbl_80271E40[]; /* "%s(0x%08x)  window does not exist\n" (SJIS) */
extern char lbl_80271E64[]; /* "%s(0x%08x)  please call this from a thread\n" (SJIS) */
extern char lbl_8035B070[]; /* "windowCheckCursor" */

/* 0x801045A8 | 0x110 */
s32 windowCheckCursor(s32 id, u8 wait)
{
    u8* window;

    do {
        window = windowSearchIDInline(id);
        if (window == NULL) {
            GSlogWrite(lbl_80271E40, lbl_8035B070, id);
            return 0;
        }
        if (window[0x98] != 0) {
            return 0;
        }
        if (window[0x99] != 0) {
            return 0;
        }
        if (wait == 0) {
            break;
        }
        if (GSthreadGetCurrentThread() == 0) {
            GSlogWrite(lbl_80271E64, lbl_8035B070, id);
            break;
        }
        _threadSwitch();
    } while (1);
    return 1;
}
