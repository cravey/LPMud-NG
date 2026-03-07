
#define DEST "room/deep_forest1"
/*
 * This is just the facade to a castle. If you want to enable the
 * "enter" command, move the player to a hall or something (which
 * you have to design yourself).
 * The predefined string DEST is where a player should come when he
 * leaves the castle.
 *
 * This file is loaded automatically from "init_file". We have to move
 * ourself to where we are supposed to be.
 */

id(str) { return str == "shop" || str == "sign"; }

short() {
    return "A shop is nearby to the north.  There is a sign on the door";
}

long(str) {
   if (!str || str == "shop") {
      write("The shop is small and unimpressive.\n");
      read("sign");
   } else if (str == "sign") read("sign");
}

read(str) {
   if (str != "sign")
      return 0;
   write("  A beat up sign on the door reads:\n" 
             + "Grog's Magic Shop.  Grog no like salesmen!\n"
+ "A small postscript is engraved: Welcome to Ringer's world\n" );
   return 1;
}

init() {
    add_action("enter"); add_verb("enter");
    add_action("north"); add_verb("north");
    add_action("read"); add_verb("read");
}

north() {
  return enter("shop");
}

enter(str) {
    if (str != "shop")
	return 0;
    call_other(this_player(),"move_player","north#players/ringer/mg_shop");
    return 1;
}

reset(arg) {
    object ob;
    if (arg)
	return;
    move_object(this_object(), DEST);
    if (!call_other("room/quest_room","present_quest","ringer")) {
	ob = clone_object("obj/quest_obj");
	call_other(ob, "set_name", "ringer");
	call_other(ob, "set_hint",
 "Talk to the quest master, he'll fill you in, or\nFree the prisoner of the sand goblins.\n");
    call_other(ob, "set_level", 20);
	move_object(ob, "room/quest_room");
    }
/*    if (!call_other("room/quest_room","present_quest","ringer2")) {
	ob = clone_object("obj/quest_obj");
	call_other(ob, "set_name", "ringer2");
	call_other(ob, "set_hint",
 "Free the prisoner of the sand goblins.\n");
	call_other(ob, "set_level", 16);
	move_object(ob, "room/quest_room");
    }*/
}



