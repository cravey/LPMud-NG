# 1 "players/kantele/dirty_room.c"
# 1 "/home/lpmud/mudlib/room/std.h"







































































































































































# 2 "players/kantele/dirty_room.c"

object priest,mace,key;





extra_reset() {
        if (!priest || !living(priest)) {
           priest = clone_object("obj/monster");
           call_other(priest, "set_name", "priest");
           call_other(priest, "set_level",14);
           call_other(priest, "set_al", -200);
           call_other(priest, "set_short", "A blackrobed Priest is here");
           call_other(priest, "set_ac",2);
           call_other(priest, "set_wc",15);
           call_other(priest, "set_aggressive", 1);
           call_other(priest, "set_move_at_reset");
           call_other(priest, "set_chance",30);
           call_other(priest, "set_spell_mess1",
               "The priest throws a curse.");
           call_other(priest, "set_spell_mess2",
                "The priest throws a curse at you, the curse weakens you");
           call_other(priest, "set_spell_dam",40);
            move_object(priest, this_object());
        key = clone_object("obj/treasure");
        call_other(key, "set_id", "key");
        call_other(key, "set_alias", "cellar key");
        call_other(key, "set_short", "A key");
        call_other(key, "set_value", 10);
        call_other(key, "set_weight", 1);
        move_object(key, priest);
        mace = clone_object("obj/weapon");
    call_other(mace, "set_name", "mace");
    call_other(mace, "set_value",250);
    call_other(mace, "set_class",17);
    call_other(mace, "wield", "mace");
    move_object(mace, priest);
}

}
reset(arg) { extra_reset(); if (arg) return; set_light(0); } short() { if (set_light(0)) return           "A small dirty room"; return "dark room"; } init() { add_action("move", "south");  } move() {  call_other(this_player(), "move_player", "south" + "#" +"players/kantele/dirty_corridor"); return 1; } long(str) { if (set_light(0) == 0){ write("It is dark.\n"); return; }  write(          "A small diry room, It looks like this room was made for some reason\n" +          "The reason is lost to you."); write("    The only obvious exit is " + "south" + ".\n"); }
# 48 "players/kantele/dirty_room.c"
