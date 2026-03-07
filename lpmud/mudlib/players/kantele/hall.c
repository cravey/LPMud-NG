#include "std.h"


reset(arg) {
    if (!arg)
	set_light(1);
}

long(str) {
    if (str == "door") {
        if (call_other("players/kantele/inner_courtyard", "query_door"))
	    write("The door is closed.\n");
	else
	    write("The door is open.\n");
	return;
    }
    write("You are in the entrance hall to the mansion, a door leads \n"+
          "north and a passage leads south.\n");
}

short() {
    return "Entrance hall";
}

init() {
    add_action("north"); add_verb("north");
    add_action("open"); add_verb("open");
    add_action("south"); add_verb("south");
}

south() {
   call_other(this_player(), "move_player", "south#players/kantele/inner_hall");
   return 1;
    }

north() {
    if (call_other("players/kantele/inner_courtyard", "query_door")) {
	write("The door is closed.\n");
	return 1;
    }
    call_other(this_player(), "move_player", "north#players/kantele/inner_courtyard");
    return 1;
}

open(str) {
    if (str == "door") {
        if (!call_other("players/kantele/inner_courtyard", "query_door")) {
	    write("It is already open!\n");
	    return 1;
	}
        call_other("players/kantele/inner_courtyard","open_door_inside");
	write("Ok.\n");
	say(call_other(this_player(), "query_name") + " Opened the door.\n");
	return 1;
    }
    return 0;
}

id(str) {
    return str ==  "door";
}

