# 1 "players/crimson/.detect.c" 
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
# 2 "players/crimson/.detect.c" 2
int i;
int gived;
string pointo;

id(str) { return str == "spell" || str == "detect magic spell"; }

long() {
    write("A detect magic spell (memorized).\n");
}

short() {
     return "A detect magic spell (memorized)";
}

query_weight() {
     return 0;
}

init() {
    add_action("cast"); add_verb("cast");
    add_action("set"); add_verb("scrutinize");
    add_action("no_find"); add_verb("origin");
    add_action("drop_object"); add_verb("drop");
    add_action("drop_object"); add_verb("put");
}

no_find(str) {
   if (str == "spell" || str == "detect magic spell") {
   write("What?\n");
   return 1;
   }
}

cast(str) {
object magicobject;
string player;

    magicobject = pointo;
    player = call_other(this_player(), "query_name", 0);
    if (!str) { return 0; }
    if (str == "detect magic" && !magicobject) {
    write("What do you want to detect magic on?\n");
    return 1;
    } else
    if (str == "detect magic" && i != 1) {
    write("What do you want to detect magic on?\n");
    return 1;
    }
             if (str == "detect magic" && (call_other(magicobject, "query_magic", 0))) {
             write("You sense the presence of magic!\n");
             say(player+ " casts a spell.\n");
             destruct(this_object());
             return 1;
             } else
             if (str == "detect magic" && (!call_other(magicobject, "query_magic", 0))) {
             write("Bummer, no magic here.\n");
             destruct(this_object());
             return 1;
             } else
             if(str) {
             return 0;
             }
}

set(arg) {

    if (!arg) {
        return 0;
    }
    else {

   pointo = (present(lower_case(arg), environment(this_player())) || present(lower_case(arg), this_player()));
        write("Ok.\n");
        i = 1;
    }
   return 1;
}

get() {
    return 1;
}

drop() {
   gived += 1;
   if (gived == 1)
   write("You forgot a detect magic spell.\n");
   say(call_other(this_player(), "query_name") + " forgot a detect magic spell.\n");
   destruct(this_object());
   return 1;
}

drop_object(str) {
   if (str == "all") {
   drop_object("detect magic spell");
   return 1;
   }
   if (!str || !id(str)) 
   return 0;
   write("You forgot a detect magic spell.\n");
   say(call_other(this_player(), "query_name") + " forgot a detect magic spell.\n");
   destruct(this_object());
   return 1;
}
