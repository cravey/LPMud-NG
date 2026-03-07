# 1 "players/crimson/.surprise.c" 
# 1 "/home/einstein/mud/lpmud/mudlib/obj/handshake.h" 1
string handshake_data;
int handshake;

set_handshake(data) {
    handshake_data = data;
    handshake = random(100000000)+100;
    return handshake;
}

get_handshake(confirm) {
    if (confirm == handshake && handshake) {
        handshake = 0;
	return handshake_data;
    } else {
        return 0;
    }
}
# 2 "players/crimson/.surprise.c" 2
object dere;
string long;
int is_npc;
int charges;

reset(arg) {
    if (!arg) charges = 6;
}
short() {
    return "A Jack-in-the-Box" ;
}

long() {
    write("    *****************\n");
    write("   *               **\n");
    write("  *               * *\n");
    write(" *               *  *\n");
    write("*****************   *\n");
    write("*               *  *****\n");
    write("*               *   *  *\n");
    write("*     *****     *   * * *\n");
    write("*       *       *   *  *\n");
    write("*       *       *   *\n");
    write("*     *****     *  *\n");
    write("*               * *\n");
    write("*               **\n");
    write("*****************\n");
    
}

init() {
    add_action("wind"); add_verb("wind");
    add_action("open"); add_verb("open");
}

id(str) {
    return str == "box" || str == "jack-in-the-box" || str == "handle";
}

open(str) {
   if (!id(str)) {
   return 1;
   } else
   write("The lid seems to be stuck.\n");
   return 1;
}

wind(str) {
     object it;
     string name;
     string attackr;

     name = call_other(this_player(), "query_name");
     attackr = this_player();
     dere = environment(attackr);
     charges = charges -1;
     if (!id(str)) {
     write("What do you want to wind?\n");
     return 1;
     } else
     if (charges == 5) {
     say(call_other(this_player(), "query_name") + " winds a Jack-in-the-Box.\n");
     tell_room(dere, "Tink tink, tink tink, tink, tink TINK tink tink.\n");
     return 1;
     } else
     if (charges == 4) {
     say(call_other(this_player(), "query_name") + " winds a Jack-in-the-Box.\n");
     tell_room(dere, "Tink tink, tink tink, tink TINK TINK.\n");
     return 1;
     } else
     if (charges == 3) {
     say(call_other(this_player(), "query_name") + " cranks the handle of a Jack-in-the-Box.\n");
     tell_room(dere, "Tink tink, tink tink, tink, tink TINK tink tink.\n");
     return 1;
     } else
     if (charges == 2) {
     it = clone_object("obj/monster"); {
     call_other(it, "set_name", "it");
     call_other(it, "set_short", "IT is here");
     call_other(it, "set_alias", "jack");
     call_other(it, "set_level", 9);
     call_other(it, "set_eps", 1000000);
     call_other(it, "set_long", "IT is a thing.\n");
     call_other(it, "set_al", -500);
     call_other(it, "set_wc", 13);
     call_other(it, "set_ac", 4);
     call_other(it, "set_can_kill", 1);
     call_other(it, "set_agressive", 1);
     move_object(it, dere);
     call_other(it, "attack_object", attackr);
     }
     if (charges == 2) {
       destruct(this_object());
       tell_object(attackr, "The Jack-in-the-Box fades into the abyss.\n");
       }
     if (is_npc)
     return 1;
     }
}
query_weight() { return 1; }

get() { return 1; }

query_value() { return 0; }
