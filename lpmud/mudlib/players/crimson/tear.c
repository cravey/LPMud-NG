id(str) { return str == "tear"; }

init() { add_action("eat"); add_verb("eat"); }

long() { write("A small golden tear.  It feels tingly to the touch \n");
         write("and it smells like jinseng root.\n");
}

short() { return "A golden tear"; }

query_value() { return 1500; }

get() {
    return 1;
}

query_weight() {
    return 1;
}

query_name() {
   return "tear";
}

eat() {
    write("You eat the golden tear, and feel rejuvenated.\n");
    call_other(this_player(), "heal_self", 45);
    destruct(this_object());
    return 1;
}

get() {
    write("The acid burns you a little, but you take the globule.\n");
    call_other(this_player(), "heal_self", -15);
    return 1;
}
