# 1 "room/mine/tunnel9.c"
# 1 "room/mine/../std.h"







































































































































































# 2 "room/mine/tunnel9.c"

string rope;










reset(arg) {  if (arg) return; set_light( 0); } short() { if (set_light(0)) return  	 "Hole in ceiling"; return "dark room"; } init() { add_action("move1",  "west"); add_action("move2",  "east"); add_action("go_up"); add_verb("up"); } move1() {  call_other(this_player(), "move_player",  "west" + "#" + "room/mine/tunnel10"); return 1; } move2() {  call_other(this_player(), "move_player",  "east" + "#" +  	 "room/mine/tunnel14"); return 1; } long(str) { if (set_light(0) == 0){ write("It is dark.\n"); return; } if (call_other("room/mine/tunnel3", "query_rope")) write("There is a rope hanging down through the hole.\n"); write( 	 "There is a big hole in the ceiling.\n"); write("There are two obvious exits, " +  "west" + " and " +  "east" + ".\n"); }
# 20 "room/mine/tunnel9.c"

go_up() 
{
    if (!call_other("room/mine/tunnel3","query_rope")) {
        write("You can't go stright up with some kind of support.\n");
	return 1;
    }
    call_other(this_player(), "move_player", "up#room/mine/tunnel8");
    return 1;
}

tie(str)
{
    if (str != "ring" && str != "rings")
        return 0;
    rope = 1;
    return 1;
}

id(str) {
    return str == "ring" || str == "rings";
}

untie(str) {
    rope = 0;
    return 1;
}
