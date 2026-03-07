int i;
int charges;
int gived;
short() {
    if (i == 0) {
    return "A yellow and red potion";
    } else
    return 0;
}

reset(arg) {
  return charges = 4;
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
    return str == "potion of fire breath" || str == "potion of fire" || str == "potion";
}

init() {
    add_action("drink"); add_verb("drink");
    add_action("drink"); add_verb("quaff");
    add_action("breath"); add_verb("breath");
    add_action("read"); add_verb("read");
    add_action("drop_object"); add_verb("drop");
    add_action("drop_object"); add_verb("quit");
}

drink(str) {
object flask;
   if (i == 1) {
   return 0;
   } else
   if (str == "potion" || str == "potion of fire" || str == "potion of fire breath") {
   write("You quaff down the yellow and red potion.\n");
   say(call_other(this_player(), "query_name") + " quaffs a potion.\n");
   flask = clone_object("obj/treasure");
   call_other(flask, "set_id", "flask");
   call_other(flask, "set_short", "An empty flask");
   call_other(flask, "set_weight", 1);
   call_other(flask, "set_value", 10);
   move_object(flask, this_player());
   i = 1;
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
  if (str == "label" && i == 0 || str == "label on yellow and red potion" && i == 0) {
  write("The label reads: Potion of Fire Breath.\n");
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

breath(str) {
  object breathed;
  object breather;
  string name;
  string nbreath;

  if (i == 0) {
  write("What?\n");
  return 1;
  }
  if (!str) { 
  write("Breath on whom?\n");
  return 1;
  }
  breathed = present(lower_case(str), environment(this_player()));
  nbreath = capitalize(str);
  if (!breathed) {
  write("No "+nbreath+" here.\n");
  return 1;
  }
  if (!living(breathed)) {
  write(nbreath+" is not alive!\n");
  return 1;
  }
  if (breathed == this_player()) {
  write("You choke on the fumes.\n");
  return 1;
  }
  name = call_other(this_player(), "query_name");
  breather = this_player();
  tell_object(breathed, name+" exhaled a blast of fire on you!\n");
  write("You breath fire all over "+nbreath+".\n");
  say(name+" breathed a blast of fire on "+nbreath+"!\n");
  call_other(breathed, "hit_player", random(30));
  call_other(breather, "attack_object", breathed);
  charges = charges -1;
  if (charges == 1) {
    destruct(this_object());
    tell_object(this_player(), "You watch the remainants of the smoke leave you.\n");
    return 1;
  }
}

drop_object(str) {
 if (i == 0) {
 return 0;
 } else
 if (str == "all") {
 destruct(this_object());
 return 0;
 } else
 if (!str) {
 destruct(this_object());
 return 0;
 }
}
