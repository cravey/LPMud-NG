#include "../../room/std.h"

#undef EXTRA_RESET
#define EXTRA_RESET\
        ex_reset();

#undef EXTRA_INIT
#define EXTRA_INIT\
        ex_init();

ONE_EXIT("players/ringer/refuge", "south",
	   "You are in a hidden room.",
"This room was hidden by a secret doorway.  There seems to be another room\n"
+"to the north.\n",0)

object xorn;

ex_init() {
    add_action("north"); add_verb("north");
}

ex_reset() {
    int i;
    if (!present("xorn")) {
        xorn = clone_object("obj/monster");
        call_other(xorn,"set_name","xorn");
        call_other(xorn,"set_alias","xorn");
        call_other(xorn,"set_level",7);
        call_other(xorn,"set_wc",5);
        call_other(xorn,"set_ac",15);
        call_other(xorn,"set_long","The Xorn is large and mean."
                   +" His hide is tough as rock!\n");
        call_other(xorn,"set_aggressive",1);
        call_other(xorn,"set_al",-300);
        move_object(xorn,this_object());
    }
}

north() {
    if (present("xorn")) {
        write("This xorn won't let you!\n");
        return 1;
    }
    call_other(this_player(),"move_player","north#players/ringer/reftr");
    return 1;
}



