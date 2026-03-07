int climb_rope;
int magic;
string tag;

reset(arg) {
    set_light(1);
    magic = 1;
    if (arg)
	return;
}

query_where() {
object where;
       where = environment(this_player());
       return where;
}

init() {
    add_action("climb"); add_verb("climb");
    add_action("look"); add_verb("look");
    add_action("exit"); add_verb("exit");
    add_action("no_find"); add_verb("origin");
}

no_find(str) {
   if (str == "rope") {
   write("What?\n");
   return 1;
   }
}

here() {
object dest;
object player;
 
     dest = query_where();
     player = this_player();
     move_object(player, dest);
     return 1;
}

look(str) {
    if(!str && environment(this_player()) == this_object()) {
    write("You are in your own personal bit of extradimensional\n");
    write("space.  To see the outside world, 'look through hole'.\n");
    write("This window (hole) is centered on the rope.  If you\n");
    write("want to exit from this place, 'climb down rope'.\n");
    return 0;
    } else
    if (str == "through hole" && (environment(this_player()) == this_object())) {
    move_object(this_player(), environment(this_object()));
    call_other(this_player(), "look", 0);
    move_object(this_player(), this_object());
    return 1;
    } else
    if (str) {
    return 0;
    } else
    return 0;
}

climb(str) {
    
    if (str == "up rope" && climb_rope == 1 && (environment(this_player()) != this_object())) {
    write("You can't to that! This is " +tag+ "'s rope trick!\n");
    return 1;
    } else
    if (str == "up rope" && (environment(this_player()) == this_object())) {
    write("You can not continue up the rope from here.\n");
    return 1;
    } else
     if (str == "up rope") {
     move_object(this_object(), environment(this_player()));
     move_object(this_player(), this_object());
     tag = call_other(this_player(), "query_name", 0);
     look();
     climb_rope = 1;
     return 1;
     } else
     if (str == "down rope" && (environment(this_player()) == this_object())) {
     write("The extradimension space closes above you and the rope trick disappears.\n");
     move_object(this_player(), environment(this_object()));
     move_object(this_object(), "players/crimson/storeroom");
     destruct(this_object());
     call_other(this_player(), "look", 0);
     return 1;
     }
}

exit(str) {
   if (str != "climb down rope") {
   return 1;
   }
   return 1;
}

query_weight() {
    return 1;
}

short() {
    return "A rope dangling in mid-air";
}

long() {
    if (environment(this_player()) != this_object()) {
    write("It looks as though it were affixed to something above it.\n");
    write("However, there is NOTHING THERE?!\n");
    return 1;
    }
}

id(str) {
    return str == "rope";
}

get() {
    return 0;
}

query_magic() {
    return magic;
}

realm() { return "NT"; }
