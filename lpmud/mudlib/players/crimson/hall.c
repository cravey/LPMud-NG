int there;
string who;
object player;

short() {
   return "Example hall"; 
}

reset(arg) {
   set_light(1);
   if (call_other("players/crimson/hall", "query_there") != 1) {
   call_other("players/crimson/hall", "set_who", 0);
   return 1;
   }
}

long() {
   write("This is a sample hotel hall.\n");
   write("The only obvious exit is east.\n");
   return 1;
}

init() {
   add_action("east"); add_verb("east");
}

east(str) {
   player = this_player();
   if (call_other(player, "query_real_name") != who && call_other("players/crimson/hall", "query_there") == 1) {
   tell_object(player, "The door is shut and locked.\n");
   return 1;
   } else
   if (call_other("players/crimson/hall", "query_who") == 0) {
   call_other(this_player(), "move_player", "east#players/crimson/example");
   who = call_other(this_player(), "query_real_name");
   call_other("players/crimson/hall", "set_there", 1);
   return 1;
   } else
   if (call_other(this_player(), "query_real_name") == who) {
   call_other(this_player(), "move_player", "east#players/crimson/example");
   return 1;
   }
}

set_there(str) {
   there = str;
}

query_there() { return there; }

query_who() { return who; }

set_who(str) {
     who = str;
}
