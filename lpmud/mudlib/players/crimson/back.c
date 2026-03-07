int door_open;
reset(arg) {
    if (!arg)
	set_light(1);
     door_open = 0;
}

open(str) {
    if (!id(str)) {
       return 1;
    } else {
      if (door_open == 1) {
      write("The door is already open.\n");
      return 1;
    } else {
      door_open = 1;
      write("The door opens almost as though it were of it's own will.\n");
      return 1;
    }
  }
}
init() {
    add_action("south"); add_verb("south");
    add_action("north"); add_verb("north");
    add_action("east"); add_verb("east");
    add_action("look"); add_verb("look");
    add_action("open"); add_verb("open");
}

south() {
    if (door_open == 0) {
      write("The door is closed.\n");
    } else {
    call_other(this_player(), "move_player", "south#room/plane4");
    }
    return 1;
}

look(str) {
    if (str == "at door" && door_open == 0) {
    write("An ordinary looking door. It is closed.\n");
    return 1;
    } else
    if (str == "at door") {
    write("The door is open.\n");
    return 1;
    } else
    if (str == "at cabinet") {
    write("There is a HUGE padlock on the cabinet doors.\n");
    return 1;
    } else
    if (str == "at sink") {
    write("It looks like an ordinary sink.\n");
    return 1;
    }
}

east() {
    
    write("You can't go that way.\n");
    return 1;
}

north()
{
    call_other(this_player(), "move_player", "north#players/crimson/archw1");
    return 1;
}

id(str) {
    return str == "door" || str == "backdoor";
}

long()
{
    write("Various surgical instruments are laid out on carts all around\n");
    write("the room. On the north wall you see several cabinets and a sink.\n");
    write("Through a southern window you can see the outside.  An archway\n");
    write("to the north leads back into the funeral service area.\n");
    write("The backdoor is next to the window on the south wall.\n");
}

short() {
    return "Back Way";
}
