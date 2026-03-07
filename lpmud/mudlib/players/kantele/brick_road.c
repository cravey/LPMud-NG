#include "std.h"

object rotweiler;

#undef EXTRA_RESET
#define EXTRA_RESET\
    if (!rotweiler || !living(rotweiler)) {\
	int i;\
        object collar;\
        i = 2;\
	while(i>0) {\
            i -= 1;\
	    rotweiler = clone_object("obj/monster");\
	    call_other(rotweiler, "set_name", "rotweiler");\
            call_other(rotweiler, "set_alias", "dog");\
            call_other(rotweiler, "set_level",2);\
	    call_other(rotweiler, "set_short", "A huge rotweiler");\
	    call_other(rotweiler, "set_long", "A mean hungry rotweiler is " +\
				  "snaring at you....\n");\
            call_other(rotweiler, "set_wc",8);\
            call_other(rotweiler, "set_aggressive", 1);\
	    call_other(rotweiler, "set_move_at_reset");\
	    call_other(rotweiler, "set_move_at_reset");\
	    move_object(rotweiler, this_object());\
            collar = clone_object("obj/treasure");\
           call_other(collar, "set_id", "collar");\
          call_other(collar, "set_alias", "dog collar");\
          call_other(collar, "set_short", "A heavy studded dog collar");\
          call_other(collar, "set_value", 25);\
          move_object(collar, rotweiler);\
	}\
    }



FOUR_EXIT("players/kantele/garden_west" , "west",
           "room/vill_track","north",
         "players/kantele/garden_east" , "east",
         "players/kantele/courtyard" , "south",
         "A small brickroad leading south",
         "A small brickroad leading south, to the north you can exit\n" +
    "through an iron gate. To the east and west you can see a beautiful\n" +
         "garden. ",1)
