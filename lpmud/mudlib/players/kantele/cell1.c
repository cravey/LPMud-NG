#include "std.h"

int helped;
object solar, shackles, bag, money;
#undef EXTRA_MOVE1
#define EXTRA_MOVE1\
if (call_other("players/kantele/guard_room","query_gate1")) {\
            write("The celldoor is closed.\n");\
        return 1;\
 }

#undef EXTRA_LONG
#define EXTRA_LONG\
if (call_other("players/kantele/guard_room","query_gate1")) {\
        write("A heavy steel celldoor leads north, the door is closed.\n");\
}\
if (!call_other("players/kantele/guard_room","query_gate1")) {\
            write("A heavy steel celldoor leads north, the door is open.\n");\
}\
    if (str == "celldoor" || str == "door") {\
if (call_other("players/kantele/guard_room","query_gate1")) {\
            write("The celldoor is closed.\n");\
            return 1;\
}\
write("The celldoor is open.\n");\
            return 1;\
}
#undef EXTRA_INIT
#define EXTRA_INIT add_action("open_shackles"); add_verb("open");

open_shackles(str) {
if (str && str !="shackles" || !str || helped ==0 || !(solar)) {
   return 0;
   } else {
   write("Ok\n");
   helped =0;
   write("The Solar thanks you.\n");
   write("The Solar gives you a bag.\n");
   destruct(solar);
   write("The Solar disapears in a puff of smoke !\n");
   bag = clone_object("obj/bag");
   money = clone_object("obj/money");
   call_other(money, "set_money", random(2500));
   move_object(money, bag);
   move_object(bag,this_player());
   return 1;
}
}
#undef EXTRA_RESET
#define EXTRA_RESET\
        extra_reset();


extra_reset() {
         helped =1;
        if (!solar || !living(solar)) {
            solar  = clone_object("obj/monster");
          call_other(solar, "set_name", "solar");
          call_other(solar, "set_level",19);
          call_other(solar, "set_short","A beautiful Solar is here");
          call_other(solar, "set_al", 300);
         call_other(solar, "set_wc",30);
         call_other(solar, "set_ac",3);
         call_other(solar, "set_chance",30);
         call_other(solar, "set_spell_mess1",
                "The Solar throws a fireball");
         call_other(solar, "set_spell_mess2",
                "You are hit by a fireball");
         call_other(solar, "set_spell_dam",40);
            move_object(solar,  this_object());
            shackles = clone_object("players/kantele/shackles");
        move_object(shackles, solar);
}
}

ONE_EXIT("players/kantele/dungeon_corridor.c","north",
         "A small dirty cell",
         "In a small dirty cell.\n",0)

id(str) {
return str == "celldoor" || str == "door" || str == "shackles";}
