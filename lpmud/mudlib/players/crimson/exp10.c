int j;
int i;
int injecta;
int injectb;
int injectc;
short() { 
      if (i == 0) {
      return "A grey potion";
      } else
      if (i == 1) {
      return 0;
      }
}

long() { 
     if (i == 0) {
     write("Hmmm, there seems to be a label on it.\n");
     return 1;
     } else
     if (i == 1) {
     write("What?\n");
     return 1;
     }
}

id(str) {
     return str == "potion" || str == "poison";
}

init() {
    add_action("drink"); add_verb("drink");
    add_action("read"); add_verb("read");
    add_action("drink"); add_verb("quaff");
}

read(str) {
    if (!str) {
    return 0;
    } else
    if (str == "label" || str == "label on potion" || str == "label on red potion") {
    write("The label reads: Potion of Giant Strength.\n");
    return 1;
    } else
    if (str) {
    return 0;
    }
}

drink(str) {
object flask;
   if (i == 1) {
   return 0;
   } else
    if (str == "potion" || str == "potion of giant strength") {
    write("You quaff down the grey potion.\n");
    say(call_other(this_player(), "query_name") + " quaffs a potion.\n");
    flask = clone_object("obj/treasure");
    call_other(flask, "set_id", "flask");
    call_other(flask, "set_short", "An empty flask");
    call_other(flask, "set_weight", 1);
    call_other(flask, "set_value", 10);
    move_object(flask, this_player());
    i = 1;
    call_other(this_player(), "add_weight", 1);
    set_heart_beat(1);
    return 1;
    }
}

heart_beat() {
    j += 1;
    injecta = 1;
    injectb = 30;
    injectc = 60;
    if (j==injecta) {
    say("You feel a sharp pain in your chest.\n");
    call_other(environment(this_object()), "hit_player", 10);
    } else
    if (j==injectb) {
    say("You feel a sharp pain in your chest.\n");
    call_other(environment(this_object()), "hit_player", 10);
    } else
    if (j ==injectc) {
    call_other(environment(this_object()), "hit_player", 20);
    say("The pains are becoming more agonizing.\n"); 
    j = 1;
    }
}

get() { return 1; }

query_weight() { return 0; }
