# 1 "players/ringer/pyramid/desert11.c"
# 1 "players/ringer/pyramid/../../../room/std.h"







































































































































































# 2 "players/ringer/pyramid/desert11.c"












reset(arg) { ex_reset(); if (arg) return; set_light(1); } short() { if (set_light(0)) return            "A vast desolate desert"; return "dark room"; } init() { add_action("move1", "south"); add_action("move2", "east"); ex_init(); } move1() {  call_other(this_player(), "move_player", "south" + "#" + "players/ringer/pyramid/desert7"); return 1; } move2() {  call_other(this_player(), "move_player", "east" + "#" +            "players/ringer/pyramid/desert5"); return 1; } long(str) { if (set_light(0) == 0){ write("It is dark.\n"); return; } ex_long(); write( "A desert stretches far into the distance to the south and west. The sand\n"+ "dunes are very large here.\n"); write("There are two obvious exits, " + "south" + " and " + "east" + ".\n"); }
# 20 "players/ringer/pyramid/desert11.c"

int dig;

ex_long() {
    if (dig >= 10) {
        write(
"The top part of a pyramid has been excavated here. A dark entry leads\n"+
"down.\n");
        return;
    }
    if (dig > 0) {
        write("It looks as if someone has been excavating the area.\n");
        return;
    }
}


ex_init() {
    add_action("down"); add_verb("down");
    add_action("search"); add_verb("search");
    add_action("dig"); add_verb("dig");
}

down() {
    if (dig >= 10) {
        call_other(this_player(),"move_player",
                   "down#players/ringer/pyramid/pyr1");
        return 1;
    }
}


ex_reset() {
    dig = 0;
}

search(str) {
    if (!str) str = "room";
    if (str == "room") {
        write(
              "You notice that mixed in with the sand are some larger chips of stone.\n");
        write(
              "Beyond that you find nothing of interest.\n");
        say(call_other(this_player(),"query_name") + " searches the room.\n");
        return 1;
    }
}

dig() {
    write("You dig.\n");
    say(call_other(this_player(),"query_name") + " digs in the sand dune.\n");
    dig = dig + 1;
    if (dig > 10) {
        write(
"A whole stone entry way has appeared.  It seems to be the tomb of some\n"+
"desert dwelling king!  From the way it looks you are standing near the top\n"+
"of a large pyramid buried by years of shifting sand. There is a dark\n"+
"passage leading down\n");
        return 1;
    }
    if (dig > 7) {
        write(
"One large stone is now uncovered, but there seems to be others below it.\n");
        return 1;
    }
    if (dig > 5) {
        write("A large stone appears to be buried in the sand.\n");
        return 1;
    }
    return 1;
}







