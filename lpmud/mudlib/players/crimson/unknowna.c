int i;
string short_desc;
string name;

short() {
    if (i == 0) {
    return short_desc;
    } else
    return 0;
}

long() {
    if (i == 0) {
    write("Hmmm, looks like a "+name+".\n");
    return 1;
    } 
    write("What?\n");
    return 1;
}

id(str) { 
    return str == name || str == "potion";
}

set_short(str) {
    short_desc = str;
}

set_id(str) {
    name = str;
}

init() {
    add_action("drink"); add_verb("drink");
    add_action("drink"); add_verb("quaff");
}

drink(str) {
object flask;
    if (i == 1) {
    return 0;
    } else
   if (str == name || str == "potion") {
   write("You quaff down the "+name+".\n");
   say(call_other(this_player(), "query_name") + " quaffs a potion.\n");
   call_other(this_player(), "heal_self", 10000);
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

query_weight() { return 1; }

get() { return 1; }

query_potion() { return "serious"; }
