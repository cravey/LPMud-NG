int gived;

short() { return "A rope trick spell (memorized)"; }

long() {
   write("A rope trick spell (memorized).\n");
   return 1;
}

id(str) {
   return str == "rope spell" || str == "spell";
}

init() {
   add_action("cast"); add_verb("cast");
}

no_find(str) {
   if (str == "spell" || str == "rope spell") {
   write("What?\n");
   return 1;
   }
}

cast(str) {
object rope;
string player;
   player = call_other(this_player(), "query_name", 0);
   if (str == "rope trick") {
   write("A magical rope appears before you.\n");
   say(player+ " casts a spell.\n");
   rope = clone_object("players/crimson/.rope");
   move_object(rope, environment(this_player()));
   destruct(this_object());
   return 1;
   }
}

get() { return 1; }

query_weight() { return 0; }
