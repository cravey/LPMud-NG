#include "../../room/std.h"

#undef EXTRA_INIT
#define EXTRA_INIT ex_init();

FOUR_EXIT("players/ringer/long_hill", "east",
          "players/ringer/en_stream", "south",
	  "players/ringer/en_forest", "west", 
          "players/ringer/aa","north",
	   "You are in a delightful valley",
"This valley is beautiful to behold.  Scents of blossoms fill the air.\n"
+"Birds sing sweetly in the trees.  You sense a magical quality about the\n"
+"place. To the south a stream gurggles softly.  There is a sign here.\n"
+"There is a small white office building to the north.\n",1)

ex_init() {
    add_action("read"); add_verb("read");
    add_action("look"); add_verb("look");
}

look(str) {
    if (str=="at sign") {
        read("sign");
        return 1;
    }
}

read(str) {
    if (str == "sign") {
        write(
"The sign says: \n"
+" Enchanted forest: west\n");
        return 1;
    }
}
