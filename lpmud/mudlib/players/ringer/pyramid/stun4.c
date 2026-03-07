#include "../../../room/std.h"

#undef EXTRA_INIT
#define EXTRA_INIT\
       ex_init();

#undef EXTRA_RESET
#define EXTRA_RESET\
        my_reset();

#undef EXTRA_LONG
#define EXTRA_LONG ex_long();

int chute;

ex_long() {
    if (chute) {
        write("A dark cooridor leads south.\n");
    }
}

ONE_EXIT("players/ringer/pyramid/stun3","down",
         "Small stone room",
"This is a small cubical room with various decorations on the walls.\n",1)


ex_init() {
    add_action("search"); add_verb("search");
    add_action("south"); add_verb("south");
}

south() {
    if (!chute) return 0;
    call_other(this_player(),"move_player",
               "south#players/ringer/pyramid/emerald_entry");
    return 1;
}

object guard;

search(str) {
    if (!str) str = "room";
    if (str != "room")  return 0;
    if (present("guard")) {
        write("The emerald guard prevents you.\n");
        return 1;
    }
    if (random(100)>50) {
        write("You find nothing unusual.\n");
    } else {
        write(
"You find a passage to the south.\n");
        chute=1;
    }
    say(call_other(this_player(),"query_name") + " searches the room.\n");
    return 1;
}

object item;

my_reset() {
    chute=0;
    if (!present("emerald guard")) {
        guard = clone_object("obj/monster");
        call_other(guard,"set_name","emerald guard");
        call_other(guard,"set_alias","guard");
        call_other(guard,"set_level",5);
        call_other(guard,"set_ac",5+random(3));
        call_other(guard,"set_wc",9+random(5));
        call_other(guard,"set_long",
"The guard's skin is stained green and he wears an emerald green tunic.\n");
        call_other(guard,"set_aggressive",1);
        call_other(guard,"set_al",-175);
        move_object(guard,this_object());
        item = clone_object("obj/armor");
        call_other(item,"set_name","bright green shield");
        call_other(item,"set_type","shield");
        call_other(item,"set_short","A bright green shield");
        call_other(item,"set_long",
                   "The shield is bright and green.  It glows softly.\n");
        call_other(item,"set_alias","green shield");
        call_other(item,"set_ac",1 + random(2));
        call_other(item,"set_value",125+random(50));
        call_other(item,"set_weight",4);
        call_other(item,"set_arm_light",1);
        move_object(item,guard);
    }
}
