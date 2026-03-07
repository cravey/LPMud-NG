#include "std.h"

string rope;

#undef EXTRA_INIT
#define EXTRA_INIT\
    add_action("go_up"); add_verb("up");

#undef EXTRA_LONG
#define EXTRA_LONG\
    if (call_other("players/crimson/greeting", "query_rope"))\
        write("There is a rope hanging down through the hole in the cavern ceiling.\n");\

TWO_EXIT("players/crimson/underworld5", "west",
         "players/crimson/iceunder2", "north",
	 "Hole in ceiling",
         "You stare up into the darkness and see a small cirle of light.\n" +
         "It is much cooler here than up topside. A faint bubbling sound \n" +
         "can be heard in the distance. The ground is moist and the cavern\n" +
         "walls are smooth.\n", 0)

go_up() 
{
    if (!call_other("players/crimson/greeting", "query_rope")) {
        write("Hmmm, someone took your rope.\n");
	return 1;
    }
    call_other(this_player(), "move_player", "up the rope#players/crimson/underworld6");
    return 1;
}

tie(str)
{
    if (str != "fountain" && str != "vat")
        return 0;
    rope = 1;
    return 1;
}

id(str) {
    return str == "fountain" || str == "vat";
}

untie(str) {
    rope = 0;
    return 1;
}
realm() { return "NT"; }
