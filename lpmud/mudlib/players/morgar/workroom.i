# 1 "players/morgar/workroom.c"
# 1 "/home/lpmud/mudlib/room/std.h"







































































































































































# 2 "players/morgar/workroom.c"




reset(arg) {  if (arg) return; set_light( 1); } short() { if (set_light(0)) return  	 "Morgar's workroom"; return "dark room"; } init() { add_action("move1", "north"); add_action("move2",  "east"); add_action("move3",  "south"); add_action("move4",  "up"); extra_init(); } move1() {  call_other(this_player(), "move_player", "north" + "#" + "room/clearing"); return 1; } move2() {  call_other(this_player(), "move_player",  "east" + "#" +  	 "players/morgar/base"); return 1; } move3() {  call_other(this_player(), "move_player",  "south" + "#" +  	 "room/post"); return 1; } move4() {  call_other(this_player(), "move_player",  "up" + "#" +           "players/morgar/store"); return 1; } long(str) { if (set_light(0) == 0){ write("It is dark.\n"); return; }  write( 	 "This is where Morgar works on strange new projects.\n"); write("There are four obvious exits, " + "north" + ", " +  "east" + ", " +  "south" + " and " +  "up" + ".\n"); }
# 12 "players/morgar/workroom.c"

extra_init() {
    add_action("torch"); add_verb("torch");
    add_action("cash"); add_verb("cash");
}

torch() {
    object torch;
    torch = clone_object("obj/torch");
    call_other(torch, "set_name", "torch");
    call_other(torch, "set_fuel",9999);
    call_other(torch, "set_weight", 1);
    move_object(torch, this_object());
    return 1;
}

cash(arg) {
    object money;
    int amount;
    amount=1000000;
    money = clone_object("obj/money");
    call_other(money, "set_money",amount);
    move_object(money,this_object());
    return 1;
}
