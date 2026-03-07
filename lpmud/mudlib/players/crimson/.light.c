string amount_of_fuel;
string name;
string long_lit_desc;
string long_unlit_desc;
status is_lit;
int weight;
int magic;
int gived;

long() {
    if (is_lit) write(long_lit_desc);
    else write(long_unlit_desc);
}

reset(arg) {
    magic = 1;
    if (arg)
	return;
    amount_of_fuel = 0; name = 0; is_lit = 0; weight = 0;
}

set_weight(w) { weight = w; }

drop() {
    gived += 1;
    if (gived == 1)
    write("The light fades away from lack of energy.\n");
    say(call_other(this_player(), "query_name") + "'s light fades away.\n");
    destruct(this_object());
    return 1;
}

drop_object(str) {
   if (str == "all") {
      drop_object("crystal");
      return 1;
   }
   if (!str || !id(str))
      return 0;
   write("The light fades away from lack of energy.\n");
   say(call_other(this_player(), "query_name") + "'s light fades away.\n");
   destruct(this_object());
   return 1;
}

query_weight() { return weight; }

short() {
    if (is_lit)
	return name + " (lit)";
    if (amount_of_fuel == 0)
	return name + " (burnt out)";
    return name;
}

set_name(n) {
    name = n; 
    long_lit_desc = "A " + name + " (lit)\n";
    long_unlit_desc = "A " + name + "\n";
}
set_fuel(f) { amount_of_fuel = f; }

init() {
    add_action("rub"); add_verb("rub");
    add_action("extinguish"); add_verb("extinguish");
    add_action("no_find"); add_verb("origin");
    add_action("drop_object"); add_verb("drop");
    add_action("drop_object"); add_verb("put");
}

rub(str) {
    if (!str || str != name)
	return 0;
    if (amount_of_fuel == 0) {
	write("End of fuel.\n");
	return 1;
    }
    if (is_lit) {
	write("It is already lit.\n");
	return 1;
    }
    is_lit = 1;
    write("Ok.\n");
    set_light(1);
    set_heart_beat(1);
    return 1;
}

extinguish(str) {
    if (!str || str != name)
	return 0;
    if(!is_lit) {
	write("It is not lit!\n");
	return 1;
    }
    is_lit = 0;
    write("Ok.\n");
    set_light(-1);
    set_heart_beat(0);
    return 1;
}

heart_beat() {
    object ob;
    if (!is_lit)
	return;
    amount_of_fuel -= 1;
    if (amount_of_fuel > 0)
	return;
    say(name + " goes dark.\n");
    set_heart_beat(0);
    is_lit = 0;
    set_light(-1);
    ob = environment();
    if (call_other(ob, "query_level"))
	call_other(ob, "add_weight", -weight);
    destruct(this_object());
}

id(str) {
    return str == name;
}

query_value() {
    return amount_of_fuel/100 + 1;
}

get() { return 1; }

set_long_lit(str) { long_lit_desc = str; }
set_long_unlit(str) { long_unlit_desc = str; }

no_find(str) {
  if (str == id(str)) {
  write("What?\n");
  return 1;
  }
}

query_magic() { return magic; }
