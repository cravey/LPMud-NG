reset(arg) {
   if (!arg)
   set_light(1);
   return 1;
}

short() {
    return "Flying Carpet";
}

long() {
   if (environment(this_player()) == environment(this_object())) {
   write("A blue and white carpet floats before you.\n");
   return 1;
   } else
   if (environment(this_player()) == this_object()) {
   write("You are on a HUGE flying carpet.\n");
   write("It is blue and grey with the\n");
   write("the design of a cloud dragon\n");
   write("in the center.\n");
   return 1;
   }
}
init() {
  add_action("jump"); add_verb("jump");
  add_action("jump"); add_verb("get");
  add_action("ride"); add_verb("ride");
}

jump(str) {
string name;
  name = call_other(this_player(), "query_name", 0);
  if (str == "off" || str == "off carpet") {
  say(name+ " jumps off the flying carpet.\n");
  write("You float gently to the ground.\n");
  move_object(this_player(), "players/crimson/storeroom");
  say(name+ " floats gently to the ground.\n");
  return 1;
  }
}

ride(str) {
    if(!str || str == "carpet" || str == "flying carpet" && (environment(this_player()) == environment(this_object()))) {
    move_object(this_player(), this_object());
    call_other(this_player(), "look", 0);
    return 1;
    }
}

id(str) {
   return str == "carpet" || str == "flying carpet";
}

query_value() { return 0; }

query_weight() { return 10000; }

get() { return 0; }
