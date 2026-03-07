# 1 "players/kantele/on_bridge.c"
# 1 "/home/lpmud/mudlib/room/std.h"







































































































































































# 2 "players/kantele/on_bridge.c"

string rope;
object ogre;
int payed,bridge_is_down;











pay_troll(str) {
if (str && str =="troll" && ogre) {
write("The troll don't want you'r stinking money !\n");
return 1;
}
}

west() {
    if (call_other(this_player(), "query_level", 0) < 20) {
    if (bridge_is_down == 1) {
    write("The bridge is down.\n");
    return 1;
    } else {
    if (ogre  && present(ogre, this_object()) && payed ==1) {
        write("The Troll bars your way !\n");
        return 1;
   }
   }
   }
call_other(this_player(), "move_player", "west#players/kantele/west_bridge");
    return 1;
}




extra_reset() {
payed=1;
bridge_is_down=0;
        if (!ogre  || !living(ogre)) {
           ogre  = clone_object("obj/monster");
           call_other(ogre, "set_name", "troll");
           call_other(ogre, "set_level",19);
           call_other(ogre, "set_al", -300);
          call_other(ogre, "set_short", "A large Troll is here");
           call_other(ogre, "set_wc",25);
           call_other(ogre, "set_ac",2);
           call_other(ogre, "add_money", random(1000)+100);
            move_object(ogre, this_object());
         }
}

reset(arg) { extra_reset(); if (arg) return; set_light(0); } short() { if (set_light(0)) return           "Besides a rickety wooden bridge"; return "dark room"; } init() { add_action("move", "east"); add_action("pay_troll"); add_verb("pay"); add_action("down"); add_verb("down"); add_action("down"); add_verb("climb"); add_action("west"); add_verb("west"); add_action("tie"); add_verb("tie"); add_action("untie"); add_verb("untie"); add_action("jump_down"); add_verb("jump"); } move() {  call_other(this_player(), "move_player", "east" + "#" +"players/kantele/dirty_temple.c"); return 1; } long(str) { if (set_light(0) == 0){ write("It is dark.\n"); return; }  write(          "A rickety wooden bridge extends across the chasm to the west\n" +          "vanishing into the darkness.  A sign posted on the bridge reads:\n" +          "'stop! pay troll!'  "); write("    The only obvious exit is " + "east" + ".\n"); }
# 66 "players/kantele/on_bridge.c"
down() {
    if (!rope) {
        write("You would fall down the chasm and possible hurt yourself.\n");
        return 1;
    }
    call_other(this_player(), "move_player", "down#players/kantele/bottom_chasm");
}

tie(str)
{
    if (str != "bridge")
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
       write("You throw yourself into the chasm !\n");
       write("You hit the dirt hard......\n");
       call_other(this_player(), "hit_player",20);
call_other(this_player(), "move_player", "jump_down#players/kantele/bottom_chasm")
;
    }
}

id(str) {
    if (str == "bridge")
        return 1;
    if (str == "chasm")
        return 1;
}
