#include "lint.h"

/*
 * Compatibility shim for builds using the system allocator.
 * Keep these symbols so driver commands that expect allocator stats
 * still link and return sensible output.
 */
int resort_free_list(void)
{
    return 0;
}

void dump_malloc_data(void)
{
    add_message("Allocator: system malloc/free/realloc (libc)\n");
}
