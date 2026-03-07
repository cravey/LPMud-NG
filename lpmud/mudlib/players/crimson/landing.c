int door_open;
int found;
reset(arg)
{
    if (!arg)
	set_light(1);
    door_open = 0;
    found = found -1;
}

open(str) {
   if (!id(str)) {
     return 1;
   } else {
     if (door_open == 1) {
     write ("The clock door is already open.\n");
     return 1;
   } else {
     door_open = 1;
     write("You open the clock door.\n");
     return 1;
   }
 }
}

init()
{
    add_action("down"); add_verb("down");
    add_action("search"); add_verb("search");
    add_action("enter"); add_verb("enter");
    add_action("look"); add_verb("look");
    add_action("get"); add_verb("get");
    add_action("take"); add_verb("take");
    add_action("east"); add_verb("east");
    add_action("west"); add_verb("west");
    add_action("open"); add_verb("open");
}

look(str)
{
    if (str == "at clock") {
    write("A large grandfather clock smiles down upon you. Oddly, there seems to be\n");
    write("a thirteenth hour on it. You can see your reflection in the glass door.\n");
    return 1;
    } else
    if (str == "at grandfather clock") {
    write("A large grandfather clock smiles down upon you. Oddly, there seems to be\n");
    write("a thirteenth hour on it. You can see your reflection in the glass door.\n");
    return 1;
    } else
    if (str == "at tapestry") {
    write("A brilliant purple tapestry with a golden letter L in the center.\n");
    return 1;
    }
}

down() {
    call_other(this_player(), "move_player", "down the stairs#players/crimson/greeting");
    return 1;
}

east() {
    write("You climb up the stairs and arrive in the east wing.\n");
    call_other(this_player(), "move_player", "up the east staircase#players/crimson/winge");
    return 1;
}

west() {
    write("You climb up the stairs and arrive in the west wing.\n");
    call_other(this_player(), "move_player", "up the west staircase#players/crimson/wingw");
    return 1;
}

get(str) {
string tapestry;
   if (str == "clock" || str == "grandfather clock") {
   write("The grandfather clock is too heavy.\n");
   return 1;
   } else
   if (found == 1) {
   write("Hmmm, its just out of reach.\n");
   return 1;
   } else
   if (str == "tapestry" && found <1) {
   write("The tapestry comes tumbling down on your head.\n");
   found = 1;
   if(!tapestry || !environment(tapestry)) {
   tapestry = clone_object("obj/treasure");
   call_other(tapestry, "set_id", "tapestry");
   call_other(tapestry, "set_short", "A tapestry");
   call_other(tapestry, "set_long", "A brilliant purple tapestry with a golden letter L in the center.\n");
   call_other(tapestry, "set_alt_name", "rug");
   call_other(tapestry, "set_weight", 2);
   call_other(tapestry, "set_value", 300);
   move_object(tapestry, this_player());
      return 1;
   }
   } else
   if (str) {
   return 0;
   } else
   if (!str) {
   return 1;
   }
}

take(str) {
string tapestry;
   if (str == "clock" || str == "grandfather clock") {
   write("The grandfather clock is too heavy.\n");
   return 1;
   } else 
   if (found == 1) {
   write("Hmmm, its just out of reach.\n");
   return 1;
   } else
   if (str == "tapestry" && found <1) {
   write("The tapestry comes tumbling down on your head.\n");
   found = 1;
   if (!tapestry || environment(tapestry)) {
   tapestry = clone_object("obj/treasure");
   call_other(tapestry, "set_id", "tapestry");
   call_other(tapestry, "set_short", "A tapestry");
   call_other(tapestry, "set_long", "A brilliant purple tapestry with a golden letter L in the center.\n");
   call_other(tapestry, "set_alt_name", "rug");
   call_other(tapestry, "set_weight", 2);
   call_other(tapestry, "set_value", 300);
   move_object(tapestry, this_player());
   return 1;
   }
   }
}

search(str) {
   if (!id(str)) {
   write("What do you want to search?\n");
   return 1;
   } else
   if (str == "clock" && door_open == 1) {
   write("Hmmm, there seems to be an opening in the back of the clock.\n");
   return 1;
   } else
   if (str == "grandfather clock" && door_open == 1) {
   write("Hmmm, there seems to be an opening in the back of the clock.\n");
   return 1;
   } else
   if (str == "clock" && door_open == 0) {
   write("You stare into the glass door of the clock.\n");
   return 1;
   } else
   if (str == "grandfather clock" && door_open == 0) {
   write("You stare into the glass door of the clock.\n");
   return 1;
   }
}

long()
{
    write("You are on the landing now.  To the east and west are yet more stairs\n");
    write("leading up into their respective wings.  On the southern wall you\n");
    write("see numerous identical tapestries and a rather large grandfather clock.\n");
    write("Staring down the stairs you see the fountain and an open door leading out.\n");
}

short() {
    return "Landing";
}

enter(str) {
    if (!id(str)) {
    write("What do you want to enter?\n");
      return 1;
    } else
    if (door_open == 0) {
        write("The clock door is closed.\n");
    return 1;
    }
    call_other(this_player(), "move_player", "into the clock#players/crimson/stairs");
    return 1;
}

id(str) {
    return str == "clock" || str == "grandfather clock" || str == "clock door" || str == "door";
}

