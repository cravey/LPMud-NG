#include "../../room/std.h"

#undef EXTRA_INIT
#define EXTRA_INIT\
            ex_init();

TWO_EXIT("players/ringer/en_stream", "north",
         "players/ringer/dn_valley","south",
	   "You are in a clear running stream",
"You are in a clear running stream.  Its waters soothe your aching body.\n",
1)

ex_init() {
    add_action("drink"); add_verb("drink");
}


drink(arg) {
    int al,h;
    string str;
    if (!arg) arg  = "water";
    if (arg == "water") {
        say(call_other(this_player(),"query_name") 
            + " drinks from the stream\n");
        al = call_other(this_player(),"query_alignment");
        h = -3; 
        str = "The water is too clean for your taste.";
        if (al > 0) { h=1; str = "Ah!";}
        if (al > 100) { h = 2; str = "Now that's good water!";}
        if (al > 400) { h = 5; str = "You feel invigorated!";}
        if (al > 800) { h = 10; str = "You love this water!";}
        if (random(100) < 81) {
            str = "Just tastes like water...";
            h = 0;
        }
        if (random(100) > 94) {
            str = "Ouch that water burns!";
            h = -10;
        }
        write (str + "\n");
        call_other(this_player(),"heal_self",h);
    }
    return 1;
}
