# 1 "players/kantele/cellar_east.c"
# 1 "/home/lpmud/mudlib/room/std.h"







































































































































































# 2 "players/kantele/cellar_east.c"
int moved;










north() {
    if (moved == 2) {
        write("You enter the small hole.\n");
call_other(this_player(), "move_player", "north#players/kantele/cellar_passage");
return 1;
}
else {
return 1;
}
}

move(arg) {
if (arg != "barrel" && arg != "barrels" || moved == 2) {
write("move what ?\n");
        return 1;
} else {
    if (call_other(this_player(), "query_level", 0) <14) {
        write("You are not strong enough to move the barrels.\n");
        say(call_other(this_player(), "query_name", 0) +
            " tries to move the barrels, but fails.\n");
        return 1;
    }
    write("You move the barrels to reveal a secret passage leading north.\n");
    moved=2;
        say(call_other(this_player(), "query_name", 0) +
            " moves some of the barrels.\n");
        return 1;
    }
}

id(str) {
    if (str == "barrel")
        return 1;
    if (str == "barrels")
        return 1;
}









reset(arg) { moved=1; if (arg) return; set_light(0); } short() { if (set_light(0)) return           "You are in a large dark hall"; return "dark room"; } init() { add_action("move", "west"); add_action("move"); add_verb("move"); add_action("west"); add_verb("west"); add_action("north"); add_verb("north"); } move() {  call_other(this_player(), "move_player", "west" + "#" +"players/kantele/stairway_down.c"); return 1; } long(str) { if (set_light(0) == 0){ write("It is dark.\n"); return; } if (moved == 1) write("Some large barrels are lying against the north wall.\n"); if (moved == 2) write("Some large barrels are lying here, you can see a small black hole to the north.\n"); write(          "You are in a large dark hall, you can enter the stairway to the west.\n"); write("    The only obvious exit is " + "west" + ".\n"); }
# 64 "players/kantele/cellar_east.c"
west() {
call_other(this_player(), "move_player", "west#players/kantele/stairway_down");

return 1;
}
