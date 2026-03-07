# 1 "players/kantele/snake_room.c"
# 1 "/home/lp2/lpmud/mudlib/room/std.h"







































































































































































# 2 "players/kantele/snake_room.c"

string rope;














   
reset(arg) {  if (arg) return; set_light(0); } short() { if (set_light(0)) return           "In a room with a pit"; return "dark room"; } init() { add_action("move", "west"); add_action("down"); add_verb("down"); add_action("down"); add_verb("climb"); add_action("jump_down"); add_verb("jump"); } move() {  call_other(this_player(), "move_player", "west" + "#" +"players/kantele/kitiaras_crossing"); return 1; } long(str) { if (set_light(0) == 0){ write("It is dark.\n"); return; } if (str == "pit") { write("You look down in the pit, it seems slipery, "); write("and imposible to climb down.\n"); return; } write(          "You are in a room with a pit, the only exit seems to be west,\n" +           "and you can see a large pillar standing beside the pit.\n"); write("    The only obvious exit is " + "west" + ".\n"); }
# 25 "players/kantele/snake_room.c"


down() {
    if (!rope) {
        write("You would fall down the pit and possible hurt yourself.\n");
        return 1;
    }
    call_other(this_player(), "move_player", "down#players/kantele/snake_pit");
}

tie(str)
{
    if (str != "pillar")
        return 0;
    rope = 1;
    return 1;
}
untie(str) {
    rope = 0;
    return 1;
}


query_rope() {
    return rope;
}
jump_down(arg) {
    if (arg != "down")  {
       write("You throw yourself into the pit !\n");
       write("You hit the dirt hard......\n");
       call_other(this_player(), "hit_player",20);
call_other(this_player(), "move_player", "jump_down#players/kantele/snake_pit");
    }
}

id(str) {
    if (str == "pillar")
        return 1;
    if (str == "pit")
        return 1;
}
