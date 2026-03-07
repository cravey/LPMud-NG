# 1 "players/crimson/.missile.c" 
int is_npc;
int gived;
int charges;

reset(arg) {
    if (!arg) charges = 1;
}
short() {
    return "A magic missile spell (memorized)";
}

long() {
    write("A magic missile spell (memorized).\n");
    return 1;
}

init() {
    add_action("missile"); add_verb("missile");
    add_action("no_find"); add_verb("origin");
    add_action("drop_object"); add_verb("drop");
    add_action("drop_object"); add_verb("put");
}

drop() {
   gived += 1;
   if (gived == 1)
   write("You forgot a magic missile spell.\n");
   say(call_other(this_player(), "query_name") + " forgets a magic missile spell.\n");
   destruct(this_object());
   return 1;
}

drop_object(str) {
   if (str == "all") {
       drop_object("magic missile spell");
       return 1;
   }
   if (!str || !id(str))
       return 0;
   write("You forgot a magic missile spell.\n");
   say(call_other(this_player(), "query_name") + " forgets a magic missile spell.\n");
   destruct(this_object());
   return 1;
}

no_find(str) {
   if (str == "spell" || str == "magic missile spell") {
   write("What?\n");
   return 1;
   }
}

id(str) {
    return str == "spell" || str == "magic missile spell";
}

missile(str) {
     object liv;
     object who;
     string name;
     string targ;
     string attackr;

     if (!str) { return 1; }
     liv = present(lower_case(str), environment(this_player()));
     if (!liv) {
     write("That is not here.\n");
     return 1;
     }
     if (!living(liv)) {
     write("That is not a living thing!\n");
     return 1;
     }
     if (liv == this_player()) {
     destruct(this_object());
     tell_object(this_player(), "You singe your eyebrows.\n");
     return 1;
     }
     name = call_other(this_player(), "query_name");
     attackr = this_player();
     targ = call_other(liv, "query_name");
     tell_object(liv, name + " hit you with a magic missile!\n");
     write("You hit " +targ+ " with a magic missile.\n");
     say(name + " hit " +targ+ " with a magic missile.\n");
     call_other(liv, "hit_player", random(10));
     call_other(attackr, "attack_object", liv);
     if (charges == 1) {
       destruct(this_object());
       }
     if (is_npc)
     return 1;
}
query_weight() { return 0; }

get() { return 1; }
