#include "../../../room/std.h"

#undef EXTRA_INIT
#define EXTRA_INIT\
       ex_init();

#undef EXTRA_LONG
#define EXTRA_LONG ex_long();

#undef EXTRA_RESET
#define EXTRA_RESET ex_reset();

int passage;

ex_reset() {
    passage=0;
}

ex_long() {
    if (passage) 
        write("A low passage leads north.\n");
}

TWO_EXIT("players/ringer/pyramid/whall1","east",
         "players/ringer/pyramid/whall3","west",
         "East/west hallway",
"The hallway continues east and west here.  The walls are smooth and\n"+
"uniform and somewhat dusty.\n",0)

ex_init() {
    add_action("search"); add_verb("search");
    add_action("north"); add_verb("north");
}

north() {
    if (!passage)  return 0;
    call_other(this_player(),"move_player",
               "north#players/ringer/pyramid/spass1");
    return 1;
}

search(str) {
    if (!str) str = "room";
    if (str == "room") {
        write(
   "You notice something slightly different about the walls to the west.\n"+
   "They seem to be pitted with dozens of small holes on both sides of \n"+
   "the corridor.\n");
        if (call_other("players/ringer/pyramid/whall1","query_stone")==0 &&
            !passage) {
            write(
"\nAs you are about to end your search a peculiarity catches your eye.\n"+
"You notice that one of the slabs to the north is set back from the \n"+
"others about an inch.  You push on it cautiously and reveal a\n"+
"passage.\n");
            passage=1;
        }
        say(call_other(this_player(),"query_name") + " searches the room.\n");
        return 1;
    }
}
