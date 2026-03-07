#include "std.h"

string rope;
object ogre;
int payed,bridge_is_down;

#undef EXTRA_INIT
#define EXTRA_INIT\
    add_action("pay_troll"); add_verb("pay");\
    add_action("down"); add_verb("down");\
    add_action("down"); add_verb("climb");\
    add_action("west"); add_verb("west");\
    add_action("tie"); add_verb("tie");\
    add_action("untie"); add_verb("untie");\
    add_action("jump_down"); add_verb("jump");

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
#undef EXTRA_RESET
#define EXTRA_RESET\
        extra_reset();

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

ONE_EXIT("players/kantele/dirty_temple.c","east",
         "Besides a rickety wooden bridge",
         "A rickety wooden bridge extends across the chasm to the west\n" +
         "vanishing into the darkness.  A sign posted on the bridge reads:\n" +
         "'stop! pay troll!'  ",0)
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
