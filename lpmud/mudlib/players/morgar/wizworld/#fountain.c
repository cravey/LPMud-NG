int been_used;

id(str) { return str == "fountain"; }

init() { add_action("drink"); add_verb("drink"); }

reset(arg) {
    been_used = 0;
    return;
}

long() {
    write("A magnificent fountain is here.  Crystal clear water falls\n");
    write("in complex patters over an abstract sculpture.\n");
}

short() { return "A magnificent fountain is here"; }

get() {
    return 0;
}

query_name() {
   return "fountain";
}

drink() {
    write("been_used = " + been_used + "\n");
    if (!been_used) {
	write("You drink from the fountain, and are healed some.\n");
	call_other(this_player(), "heal_self", 5);
	been_used = 1;
    }
    else
	write("The level of the water is too low, although it is filling up.\n");
    return 1;
}
