# 1 "players/kantele/inner_hall.c"
# 1 "/home/lp2/lpmud/mudlib/room/std.h"







































































































































































# 2 "players/kantele/inner_hall.c"
int shieldi;

object james;














































object shield;

reset(arg) { if (!james || !living(james)) { shield = 0; james = clone_object("obj/monster"); call_other(james, "set_name", "james"); call_other(james, "set_level", 12); call_other(james, "set_al", 12); call_other(james, "set_short", "James the butler is here"); call_other(james, "set_wc", 10); call_other(james, "set_alias","butler"); call_other(james, "add_money", random(1000)); move_object(james, this_object()); } if (arg) return; set_light(1); } short() { if (set_light(0)) return           "The inner hall"; return "dark room"; } init() { add_action("move1", "south"); add_action("move2",  "north"); add_action("move3", "west"); add_action("move4", "east"); add_action("get_shield"); add_verb("pry"); } move1() { if (call_other(this_player(), "query_level", 0) < 20) { if (james && present(james, this_object())) { write("James bars your way !\n"); write("James says: Can you please leave, Sir !\n"); return 1; } } call_other(this_player(), "move_player", "south" + "#" + "players/kantele/stairway"); return 1; } move2() {  call_other(this_player(), "move_player",  "north" + "#" +            "players/kantele/hall"); return 1; } move3() { if (call_other(this_player(), "query_level", 0) < 20) { if (james && present(james, this_object())) { write("James bars your way !\n"); write("James says: Can you please leave, Sir !\n"); return 1; } } call_other(this_player(), "move_player", "west" + "#" +           "players/kantele/dining_room"); return 1; } move4() { if (call_other(this_player(), "query_level", 0) < 20) { if (james && present(james, this_object())) { write("James bars your way !\n"); write("James says: Can you please leave, Sir !\n"); return 1; } } call_other(this_player(), "move_player", "east" + "#" +           "players/kantele/living_room"); return 1; } long(str) { if (set_light(0) == 0){ write("It is dark.\n"); return; }  write(          "You are in the inner hall, the walls are lined with shields\n" +          "It looks like the shields are loot from some sort of campaign.\n"); write("There are four obvious exits, " + "south" + ", " +  "north" + ", " + "west" + " and " + "east" + ".\n"); }
# 64 "players/kantele/inner_hall.c"

get_shield(arg) {
if (arg != "of shield") {
write("pry of what ?\n");
} else {
   if (!present("shield", this_player()) &&  shieldi < 5) {
        shieldi = shieldi +1;
        write("you pry off a shield.\n");
        shield = clone_object("obj/armour");
        call_other(shield, "set_ac",1);
        call_other(shield, "set_name", "shield");
        call_other(shield, "set_alias", "small shield");
        call_other(shield, "set_value", 70);
        call_other(shield, "set_short", "A small metal shield");
        call_other(shield, "set_weight",4);
        call_other(shield, "set_type", "shield");
        move_object(shield, this_player());
        return 1;
                                            }
   else {
        write("the shields are fastened to hard.\n");
        return 1;
        }
}
}
