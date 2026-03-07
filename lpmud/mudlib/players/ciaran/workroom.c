#include "std.h"

#undef EXTRA_RESET
#define EXTRA_RESET extra_reset();

ONE_EXIT("room/church", "church",
          "Ciaran's residence",
          "This is the residence of Ciaran.\n", 1)


object tracer;

extra_reset() {
    if (!tracer) {
	tracer = clone_object("/obj/trace");
	move_object(tracer, this_object());
    }
}
