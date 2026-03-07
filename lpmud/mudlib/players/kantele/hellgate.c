#include "std.h"

int gate_is_open;
object gatekeeper,robe;

#undef EXTRA_LONG
#define EXTRA_LONG\
    if (str == "gate" && gate_is_open == 1) {\
       write("The gate is shimering black sinister.\n");\
       return;\
   }\
    if (str == "gate" && gate_is_open ==0) {\
       write("The gate looks compleatly dead.\n");\
       return;\
   }

#undef EXTRA_INIT
#define EXTRA_INIT\
    add_action("enter"); add_verb("enter");\
    add_action("open_gate"); add_verb("open");\
    add_action("close_gate"); add_verb("close");\

#undef EXTRA_RESET
#define EXTRA_RESET\
        extra_reset();

extra_reset() {
        if (!gatekeeper || !living(gatekeeper)) {
           gatekeeper = clone_object("obj/monster");
           call_other(gatekeeper, "set_name", "gatekeeper");
           call_other(gatekeeper, "set_level",19);
           call_other(gatekeeper, "set_short", "The grayrobed Gatekeeper is here");
           call_other(gatekeeper, "set_long","He looks VERY tough");
           call_other(gatekeeper, "set_wc", 35);
           call_other(gatekeeper, "set_ac",2);
           call_other(gatekeeper, "set_hp",450);
            move_object(gatekeeper, this_object());
            robe  = clone_object("obj/armour");
           call_other(robe, "set_name", "robe");
           call_other(robe, "set_short","A grey robe");
           call_other(robe, "set_value",1000);
           call_other(robe, "set_weight",2);
           call_other(robe, "set_ac",1);
           call_other(robe, "set_type","robe");
            move_object(robe,gatekeeper);
        }
}

ONE_EXIT("players/kantele/hellgate_anteroom.c","south",
         "Hellgate",
         "You are at hellgate, the gate stands black and sinister to the north.\n" +
         "This is the place where gods expel people they don't want on earth.\n", 1)

enter() {
        if (gate_is_open ==0) {
        write("the gate is closed\n");
        return 1;
        } else {
        write("you get a tingeling feeling as you enter the gate.\n");
        call_other(this_player(), "move_player", "X#players/kantele/hell1");
        return 1;
     }
  }

open_gate(str) {
     if(str && str =="gate") {
    if (call_other(this_player(), "query_level", 0) > 20) {
gate_is_open =1;
say(call_other(this_player(), "query_name") + " has opened the gate.\n");
write("You have opened the gate.\n");
return 1;
} else {
say(call_other(this_player(), "query_name") + " tries to open the gate. but fails.\n");
write("The power to open the gate don't belong to you.\n");
return 1;
}
}
}

close_gate(str) {
     if(str && str =="gate") {
    if (call_other(this_player(), "query_level", 0) > 20) {
gate_is_open =0;
say(call_other(this_player(), "query_name") + " has closed the gate.\n");
write("You have closed  the gate.\n");
return 1;
} else {
say(call_other(this_player(), "query_name") + " tries to close the gate. but fails.\n");
write("The power to open the gate don't belong to you.\n");
return 1;
}
}
}
id(str) {
    if (str == "gate")
        return 1;
}
