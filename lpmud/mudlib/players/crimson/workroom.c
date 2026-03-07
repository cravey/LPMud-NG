#include "std.h"
#include "lc.h"

#undef EXTRA_RESET
#define EXTRA_RESET\
    special_reset();

#undef EXTRA_INIT
#define EXTRA_INIT\
   add_action("up"); add_verb("up");\
   add_action("east"); add_verb("east");\
   add_action("post"); add_verb("post");\
   add_action("thunder"); add_verb("thunder");\
   if(call_other("players/crimson/thunderdome", "query_chall") == 1)\
   write("HEY! There is a challenge going on in THUNDERDOME.\n");

up(str) {
   if (call_other(this_player(), "query_real_name") != "crimson") {
    return 1;
   } else
   call_other(this_player(), "move_player", "up#players/crimson/storeroom");
   return 1;
}

east(str) {
   if (call_other(this_player(), "query_real_name") != "crimson") {
    return 1;
   } else
   call_other(this_player(), "move_player", "east#players/crimson/hideyhole");
   return 1;
}

thunder(str) {
   call_other(this_player(), "move_player", "to THUNDERDOME#players/crimson/thunderdome");
   return 1;
}

post() {
   if (call_other(this_player(), "query_real_name") != "crimson") {
   return 0;
   } else
   cat("/room/post_dir/crimson.o");
}

extra_reset() {
object orc;
         if (!orc || !living(orc)) {
         orc = clone_object("players/crimson/invaders");
         call_other(orc, "set_name", "orc");
         call_other(orc, "set_short", "An ugly snout nosed ORC!");
         call_other(orc, "set_level", 1);
         call_other(orc, "set_long", "He looks to be a truely foul creature.\n");
         call_other(orc, "set_wc", 9);
         call_other(orc, "set_ac", 3);
         call_other(orc, "set_hp", 30);
         call_other(orc, "set_aggressive", 1);
         move_object(orc, this_object());
         }
}
FOUR_EXIT("room/plane4", "west",
          "room/pub2", "north",
          "room/shop", "south",
          "room/church", "down",
          "Velcome to my Coffin!",
          "Velcome to my Coffin!\n"+
          "..... reads a sign over a shiny black coffin.\n" +
          "The CHURCH is DOWN.\n" +
          "The shop is south.\n" +
          "The pub is to the north.\n" +
          "And Crimson's castle is west.\n", 1)
