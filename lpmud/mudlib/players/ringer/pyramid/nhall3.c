#include "../../../room/std.h"

#undef EXTRA_INIT
#define EXTRA_INIT\
       ex_init();

#undef EXTRA_RESET
#define EXTRA_RESET ex_reset();

int pit;

query_pit() { return pit; }

TWO_EXIT("players/ringer/pyramid/nhall2","south",
         "players/ringer/pyramid/nhall4","north",
         "North/south hallway",
"The hallway continues north and south here.  The walls are smooth and\n"+
"uniform and somewhat dusty.\n",0)

ex_init() {
    if (!pit) {
        add_action("search"); add_verb("search");
    } else {
        write("You fall into a pit.\n");
        move_object(this_player(),"players/ringer/pyramid/pit1");
    }
}

ex_reset() {
    pit=0;
    set_heart_beat(1);
}

search(str) {
    if (!str) str = "room";
    if (str == "room") {
        write(
"You notice something slightly different about the floor here.\n"+
"Upon closer examination you see that the stone slab for this section\n"+
"must be a trap door.\n");
        say(call_other(this_player(),"query_name") + " searches the room.\n");
        return 1;
    }
}

heart_beat() {
    if (call_other("players/ringer/pyramid/nhall1","query_stone")) 
      return;
    if (first_inventory(this_object())) {
        pit=1;
        do_trap();
        set_heart_beat(0);
    }
}

do_trap() {
    object ob,obn;
    say(
"\nThe floor falls out from beneath you!  You fall a long way into a pit.\n");
    ob = first_inventory(this_object());
    while (ob) {
        obn = next_inventory(ob);
        move_object(ob,"players/ringer/pyramid/pit1");
        ob = obn;
    }
}
