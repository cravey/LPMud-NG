# 1 "players/crimson/.lightcast.c" 
int gived;

short() { return "A light spell (memorized)"; }

long() {
   write("A light spell (memorized).\n");
   return 1;
}

id(str) {
   return str == "light spell" || str == "spell";
}

init() {
   add_action("cast"); add_verb("cast");
   add_action("no_find"); add_verb("origin");
   add_action("drop_object"); add_verb("drop");
   add_action("drop_object"); add_verb("put");
}

no_find(str) {
   if (str == "spell" || str == "light spell") {
   write("What?\n");
   return 1;
   }
}

cast(str) {
object light;
string player;
   player = call_other(this_player(), "query_name", 0);
   if (str == "light") {
   write("A small crystal appears in your hand.\n");
   say(player+ " casts a spell.\n");
   light = clone_object("players/crimson/.light");
   call_other(light, "set_name", "crystal");
   call_other(light, "set_short", "A crystal");
   call_other(light, "set_fuel", 900);
   call_other(light, "set_weight", 0);
   move_object(light, this_player());
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
   write("You forgot a light spell.\n");
   say(call_other(this_player(), "query_name") + " forgot a rope trick spell.\n");
   destruct(this_object());
   return 1;
   }
}
