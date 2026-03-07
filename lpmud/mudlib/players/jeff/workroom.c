#include "std.h"

#undef EXTRA_RESET
#define EXTRA_RESET extra_reset();

TWO_EXIT("room/ruin", "north",
         "room/pub2", "west",
          "Jeff's residence",
          "Jeff's workroom. To the north is a ruin, west a pub.\n", 1)


object tracer;

extra_reset() {
    if (!tracer) {
	tracer = clone_object("/obj/trace");
	move_object(tracer, this_object());
    }
}
