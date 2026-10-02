/**
 * @file menu_exact_8007B6A4.c
 * @brief Run the GBA boot worker and publish completion.
 */
#include "dolphin/types.h"

extern void fn_8007B6D8(u8* context);

void fn_8007B6A4(u8* context)
{
    u8* saved_context = context;

    fn_8007B6D8(saved_context);
    saved_context[0x345] = 1;
}
