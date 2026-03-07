int door_open;
int found;
int dead;
int keythere;
object bone;
object boned;
object bottle;
object flask;
object flaska;
object wire;

short() { return "A cage"; }

reset(arg) {
    set_light(1);
    door_open = 0;
    found = 0;
    dead = 0;
    keythere = 0;
}

long() {
     write("You are inside a cage with iron bars.\n");
     write("Peering through the bars you notice\n");
     write("that your cage is inside a cave.  An\n");
     write("orc stands in front of the cage and\n");
     write("guards the door. You notice that mounds\n");
     write("of rubble cover the floor.\n");
     if (door_open == 0) {
     write("The cage door to the east is closed.\n");
     return 1;
     } else
     if (door_open == 1) {
     write("The cage door to the east is open.\n");
     return 1;
     }
}

east(str) {
     if (door_open == 0) {
     write("The cage door is closed.\n");
     return 1;
     } else
     if (door_open == 1) {
     call_other(this_player(), "move_player", "east#players/crimson/cave1");
     return 1;
     }
}

init() {
    add_action("east"); add_verb("east");
    add_action("search"); add_verb("search");
    add_action("search"); add_verb("dig");
    add_action("look"); add_verb("look");
    add_action("use"); add_verb("use");
}

search(str) {
   if (str == "rubble" && (found == 1) || str == "through rubble" && (found == 1)) {
   write("You dig through the rubble and find nothing.\n");
   say(call_other(this_player(), "query_name") +" digs through the rubble.\n");
   return 1;
   } else
   if (str == "rubble" && (found == 0) || str == "through rubble" && (found == 0)) {
   write("You pull up some old bones, a wire, and some bottles.\n");
   say(call_other(this_player(), "query_name")+" pulls up some bones, a wire, and a some bottles from the rubble.\n");
   found = 1;
   bone = clone_object("obj/weapon");
   call_other(bone, "set_name", "bone");
   call_other(bone, "set_short", "A gnawed on old bone");
   call_other(bone, "set_long", "It looks like it might have been the femur of some unfortunate soul.\n");
   call_other(bone, "set_weight", 3);
   call_other(bone, "set_alias", "gnawed on old bone");
   call_other(bone, "set_class", 10);
   call_other(bone, "set_value", 175);
   move_object(bone, this_object());
   boned = clone_object("obj/weapon");
   call_other(boned, "set_name", "bone");
   call_other(boned, "set_short", "A chewed on old bone");
   call_other(boned, "set_long", "It looks like the jawbone of a rather large animal.\n");
   call_other(boned, "set_weight", 4);
   call_other(boned, "set_alias", "chewed on old bone");
   call_other(boned, "set_class", 11);
   call_other(boned, "set_value", 200);
   move_object(boned, this_object());
   bottle = clone_object("obj/weapon");
   call_other(bottle, "set_name", "bottle");
   call_other(bottle, "set_short", "A broken bottle");
   call_other(bottle, "set_alias", "broken bottle");
   call_other(bottle, "set_long", "Hmmm, it was a good year too.\n");
   call_other(bottle, "set_weight", 1);
   call_other(bottle, "set_class", 10);
   call_other(bottle, "set_value", 175);
   move_object(bottle, this_object());
   flask = clone_object("players/crimson/exp");
   move_object(flask, this_object());
   flaska = clone_object("players/crimson/exp5");
   move_object(flaska, this_object());
   return 1;
   } else
   if (!str) {
   write("What do you want to search?\n");
   return 1;
   } else
   if (str) {
   write("What do you want to search?\n");
   return 1;
   }
}

look(str) {
  if (str == "at guard" && (dead == 1) || str == "at orc guard" && (dead == 1) || str == "at orc" && (dead == 1)) {
  return 0;
  } else
  if (str == "at guard" || str == "at orc guard" || str == "at orc") {
  write("He really is an ugly brute.\n");
  write("Hmmm, he appears to be dozing off.\n");
  if (keythere != 1) {
  write("Hey! There is a key on his belt!\n");
  }
  say(call_other(this_player(), "query_name")+" stares at the orc guard.\n");
  return 1;
  } else
  if (str == "at bars" || str == "at iron bars") {
  write("They seem to somewhat rusted yet, very sturdy.\n");
  say(call_other(this_player(), "query_name")+" stares at the bars.\n");
  return 1;
  } else
  if (str == "at rubble" || str == "at mounds on floor" || str == "at mounds" || str == "at floor") {
  write("It really is a mess in here.\n");
  write("Bits and pieces of things stick up through the rubble.\n");
  say(call_other(this_player(), "query_name")+" looks at the rubble on the floor.\n");
  return 1;
  } else
  if (str == "at cage") {
  long();
  return 1;
  } else
  if (str == "at cage door" && door_open == 0 || str == "at door" && door_open == 0) {
  write("There is some sort of lock on it.\n");
  write("It is closed.\n");
  return 1;
  }
  if (!str) {
  return 0;
  } else
  if (str) {
  return 0;
  }
}

use(str) {
  if (str == "wire to get key" && !present("wire", environment(this_player()))){
  write("The key is just barely out of reach.\n");
  return 1;
  } else
  if (str == "wire to take key" && !present("wire", environment(this_player()))){
  write("The key is just barely out of reach.\n");
  return 1;
  } else
  if (str == "wire to get key from guard" && !present("wire", environment(this_player()))){
  write("The key is just barely out of reach.\n");
  return 1;
  } else
  if (str == "wire to take key from guard" && !present("wire", environment(this_player()))){
  write("The key is just barely out of reach.\n");
  return 1;
  } else
  if (str == "wire to get key from belt" && !present("wire", environment(this_player()))) {
  write("The key is just barely out of reach.\n");
  return 1;
  } else
  if (str == "wire to take key from belt" && !present("wire", environment(this_player()))){
  write("The key is just barely out of reach.\n");
  return 1;
  } else
  if (str == "wire to get key from orc guard's belt" && !present("wire", environment(this_player()))) {
  write("The key is just barely out of reach.\n");
  return 1;
  } else
  if (str == "wire to take key from orc guard's belt" && !present("wire", environment(this_player()))){
  write("The key is just barely out of reach.\n");
  return 1;
  } else
  if (str == "wire to get key from orc" && !present("wire", environment(this_player()))) {
  write("The key is just barely out of reach.\n");
  return 1;
  } else
  if (str == "wire to take key from orc" && !present("wire", environment(this_player()))){
  write("The key is just barely out of reach.\n");
  return 1;
  } else
  if (str == "wire to get key from orc guard" && !present("wire", environment(this_player()))){
  write("The key is just barely out of reach.\n");
  return 1; 
  } else
  if (str == "key to take key from orc guard" && !present("wire", environment(this_player()))){
  write("The key is just barely out of reach.\n");
  return 1;
  }
}
