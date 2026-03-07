#include "std.h"
int door_is_open, door_is_locked;
object guard;

#undef EXTRA_RESET
#define EXTRA_RESET\
    if (!guard || !living(guard)) {\
	object key;\
	key = clone_object("obj/treasure");\
	call_other(key, "set_id", "key");\
	call_other(key, "set_alias", "bank key");\
	call_other(key, "set_short", "A key");\
	call_other(key, "set_value", 10);\
	call_other(key, "set_weight", 1);\
    }\
    door_is_open = 0; door_is_locked = 1;
	
#undef EXTRA_LONG
#define EXTRA_LONG\
    if (str == "sign") {\
        write("there is a sign on the door, the sign says\n" +\
            "Kantele lives here, beaware.\n");\
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
    add_action("south"); add_verb("south");

ONE_EXIT("players/kantele/courtyard", "north",
         "The inner courtyard",
         "You are in the inner courtyard, it opens up to the north \n" +
         "and a door leads south. there is a sign on the door.\n ",1)

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
    if (!present("mansion key", this_player())) {
    if (present("bank key", this_player())) {
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

south() {
    if (!door_is_open) {
	write("The door is closed.\n");
	return 1;
    }
    call_other(this_player(), "move_player", "south#players/kantele/hall");
    return 1;
}

query_door() {
    return !door_is_open;
}

open_door_inside() {
    door_is_locked = 0;
    door_is_open = 1;
}
