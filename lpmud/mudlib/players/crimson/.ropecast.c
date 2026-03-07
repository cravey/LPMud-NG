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
   add_action("no_find"); add_verb("origin");
   add_action("drop_object"); add_verb("drop");
   add_action("drop_object"); add_verb("put");
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

drop() {
   gived += 1;
   if (gived == 1)
   write("You forgot a rope trick spell.\n");
   say(call_other(this_player(), "query_name") + " forgot a rope trick spell.\n");
   destruct(this_object());
     return 1;
}

drop_object(str) {
   if (str == "all") {
     drop_object("rope spell");
     return 1;
   }
   if (!str || !id(str)) {
     return 0;
   write("You forgot a rope trick spell.\n");
   say(call_other(this_player(), "query_name") + " forgot a rope trick spell.\n");
   destruct(this_object());
   return 1;
   }
}
