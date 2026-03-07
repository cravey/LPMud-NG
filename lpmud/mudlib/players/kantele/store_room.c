#include "std.h"

#undef EXTRA_RESET
object spider;
#define EXTRA_RESET\
    if (!spider || !living(spider)) {\
       spider = clone_object("players/kantele/spider.c");\
       move_object(spider, this_object());\
    }

ONE_EXIT("players/kantele/kitchen","north",
         "A dusty store room",
         "This is a small dusty store room, it looks like it has been\n" +
         "a very long time since anyone used it.\n",1)
