#include "../../room/std.h"

#undef EXTRA_RESET
#define EXTRA_RESET my_reset();

TWO_EXIT("players/ringer/en_for3", "east",
         "players/ringer/en_for5", "north",
	   "You are in a calm quiet forest",
"You are in a calm quiet forest. Sunlight streams through the trees above.\n"
+"You feel great peace and solitude.\n",1)

object elf;

my_reset() {
    if (!present("elf")) {
	elf = clone_object("obj/monster");
	call_other(elf, "set_name", "peaceful elf");
        call_other(elf, "set_alias", "elf");
	call_other(elf, "set_level", 4);
	call_other(elf, "set_al", 100);
	call_other(elf, "set_short", "A peaceful elf");
	call_other(elf, "set_long",
		   "The elf is sitting peacefully on a log.\n");
	call_other(elf, "set_aggressive", 0);
        move_object(elf,this_object());
    }
}    
