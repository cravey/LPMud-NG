#include "std.h"

string rope;
object snake,orb,bar;
reset(arg) {
    if (!arg)
        set_light(1);
    if (!snake || !living(snake)) {
        int i;
        i = random(10);
        while(i>0) {
            i -= 1;
            snake  = clone_object("obj/monster");
            call_other(snake, "set_name", "snake");
            call_other(snake, "set_level",random(5));
            call_other(snake, "set_short", "A small poison snake");
            call_other(snake, "set_wc",random(30));
            call_other(snake, "set_aggressive", 1);
            move_object(snake, this_object());
}
orb = clone_object("obj/treasure");
            call_other(orb, "set_name", "orb");
            call_other(orb, "set_value",750);
            call_other(orb, "set_short", "A small gold orb");
            call_other(orb, "set_alias", "gold orb");
            move_object(orb, this_object());
bar = clone_object("obj/treasure");
            call_other(bar, "set_name", "bar");
            call_other(bar, "set_value",750);
            call_other(bar, "set_short", "A gold bar");
            call_other(bar,"set_alias", "gold bar");
            move_object(bar, this_object());
}
}



long() {
    write("You are in the snake pit,  it seems impossible to get out !!\n");
    if (call_other("players/kantele/snake_room", "query_rope"))
        write("There is a rope hanging down through the hole.\n");
}
init() {
add_action("go_up"); add_verb("up");
}

short() {
    return "You are in the snake pit";
}
go_up() 
{
    if (!call_other("players/kantele/snake_room","query_rope")) {
        write("You can't go stright up with some kind of support.\n");
        return 1;
    }
    call_other(this_player(), "move_player", "up#players/kantele/snake_room");
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

