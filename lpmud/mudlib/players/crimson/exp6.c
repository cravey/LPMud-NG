int i;
short() {
    if (i == 0) {
    return "A black potion";
    } else
    return 0;
}

long() {
    if (i == 0) {
    write("Hmmm, looks like there is a label on it.\n");
    return 1;
    } 
    write("What?\n");
    return 1;
}

id(str) { 
    return str == "potion of vitality" || str == "potion of coffee" || str == "potion";
}

init() {
    add_action("drink"); add_verb("drink");
    add_action("drink"); add_verb("quaff");
    add_action("read"); add_verb("read");
}

drink(str) {
object flask;
   if (i == 1) {
   return 0;
   } else
   if (str == "potion" || str == "potion of vitality" || str == "potion of coffee") {
   write("You quaff down the black potion.\n");
   say(call_other(this_player(), "query_name") + " quaffs a potion.\n");
   call_other(this_player(), "drink_alcohol", -(10));
   flask = clone_object("obj/treasure");
   call_other(flask, "set_id", "flask");
   call_other(flask, "set_short", "An empty flask");
   call_other(flask, "set_weight", 1);
   call_other(flask, "set_value", 10);
   move_object(flask, this_player());
   i = 1;
   destruct(this_object());
   call_other(this_player(), "add_weight", 1);
   return 1;
   } else
   if (!str) {
   return 0;
   } else
   if (str) {
   return 0;
   }
}

read(str) {
  if (str == "label" && i == 0 || str == "label on black potion" && i == 0) {
  write("The label reads: Potion of Vitality.\n");
  return 1;
  } else
  if (!str) {
  return 0;
  } else
  if (str) {
  return 0;
  }
}

query_weight() { return 1; }

get() { return 1; }

query_value() { return 100; }
