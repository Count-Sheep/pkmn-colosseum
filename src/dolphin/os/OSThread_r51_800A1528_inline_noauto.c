/**
 * @file OSThread_r51_800A1528_inline_noauto.c
 * @brief OSThread.c: SetEffectivePriority, 0x800A1528 - 0x800A16E8.
 */

#include "dolphin/os/OS.h"
#include "dolphin/os/OSContext.h"
#include "dolphin/os/OSInterrupt.h"
#include "dolphin/os/OSThread.h"

/* OSThread.c's scheduler state; owned by the TU's data units. */
extern OSThreadQueue RunQueue_803FB898[32];
extern volatile u32 RunQueueBits_8047A760;
extern volatile int RunQueueHint_8047A764;
extern int Reschedule_8047A768;

#define RunQueue RunQueue_803FB898
#define RunQueueBits RunQueueBits_8047A760
#define RunQueueHint RunQueueHint_8047A764
#define Reschedule Reschedule_8047A768

void UnsetRun(OSThread* thread);

#define AddTail(queue, thread, link)                                           \
    do {                                                                       \
        OSThread* __prev;                                                      \
        __prev = (queue)->tail;                                                \
        if (__prev == NULL) {                                                  \
            (queue)->head = (thread);                                          \
        } else {                                                               \
            __prev->link.next = (thread);                                      \
        }                                                                      \
        (thread)->link.prev = __prev;                                          \
        (thread)->link.next = NULL;                                            \
        (queue)->tail = (thread);                                              \
    } while (0)

#define AddPrio(queue, thread, link)                                           \
    do {                                                                       \
        OSThread* __next;                                                      \
        OSThread* __prev;                                                      \
        for (__next = (queue)->head;                                           \
             __next != NULL && __next->priority <= (thread)->priority;         \
             __next = __next->link.next) {                                     \
            ;                                                                  \
        }                                                                      \
        if (__next == NULL) {                                                  \
            AddTail(queue, thread, link);                                      \
        } else {                                                               \
            (thread)->link.next = __next;                                      \
            __prev = __next->link.prev;                                        \
            __next->link.prev = (thread);                                      \
            (thread)->link.prev = __prev;                                      \
            if (__prev == NULL) {                                              \
                (queue)->head = (thread);                                      \
            } else {                                                           \
                __prev->link.next = (thread);                                  \
            }                                                                  \
        }                                                                      \
    } while (0)

#define RemoveItem(queue, thread, link)                                        \
    do {                                                                       \
        OSThread* __next;                                                      \
        OSThread* __prev;                                                      \
        __next = (thread)->link.next;                                          \
        __prev = (thread)->link.prev;                                          \
        if (__next == NULL) {                                                  \
            (queue)->tail = __prev;                                            \
        } else {                                                               \
            __next->link.prev = __prev;                                        \
        }                                                                      \
        if (__prev == NULL) {                                                  \
            (queue)->head = __next;                                            \
        } else {                                                               \
            __prev->link.next = __next;                                        \
        }                                                                      \
    } while (0)

#define RemoveHead(queue, thread, link)                                        \
    do {                                                                       \
        OSThread* __next;                                                      \
        (thread) = (queue)->head;                                              \
        __next = (thread)->link.next;                                          \
        if (__next == NULL) {                                                  \
            (queue)->tail = NULL;                                              \
        } else {                                                               \
            __next->link.prev = NULL;                                          \
        }                                                                      \
        (queue)->head = __next;                                                \
    } while (0)

#define SetRun(thread)                                                         \
    do {                                                                       \
        (thread)->queue = &RunQueue[(thread)->priority];                       \
        AddTail((thread)->queue, (thread), link);                              \
        RunQueueBits |= 1 << (31 - (thread)->priority);                        \
        RunQueueHint = TRUE;                                                   \
    } while (0)

OSThread* SetEffectivePriority(OSThread* thread, s32 priority)
{
    switch (thread->state) {
    case OS_THREAD_STATE_READY:
        UnsetRun(thread);
        thread->priority = priority;
        SetRun(thread);
        break;

    case OS_THREAD_STATE_WAITING:
        RemoveItem(thread->queue, thread, link);
        thread->priority = priority;
        AddPrio(thread->queue, thread, link);
        if (thread->mutex != NULL) {
            return thread->mutex->thread;
        }
        break;

    case OS_THREAD_STATE_RUNNING:
        RunQueueHint = TRUE;
        thread->priority = priority;
        break;
    }
    return NULL;
}

