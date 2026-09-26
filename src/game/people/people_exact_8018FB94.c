#include "game/people/people.h"

u8 peopleTestFlags(PeopleEntry* entry, u32 mask)
{
    return (entry->flags & mask) != 0;
}
