#include "../../room/std.h"

#undef EXTRA_INIT
#define EXTRA_INIT ex_init();

ONE_EXIT("players/ringer/refuge", "east",
	   "You are in a large dark cavern.",
"This is a very large cavern. It seems this was once a communal dwelling\n"
+"for a group of people.  The living conditions must have been pretty bad.\n"
+"The walls of the cavern seems to be scratched in various places.\n",0)

ex_init() {
    add_action("look"); add_verb("look");
    add_action("look"); add_verb("examine");
    add_action("look"); add_verb("l");
    add_action("look"); add_verb("exa");
}

look(str) {
    if (str == "at wall" || str=="at walls") {
        write(
"You closely examine the wall and find that some of the scratches are\n"
+"actually writings.  The people who dwelt here kept a log of their plight.\n"
+"It seems they were the servants of the evil mage Gurfnel. While he was \n");
	write(
"away on a long journey the platinum dragon came and attacked the castle. \n"
+"The castle's inhabitants were powerless to stop the dragon, so they fled \n"
+"into the mountain. Gurfnel had been seeking the only weapon that could \n");
	write(
"slay the platinum dragon.  His good cousin Gulf (whom Gurfnel loathes) \n"
+"had also left his castle to find the blade that could destroy the evil \n"
+"chromatic dragon.\n");
	write(
"Here the record ends. Whether Gurfnel returned to gather these servants or\n"
+"whether they perished is not clear.\n");
        return 1;
    }
}
