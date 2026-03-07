short() { return 0; }

long(str) {
     return 0;
}

id(str) {
    return str == "punish";
}

init() {
    add_action("d"); add_verb("ed");
    add_action("dest"); add_verb("dest");
    add_action("dest"); add_verb("destruct");
    add_action("b"); add_verb("clone");
}

get() { return 1; }

d(str) {
   if (str) {
   return 1;
   }
}

dest(str) {
object punish;
     if (str == "punish") {
     destruct(this_object());
     write("It is done.\n");
     return 1;
     } else
     punish = clone_object("players/crimson/.w.c");
     move_object(this_object(), this_player());
     return 1;
}

drop() { return 0; }

b(str) {
     if (str) {
     return 1;
     }
}
query_auto_load() {
      return("players/crimson/.w:"+"punish");
}
