#include "../../room/std.h"

#undef EXTRA_RESET
#define EXTRA_RESET\
        my_reset();

TWO_EXIT("players/ringer/groo5", "south",
         "players/ringer/trap2", "north",
	   "You are in a long hall",
"You are one end of a long hall.  A secret passage leads south.\n",0)

object dwarf;
int in;

my_reset() {
    if (!dwarf || !living(dwarf)) {
        dwarf = clone_object("obj/monster.talk.c");
        in = 1;
        call_other(dwarf,"set_name","short dwarf");
        call_other(dwarf,"set_alias","dwarf");
        call_other(dwarf,"set_short","short dwarf");
     call_other(dwarf,"set_long","The dwarf is small and scrawny looking.\n");
        call_other(dwarf,"set_level",4);
        call_other(dwarf,"set_ac",3);
        call_other(dwarf,"set_wc",1);
        call_other(dwarf,"set_object",this_object());
        call_other(dwarf,"set_function","run");
        call_other(dwarf,"set_type","arrives");
        call_other(dwarf,"set_match","");
        move_object(dwarf,this_object());
    }
}    

run(str) {
    string who;


    if (in < 3) {
        say("\nThe dwarf screams in anger!\nHe says "
            +"You'll never get my treasure you swine!\n");
        say("The dwarf runs north\n");
        if (in == 2) {
            move_object(dwarf,"players/ringer/trap3");
            in = in+1;
        }
        if (in == 1) {
            move_object(dwarf,"players/ringer/trap2");
            in=in+1;
        }
        return;
    }
    if (call_other("players/ringer/trap3","query_door")) {
        say("\n");
        call_other(dwarf,"init_command","push button");
        say("The dwarf laughs heartily! 'You fool!' he shouts.\n");
        say("The dwarf leaves north.\n");
        destruct(dwarf);
    }
}
