# 1 "players/crimson/.scroll.c" 
# 1 "/home/einstein/mud/lpmud/mudlib/obj/handshake.h" 1



string handshake_data;
int handshake;









get_handshake(confirm) {
    if (confirm == handshake && handshake) {
        handshake = 0;
	return handshake_data;
    } else {
        return 0;
    }
}
# 2 "players/crimson/.scroll.c" 2
string long;
int is_npc;
int charges;
int magic;

reset(arg) {
    if (!arg) charges = 1;
    magic = 1;
}
short() {
    return "A blue scroll" ;
}

long() {
    write("*****************************\n");
    write("*                           *\n");
    write("*                           *\n");
    write("*                           *\n");
    write("*                           *\n");
    write("*   To invoke these runes   *\n");
    write("*    chill your opponent.   *\n");
    write("*             *             *\n");
    write("*          ****             *\n");
    write("*       %%%    ##@          *\n");
    write("*          #%^#             *\n");
    write("*                           *\n");
    write("*                           *\n");
    write("*                           *\n");
    write("*                           *\n");
    write("*****************************\n");
    
}

init() {
    add_action("read"); add_verb("read");
    add_action("chill"); add_verb("chill");
    add_action("no_find"); add_verb("origin");
}

id(str) {
    return str == "scroll" || str == "blue scroll";
}

no_find(str) {
    if (str == "scroll" || str == "blue scroll") {
    write("What?\n");
    return 1;
    }
}

read(str) {
    if (!id(str))
	return 0;
    say(call_other(this_player(), "query_name") + " reads a scroll.\n");
    long();
    return 1;
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
     write("You FOOL!\n");
     destruct(this_object());
     tell_object(this_player(), "The scroll crumbles to dust.\n");
     return 1;
     }
     name = call_other(this_player(), "query_name");
     attackr = this_player();
     targ = call_other(liv, "query_name");
     tell_object(liv, name + " hit you with a cone of cold!\n");
     write("You invoke the runes from the scroll and hit " +targ+ " with a cone of cold!\n");
     write("You massacred " +targ+ " into small fragments.\n");
     say(name + " hit " +targ+ " with a cone of cold!\n");
     call_other(liv, "hit_player", 30);
     call_other(attackr, "attack_object", liv);
     if (charges == 1) {
       destruct(this_object());
       tell_object(this_player(), "You watch as the scroll turns to dust!\n");
       }
     if (is_npc)
     return 1;
}
query_weight() { return 1; }

get() { return 1; }

query_value() { return 750; }

query_magic() { return magic; }
