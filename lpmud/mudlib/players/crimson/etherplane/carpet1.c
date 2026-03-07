reset(arg) {
   if (!arg)
   set_light(1);
   extra_reset();
   return 1;
}

short() {
    return "Flying Carpet";
}

long() {
   write("You are on a HUGE flying carpet.\n");
   write("It is blue and grey with the\n");
   write("the design of a cloud dragon\n");
   write("in the center.\n");
}
init() {
  add_action("kill"); add_verb("kill");
  add_action("kill"); add_verb("fireball");
  add_action("kill"); add_verb("shock");
  add_action("kill"); add_verb("missile");
  add_action("kill"); add_verb("strike");
  add_action("kill"); add_verb("chill");
  add_action("jump"); add_verb("jump");
  add_action("jump"); add_verb("get");
}

kill(str) {
  if (str == "djinni") {
  write("Djinni falls down laughing.\n");
  write("Djinni says: You are very foolish but, Djinni forgives you.\n");
  return 1;
  }
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
    if(str == "north" && environment(this_object()) == "players/crimson/path") {
    move_object(this_object(), "players/crimson/path2");
    }
}

id(str) {
   return str == "Flying Carpet";
}

extra_reset() {
object djinni;

  if(!djinni || !living(djinni)) {
  djinni = clone_object("obj/monster.talk");
  call_other(djinni, "set_name", "djinni");
  call_other(djinni, "set_short", "Djinni");
  call_other(djinni, "set_level", 19);
  call_other(djinni, "set_al", 1000);
  call_other(djinni, "set_wc", 25);
  call_other(djinni, "set_ac", 15);
  call_other(djinni, "set_long", "She is dressed in flowing blue silks\n" +
             "and looks very lovely.\n");
  call_other(djinni, "set_chat_chance", 9);
  call_other(djinni, "set_a_chat_chance", 2);
  call_other(djinni, "load_chat", "Djinni says: If you want to stop here just jump off.\n");
  call_other(djinni, "load_chat", "Djinni says: Just look in the direction you want to...\n" +
  "Djinni says: see in order to get a bearing on where we are.\n");
  call_other(djinni, "load_chat", "Djinni says: I hope you enjoy your stay here.\n");
  call_other(djinni, "load_a_chat", "Djinni says: I am not pleased with this!\n");
  call_other(djinni, "set_aggressive", 0);
  move_object(djinni, this_object());
  }
}

query_value() { return 0; }

query_weight() { return 10000; }

get() { return 0; }
