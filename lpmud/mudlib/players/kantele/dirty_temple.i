# 1 "players/kantele/dirty_temple.c"
# 1 "/home/lpmud/mudlib/room/std.h"







































































































































































# 2 "players/kantele/dirty_temple.c"




object fiend;

extra_reset() {
        if (!fiend || !living(fiend)) {
           fiend = clone_object("obj/monster");
           call_other(fiend, "set_name", "pit fiend");
           call_other(fiend, "set_name", "pit fiend");
           call_other(fiend, "set_alias", "demon");
           call_other(fiend, "set_level",16);
           call_other(fiend, "set_al", -300);
           call_other(fiend, "set_short", "A huge Pit Fiend is here");
           call_other(fiend, "set_aggressive", 1);
           call_other(fiend, "set_wc",20);
           call_other(fiend, "set_ac",2);
           call_other(fiend, "set_chance",30);
           call_other(fiend, "set_spell_mess1",
               "The Pit Fiend casts a firebolt at you.");
          call_other(fiend, "set_spell_mess2",
                "You are hit by a firebolt");
          call_other(fiend, "set_spell_dam",15);
            move_object(fiend,this_object());
}
}

reset(arg) { extra_reset(); if (arg) return; set_light(1); } short() { if (set_light(0)) return           "In the Evil Temple"; return "dark room"; } init() { add_action("move1", "north"); add_action("move2", "south"); add_action("move3", "west");  } move1() {  call_other(this_player(), "move_player", "north" + "#" + "players/kantele/dirty_anteroom"); return 1; } move2() {  call_other(this_player(), "move_player", "south" + "#" +             "players/kantele/behind_altar"); return 1; } move3() {  call_other(this_player(), "move_player", "west" + "#" +             "players/kantele/on_bridge.c"); return 1; } long(str) { if (set_light(0) == 0){ write("It is dark.\n"); return; }  write(          "You are in a Evil Temple, behind the alter to the south you can \n" +       "see a huge statue of Kali. to the west a rickety bridge leads into\n" +          "the darkness. The bridge looks wery frail.\n"); write("There are three obvious exits, " + "north" + ", " + "south" + " and " + "west" + ".\n"); }
# 38 "players/kantele/dirty_temple.c"
