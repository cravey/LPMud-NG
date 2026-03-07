int i;
short() {
    if (i == 0) {
    return "A purple potion";
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
    return str == "potion of polymorph" || str == "potion of random polymorph" || str == "potion";
}

init() {
    add_action("drink"); add_verb("drink");
    add_action("drink"); add_verb("quaff");
    add_action("read"); add_verb("read");
}

drink(str) {
object flask;
int n;
    if (i == 1) {
    return 0;
    } else
   if (str == "potion" || str == "potion of hallucinations" || str == "potion of hallucination") {
   write("You quaff down the purple potion.\n");
   say(call_other(this_player(), "query_name") + " quaffs a potion.\n");
   n = random(21);
   if (n == 0) {
   call_other(this_player(), "set_title", "The Dragon");
   } else
   if (n == 1) {
   call_other(this_player(), "set_title", "The Mule");
   } else
   if (n == 2) {
   call_other(this_player(), "set_title", "The Parot");
   } else
   if (n == 3) {
   call_other(this_player(), "set_title", "The Fly");
   } else
   if (n == 4) {
   call_other(this_player(), "set_title", "The Toad");
   } else
   if (n == 5) {
   call_other(this_player(),  "set_title", "The Rabbit");
   } else
   if (n == 6) {
   call_other(this_player(),  "set_title", "The Bat");
   } else
   if (n == 7) {
   call_other(this_player(),  "set_title", "The Dwarf");
   } else
   if (n == 8) {
   call_other(this_player(),  "set_title", "The Elf");
   } else
   if (n == 9) {
   call_other(this_player(),  "set_title", "The Pig");
   } else
   if (n == 10) {
   call_other(this_player(),  "set_title", "The Weasel");
   } else
   if (n == 11) {
   call_other(this_player(),  "set_title",  "The Tiger");
   } else
   if (n == 12) {
   call_other(this_player(),  "set_title",  "The Mouse");
   } else
   if (n == 13) {
   call_other(this_player(),  "set_title",  "The Bear");
   } else
   if (n == 14) {
   call_other(this_player(),  "set_title",  "The Icky Green Thing");
   } else
   if (n == 15) {
   call_other(this_player(),  "set_title",  "The Troll");
   } else
   if (n == 16) {
   call_other(this_player(),  "set_title",  "The Knome");
   } else
   if (n == 17) {
   call_other(this_player(),  "set_title",  "The Halfing");
   } else
   if (n == 18) {
   call_other(this_player(),  "set_title",  "The Snake");
   } else
   if (n == 19) {
   call_other(this_player(),  "set_title",  "The Orc");
   } else
   if (n == 20) {
   call_other(this_player(),  "set_title",  "The Kirin");
   }
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
  if (str == "label" && i == 0 || str == "label on purple potion" && i == 0) {
  write("The label reads: Potion of Random Polymorph.\n");
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

query_value() { return 50; }
