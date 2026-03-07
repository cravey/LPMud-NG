#include "../../../room/std.h"

#undef EXTRA_INIT
#define EXTRA_INIT\
       ex_init();

#undef EXTRA_RESET
#define EXTRA_RESET ex_reset();

#undef EXTRA_LONG
#define EXTRA_LONG ex_long();

int passage;

ex_reset() {
    passage=0;
}

ex_long() {
    if (passage)
      write("There is a passage leading up.\n");
}

TWO_EXIT("players/ringer/pyramid/stun1","east",
         "players/ringer/pyramid/stun3","west",
         "Secret tunnel",
"This seems to be an auxiliary tunnel used by the pyramid makers.  It\n"+
"continues east and west.\n",0)

ex_init() {
    add_action("search"); add_verb("search");
    add_action("put"); add_verb("put");
    add_action("put"); add_verb("insert");
    add_action("up"); add_verb("up");
    add_action("exam"); add_verb("exa");
    add_action("exam"); add_verb("examine");
    add_action("exam"); add_verb("l");
    add_action("exam"); add_verb("look");
}

exam(str) {
    if (str!="hole" && str!="small hole" && str !="at hole" 
        && str!="at small hole" && str!="at pentagon" 
        && str!="pentagon") return 0;
    write("The hole is five-sided and shallow.\n");
    return 1;
}

up() {
    if (!passage) return 0;
    write(
"At the top of the passage you step out into the desert.  Stones slide back\n"+
"together behind you, concealing the passage.\n");
    call_other(this_player(),"move_player",
               "up#players/ringer/pyramid/desert12");
    return 1;
}

put(str) {
    if (str != "emerald in hole") 
      return 0;
    if (present("ringer_emerald",this_player())) {
        say(call_other(this_player(),"query_name")
            + " puts an emerald into the hole.\n");
        write(
"The emerald fits into hole the perfectly. It begins to glow as a passage\n"+
"opens leading up.\n");
        say(
"The emerald fits into hole the perfectly. It begins to glow as a passage\n"+
"opens leading up.\n");
        move_object(present("ringer_emerald",this_player()),this_object());
        passage=1;
        return 1;
    }
}

search(str) {
    if (!str) str = "room";
    if (str == "room") {
        write("You find a small pentagon shaped hole in the wall.\n");
        say(call_other(this_player(),"query_name") + " searches the room.\n");
        return 1;
    }
}

