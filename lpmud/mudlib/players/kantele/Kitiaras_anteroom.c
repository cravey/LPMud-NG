#include "std.h"
int door_is_open, door_is_locked;

#undef EXTRA_RESET
#define EXTRA_RESET\
    door_is_open = 0; door_is_locked = 1;
	
#undef EXTRA_LONG
#define EXTRA_LONG\
    if (str == "sign") {\
        write("there is a sign on the door, the sign says\n" +\
            "MORTALS SHOULD NOT ENTER !(on pain of death.)\n");\
	return;\
    }\
    if (str == "door") {\
	if (door_is_open) {\
	    write("The door is open.\n");\
	    return;\
	}\
	write("The door is closed.\n");\
	return;\
    }

#undef EXTRA_INIT
#define EXTRA_INIT\
    add_action("open"); add_verb("open");\
    add_action("unlock"); add_verb("unlock");\
    add_action("west"); add_verb("west");

ONE_EXIT("players/kantele/cellar_west.c", "east",
         "You are in Kitiaras anteroom.",
         "You are in Kitiaras anteroom, a sturdy steal door leads west\n" +
         "and a passage east. There is a sign on the door.\n",0)

id(str) {
    return str == "door" || str == "sign";
}

open(str) {
    if (str && str != "door")
	return 0;
    if (door_is_open)
	return 0;
    if (door_is_locked) {
	write("The door is locked.\n");
	return 1;
    }
    door_is_open = 1;
    write("ok.\n");
    say(call_other(this_player(), "query_name") + " opened the door.\n");
    return 1;
}

unlock(str) {
    if (str && str != "door")
	return 0;
    if (door_is_open || !door_is_locked)
	return 0;
    if (!present("cellar key", this_player())) {
    if (present("bank key", this_player()) || present("mansion key", this_player())) {
        write("the key doesn't fit.\n");
return 1;
}
	write("You need a key.\n");
	return 1;
    }
    door_is_locked = 0;
    write("ok.\n");
    say(call_other(this_player(), "query_name") + " unlocked the door.\n");
    return 1;
}

west() {
    if (!door_is_open) {
	write("The door is closed.\n");
	return 1;
    }
    call_other(this_player(), "move_player", "west#players/kantele/kitiaras_hall");
    return 1;
}

query_door() {
    return !door_is_open;
}

open_door_inside() {
    door_is_locked = 0;
    door_is_open = 1;
}
