# 1 "players/ringer/castle.c"
# 1 "<built-in>" 1
# 1 "<built-in>" 3
# 465 "<built-in>" 3
# 1 "<command line>" 1
# 1 "<built-in>" 2
# 1 "players/ringer/castle.c" 2


# 12 "players/ringer/castle.c"
   

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
    move_object(this_object(), "room/deep_forest1");
    if (!call_other("room/quest_room","present_quest","ringer")) {
	ob = clone_object("obj/quest_obj");
	call_other(ob, "set_name", "ringer");
	call_other(ob, "set_hint",
 "Talk to the quest master, he'll fill you in, or\nFree the prisoner of the sand goblins.\n");
    call_other(ob, "set_level", 20);
	move_object(ob, "room/quest_room");
    }







       
}




