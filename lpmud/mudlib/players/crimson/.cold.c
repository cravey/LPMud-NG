/* This is the property of Crimson! Do not copy! Do not touch! */
/* If this scroll or anything looking like this file is found in */
/*   your possesion you WILL be punished!                        */
string long;
int is_npc;
int charges;

reset(arg) {
    if (!arg) charges = 1;
}
short() {
    return "A cone of cold spell (memorized)";
}

long() {
    write("A cone of cold spell (memorized).\n");
    return 1;
    
}

init() {
    add_action("chill"); add_verb("chill");
    add_action("drop_object"); add_verb("drop");
    add_action("give_object"); add_verb("give");
}

id(str) {
    return str == "cone of cold spell" || str == "spell" || str == "cold spell";
}

chill(str) {
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
     tell_object(this_player(), "You forgot the cone of cold spell.\n");
     return 1;
     }
     name = call_other(this_player(), "query_name");
     attackr = this_player();
     targ = call_other(liv, "query_name");
     tell_object(liv, name + " hit you with a cone of cold!\n");
     write("You cast the spell and hit " +targ+ " with a cone of cold!\n");
     say(name + " hit " +targ+ " with a cone of cold!\n");
     call_other(liv, "hit_player", random(30));
     call_other(attackr, "attack_object", liv);
     if (charges == 1) {
       destruct(this_object());
       }
     if (is_npc)
     return 1;
}
query_weight() { return 1; }

get() { return 1; }

query_value() { return 750; }

drop_object(str) {
object player;
    player = this_player();
    if (str == "all") {
    drop_object("cold spell");
    return 1;
    }
    if (!str || !id(str))
    return 0;
    write("You forgot the cone of cold spell.\n");
    say(call_other(this_player(), "query_name") + " forgets a cone of cold spell.\n");
    destruct(this_object());
    return 1;
}

give_object(str) {
string dest;

  dest = present(lower_case(str), environment(this_player()));
   if (str == "spell to " +dest) {
   write("You forgot the cone of cold spell.\n");
   say(call_other(this_player(), "query_name") + " forgets a cone of cold spell.\n");
   destruct(this_object());
   return 1;
   }
}
