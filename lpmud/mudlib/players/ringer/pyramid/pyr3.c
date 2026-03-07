#include "../../../room/std.h"

#undef EXTRA_INIT
#define EXTRA_INIT\
       ex_init();

TWO_EXIT("players/ringer/pyramid/pyr2","south",
         "players/ringer/pyramid/pyr4","north",
         "Long hallway",
"This is a long stone hallway.  The floor and walls seem made of large\n"+
"slabs of sandstone.  The workmanship is awesome.  The seams between stones\n"+
"are straight and smooth.\n",0)

ex_init() {
    if (random(100)>=50) {
        write(
"As you venture through the passage you hear stone sliding on stone. You \n"+
"instinctively draw back just as several darts fly past, inches away from\n");
        write(
"your face.  You notice two holes in the hall on either side -- one from \n"+
"which the darts came and the other into which they disappeared.  Stones\n"+
"slide back into place and the holes disappear.\n");
    }
    add_action("search"); add_verb("search");
    add_action("exam"); add_verb("exa");
    add_action("exam"); add_verb("examine");
    add_action("exam"); add_verb("l");
    add_action("exam"); add_verb("look");
}

search(str) {
    if (!str) str = "room";
    if (str == "room") {
        l_wall();
        say(call_other(this_player(),"query_name") + " searches the room.\n");
        return 1;
    }
}

l_wall() {
    write(
"There is something different about the walls, but it is so slight that you\n"+
"cannot tell just exactly what is different.\n");
}

exam(str) {
    if (str!="wall" && str!="at wall") return 0;
    l_wall();
    return 1;
}
