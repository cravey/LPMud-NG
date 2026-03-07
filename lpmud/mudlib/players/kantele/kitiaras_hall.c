#include "std.h"


reset(arg) {
    if (!arg)
	set_light(1);
}

long(str) {
    if (str == "door") {
        if (call_other("players/kantele/kitiaras_anteroom", "query_door"))
	    write("The door is closed.\n");
	else
	    write("The door is open.\n");
	return;
    }
    write("You are in a small dark hall, a pasage leads south and a \n"+
          "door leads east, you can see some light to the south.\n");
}

short() {
    return "A small dark hall";
}

init() {
    add_action("east"); add_verb("east");
    add_action("open"); add_verb("open");
    add_action("south"); add_verb("south");
}

south() {
   call_other(this_player(), "move_player", "south#players/kantele/kitiaras_crossing");
    }

east() {
    if (call_other("players/kantele/kitiaras_anteroom", "query_door")) {
	write("The door is closed.\n");
	return 1;
    }
call_other(this_player(), "move_player", "east#players/kantele/Kitiaras_anteroom");
}

open(str) {
    if (str == "door") {
        if (!call_other("players/kantele/kitiaras_anteroom", "query_door")) {
	    write("It is already open!\n");
	    return 1;
	}
        call_other("players/kantele/kitiaras_anteroom","open_door_inside");
	write("Ok.\n");
	say(call_other(this_player(), "query_name") + " Opened the door.\n");
	return 1;
    }
    return 0;
}

id(str) {
    return str ==  "door";
}

