#include "std.h"

object beast;
#undef EXTRA_MOVE1
#define EXTRA_MOVE1\
    if (beast && present(beast)) {\
        write("The Displacer beast shimmers and blurs !\n");\
        write("Sudenly it appears in front of you, barring your way !\n");\
        return 1;\
    }
#undef EXTRA_MOVE2
#define EXTRA_MOVE2\
   if (beast && present(beast)) {\
        write("The Displacer beast shimmers and blurs !\n");\
       write("Sudenly it appears in front of you, barring your way !\n");\
        return 1;\
     }

#undef EXTRA_MOVE3
#define EXTRA_MOVE3\
   if (beast && present(beast)) {\
        write("The Displacer beast shimmers and blurs !\n");\
        write("Sudenly it appears in front of you, barring your way !\n");\
        return 1;\
    }
#undef EXTRA_RESET
#define EXTRA_RESET\
        extra_reset();

extra_reset() {
   if (!beast || !living(beast)) {
       beast = clone_object("obj/monster");
      call_other(beast, "set_name", "displacer beast");
      call_other(beast, "set_wc",20);
      call_other(beast, "set_ac",5);
      call_other(beast, "set_short", "Kitiaras pet displacer beast is here");
      call_other(beast, "set_aggressive", 1);
      call_other(beast, "set_level",10);
      call_other(beast, "set_alias","beast");
      move_object(beast, this_object());
    }
   }

FOUR_EXIT("players/kantele/kitiaras_crossing","north",
          "players/kantele/kitiaras_lab","east",
          "players/kantele/kitiaras_slepingroom","west",
          "players/kantele/kitiaras_livingroom","south",
          "You are in a small hall",
           "You are in a small hall, you can smell some sulphur to the\n" +
           "east.", 0)
