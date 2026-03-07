#include "../../room/std.h"

#undef EXTRA_INIT
#define EXTRA_INIT ex_init();

#undef EXTRA_RESET
#define EXTRA_RESET\
          ex_reset();

TWO_EXIT("players/ringer/dk_ruin", "south",
         "players/ringer/precip","north",
         "You are on the high point of the peak",
"You are on the highest point of the peak.  The mountain seems to have \n"
+"been leveled here to lay the foundation of a once great fortress.  Now\n"
+"the fortress seems to have been shorn away leaving only hard bedrock.\n"
+"You notice strange tracks here on the ground.  They appear to be tracks\n"
+"of a bird.  A LARGE bird...\n",1)

object grif;

ex_init() {
    add_action("mount"); add_verb("mount");
    add_action("mount"); add_verb("ride");
}

ex_reset() {
    grif = 0;
}

call_grif() {
    if (!grif) {
    write(
"In the distance you hear the high pitched cry of a bird of prey.  Suddenly\n"
+"a large griffon rises from the edge of the cliff.  It lands near you and \n"
+"sits peacefully.  You notice that it is harnessed and saddled!\n");
    say(call_other(this_player(),"query_name") + " uses the staff.\n" +
"In the distance you hear the high pitched cry of a bird of prey.  Suddenly\n"
+"a large griffon rises from the edge of the cliff.  It lands nearby and \n"
+"sits peacefully.  You notice that it is harnessed and saddled!\n");
    grif = clone_object("obj/monster");
    call_other(grif,"set_name","griffon");
    call_other(grif,"set_alias","griffon");
    call_other(grif,"set_short","A large griffon");
    call_other(grif,"set_long",
"The griffon is very large.  It seems calm and expectant.  It is bridled and\n"
+"saddled.\n");
    call_other(grif,"set_level",15);
    call_other(grif,"set_al",400);
    call_other(grif,"set_ac",5);
    call_other(grif,"set_wc",15);
    call_other(grif,"set_aggressive",0);
    call_other(grif,"set_chance",20);
    call_other(grif,"set_spell_mess1","Griffon breathes fire");
    call_other(grif,"set_spell_mess2","Griffon breathes fire");
    call_other(grif,"set_spell_dam",35);
    move_object(grif,this_object());
    return 1;
}
    write("There's one here already!\n");
    return 0;
}

mount(str) {
    if (!str) return 0;
    if (!grif) return 0;
    if (call_other(grif,"query_attack")) {
        write("The griffon is too upset at the moment to ride...\n");
        return 1;
    }
    if (call_other(grif,"id",str)) {
        say(
call_other(this_player(),"query_name") + " gets in the griffon's saddle.\n"
+"The griffon flies away with him.\n");
        write(
"You climb into the griffon's saddle.  The griffon needs no prodding as it\n"
+"slips into a well trained pattern.  It stands and with a powerful graceful\n"
+"motion soars off over the cliff.  The griffon quickly finds its course and\n"
+"flies straight and unwavering.  Within a few moments a large glittering \n"
+"castle comes into view.  The castle seems built (like the now defunct one)\n"
+"at the top of a high peak with sheer cliff on all sides. The griffon lands\n"
+"and you dismount.\n");
        move_object(this_player(),"players/ringer/c_gates");
        move_object(grif,"players/ringer/c_gates");
        grif = 0;
        return 1;
    }
    return 0;
}
