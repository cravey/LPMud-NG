#include "std.h"

string rope;
reset(arg) {
    if (!arg)
        set_light(1);
}



long() {
    write("You are in the bottom of the chasm, it seems impossible to get out !!\n");
    if (call_other("players/kantele/on_bridge", "query_rope"))
       write("There is a rope hanging down from the bridge.\n");
}
init() {
add_action("go_up"); add_verb("up");
}

short() {
    return "You are in the bottom of the chasm";
}
go_up() 
{
    if (!call_other("players/kantele/on_bridge","query_rope")) {
        write("You can't go stright up with some kind of support.\n");
        return 1;
    }
    call_other(this_player(), "move_player", "up#players/kantele/on_bridge");
}
tie(str)
{
    if (str != "pillar")
        return 0;
    rope = 1;
    return 1;
}

id(str) {
    return str == "pillar";
}

untie(str) {
    rope = 0;
    return 1;
}

