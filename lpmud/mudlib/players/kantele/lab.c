#include "std.h"

#undef EXTRA_RESET
#define EXTRA_RESET\
    if (!present("scroll"))\
        move_object(clone_object("players/kantele/scroll"), this_object());

TWO_EXIT("players/kantele/upper_hall","east",
         "players/kantele/office2","south",
         "Kanteles laboratory",
         "You are in Kanteles laboratory, you can go south into her work\n" +
         "her workroom, or you can eneter the hall to the east. The room\n" +
         "Is covered with strange devices, an odd smell penetrates the air\n",1)
