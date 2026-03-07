#include "std.h"

#undef EXTRA_LONG
#define EXTRA_LONG\
    if (str == "hole") {\
        write("You look up in the whole, you see only light !\n");\
        return;\
    }
object kitiara;
#undef EXTRA_RESET
#define EXTRA_RESET\
       extra_reset();

        extra_reset() {
   if (!kitiara || !living(kitiara)) {
      kitiara = clone_object("obj/monster");
      call_other(kitiara, "set_name", "kitiara");
      call_other(kitiara, "set_level",20);
      call_other(kitiara, "set_al",-1000);
      call_other(kitiara, "set_short", "Kitiara the Witch is here");
      call_other(kitiara, "set_chance",30);
      call_other(kitiara, "set_aggressive", 1);
      call_other(kitiara, "set_spell_mess1",
                "Kitiara casts a flashing glance at you !");
      call_other(kitiara, "set_spell_mess2",
                "You are hit by the flashing glance !");
      call_other(kitiara, "set_spell_dam",20);
      call_other(kitiara, "set_wc",20);
      move_object(kitiara, this_object());
      }
}
THREE_EXIT("players/kantele/kitiaras_innerhall","north",
   "players/kantele/kitiaras_smallroom","east",
           "players/kantele/kitiaras_strangeroom","west",
           "In a livingroom",
           "In Kitiaras livingroom, this is a comfortable room where it\n" +
           "should be possible to relax. the room is dimly lighted from a\n" +
           "hole in the ceiling.\n",1)

id(str) {
    if (str == "hole")
        return 1;
}
