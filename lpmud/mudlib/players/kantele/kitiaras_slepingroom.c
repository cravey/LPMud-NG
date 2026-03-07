#include "std.h"
object ring;
int found;

#undef EXTRA_LONG
#define EXTRA_LONG\
    if (str == "bed") {\
       write("The bed is covered with clean silken sheets, it smells of roses.\n");\
       return;\
   }

#undef EXTRA_RESET
#define EXTRA_RESET\
found = found-1;\

#undef EXTRA_INIT
#define EXTRA_INIT\
    add_action("search_bed"); add_verb("search");\

ONE_EXIT("players/kantele/kitiaras_innerhall","east",
         "A tidy sleepingroom",
      "A tidy sleepingroom, it has a feminine look and a large pink bed.\n", 1)

search_bed(arg) {
    if (arg == "bed" && found <1)  {
       found =3;
       ring = clone_object("players/kantele/ring");
        move_object(ring, this_object());
        write("You found a small ring.\n");
      return 1;
    } else
    if (arg == "bed")  {
        write("You found nothing.\n");
        return 1;
           }
    }
id(str) {
    if (str == "bed")
        return 1;
}
