# 1 "room/well.c"
# 1 "room/std.h"







































































































































































# 2 "room/well.c"





















reset(arg) {  if (arg) return; set_light( 0); } short() { if (set_light(0)) return  	 "Down the well"; return "dark room"; } init() { add_action("move1",  "up"); add_action("move2",  "north"); add_action("west", "west"); add_action("open", "open"); add_action("close", "close"); add_action("pull", "pull"); add_action("pull", "turn"); add_action("pull", "move"); } move1() {  call_other(this_player(), "move_player",  "up" + "#" + "room/narr_alley"); return 1; } move2() {  call_other(this_player(), "move_player",  "north" + "#" +  	 "room/maze1/maze1"); return 1; } long(str) { if (set_light(0) == 0){ write("It is dark.\n"); return; } if (str == "lever") { write("The lever can be moved between two positions.\n"); return; } if (str == "door") { if (call_other("room/sub/door_trap", "query_west_door")) write("The door is closed.\n"); else write("The door is open\n"); } write( 	 "You are down the well. It is wet and slippery.\n" + 	 "There is a lever beside a door to the west.\n"); write("There are two obvious exits, " +  "up" + " and " +  "north" + ".\n"); }
# 30 "room/well.c"

west() {
    if (call_other("room/sub/door_trap", "query_west_door") == 0) {
	call_other(this_player(), "move_player", "west#room/sub/door_trap");
	return 1;
    }
    write("The door is closed.\n");
    return 1;
}

close(str) {
    if (!str && str != "door")
	return 0;
    write("You can't.\n");
    return 1;
}

open(str) {
    if (!str && str != "door")
	return 0;
    write("You can't.\n");
    return 1;
}

pull(str) {
    if (!str || str != "lever")
	return 0;
    call_other("room/sub/door_trap", "toggle_door");
    return 1;
}

id(str) {
    return str == "lever" || str == "door";
}
