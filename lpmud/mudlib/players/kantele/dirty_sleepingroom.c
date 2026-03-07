#include "std.h"

#undef EXTRA_LONG
#define EXTRA_LONG\
    if (str == "bed") {\
       write("the bed is dirty and smelling, and you should be ashamed\n");\
       write("to mess around in it.\n");\
       return;\
   }
ONE_EXIT("players/kantele/mechanic_room" , "west",
         "A dirty sleeping room",
        "A dirty sleeping room, a single dirty bed stands in one corner.\n" +
        "the only exit is to the west.\n",1)

id(str) {
    if (str == "bed")
        return 1;
}
