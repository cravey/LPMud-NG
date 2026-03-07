int j;
int i;
int time_to_exast;
string short_desc;
string name;

short() { 
      if (i == 0) {
      return short_desc;
      } else
      if (i == 1) {
      return 0;
      }
}

long() { 
     if (i == 0) {
     write("Hmmm, looks like a "+name+".\n");
     return 1;
     } else
     if (i == 1) {
     write("What?\n");
     return 1;
     }
}

set_short(str) {
   short_desc = str;
}

id(str) {
     return str == name || str == "potion";
}

set_id(str) {
     name = str;
}

init() {
    add_action("drink"); add_verb("drink");
    add_action("drink"); add_verb("quaff");
    add_action("drop_object"); add_verb("drop");
    add_action("drop_object"); add_verb("quit");
}

drink(str) {
object flask;
    if (i == 1) {
    return 0;
    } else
    if (str == name || str == "potion") {
    write("You quaff down the "+name+".\n");
    say(call_other(this_player(), "query_name") + " quaffs a potion.\n");
    call_other(environment(this_object()), "set_wc", 25);
    flask = clone_object("obj/treasure");
    call_other(flask, "set_id", "flask");
    call_other(flask, "set_short", "An empty flask");
    call_other(flask, "set_weight", 1);
    call_other(flask, "set_value", 10);
    move_object(flask, this_player());
    i = 1;
    set_heart_beat(1);
    call_other(environment(this_object()), "add_weight", 1);
    return 1;
    }
}

heart_beat() {
    j += 1;
    time_to_exast = 25;
    if (j>time_to_exast) {
    say("You become exhausted and collapse.\n");
    call_other(environment(this_object()), "set_wc", 3);
    call_other(environment(this_object()), "stop_wielding", 0);
    destruct(this_object());
    call_other(this_player(), "add_weight", 1);
    set_heart_beat(1);
    }
}

get() { return 1; }

query_weight() { return 0; }

query_potion() { return "strength"; }

drop_object(str) {
   if (i == 0) {
   return 0;
   } else
   if (str == "all") {
   call_other(environment(this_object()), "set_wc", 3);
   call_other(environment(this_object()), "stop_wielding", 0);
   destruct(this_object());
   return 0;
   } else
   if (!str) {
   call_other(environment(this_object()), "set_wc", 3);
   call_other(environment(this_object()), "stop_wielding", 0);
   destruct(this_object());
   return 0;
   }
}
