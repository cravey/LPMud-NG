#include "std.h"

object haunt;
/*
*#undef EXTRA_MOVE3
*#define EXTRA_MOVE3\
*    if (haunt && present(haunt)) {\
*        write("The haunt is in the way  !\n");\
*        return 1;\
*    }
*/

#undef EXTRA_MOVE2
#define EXTRA_MOVE2\
    if (call_other(this_player(), "query_level", 0) < 20) {\
    if (haunt && present(haunt)) {\
        write("The haunt is in the way  !\n");\
        return 1;\
   }\
    }


#undef EXTRA_RESET
#define EXTRA_RESET\
   if (!haunt || !living(haunt)) {\
       haunt = clone_object("obj/monster");\
      call_other(haunt, "set_name", "haunt");\
      call_other(haunt, "set_short", "A ghostly haunt is here");\
      call_other(haunt, "set_aggressive", 1);\
      call_other(haunt, "set_level",12);\
      call_other(haunt, "set_wc", 15);\
      call_other(haunt, "set_ac", 2);\
      call_other(haunt, "set_chance",20);\
      call_other(haunt, "set_spell_mess1",\
                "The haunt utters a wailing scream.");\
      call_other(haunt, "set_spell_mess2",\
                "The scream hurts you bad.");\
      call_other(haunt, "set_spell_dam",40);\
            move_object(haunt, this_object());\
    }

THREE_EXIT("players/kantele/stairway.c","up",
         "players/kantele/cellar_west","west",
          "players/kantele/cellar_east","east",
         "Bottom of stairway",
         "You are at the bottom of the stairway,  the stairway goes up\n" +
         "and you can go further into the cellar to the west.\n",0)
