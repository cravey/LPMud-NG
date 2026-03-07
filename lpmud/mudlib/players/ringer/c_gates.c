#include "../../room/std.h"

#undef EXTRA_INIT
#define EXTRA_INIT ex_init();

#undef EXTRA_RESET
#define EXTRA_RESET\
          ex_reset();

ONE_EXIT("players/ringer/courtyd", "north",
	   "You are at the gates of a glimmering castle",
"You are at the gates of a glimmering castle.  This castle is built atop\n"
+"an incredible peak.  You cannot conceive of getting here without the\n"
+"ability to fly.  The gates are open and lead north to a courtyard.\n",1)

object grif;

ex_init() {
    add_action("mount"); add_verb("mount");
    add_action("mount"); add_verb("ride");
}

ex_reset() {
}

mount(str) {
    if (!str) return 0;
    grif = present("griffon");
    if (!grif) return 0;
    if (call_other(grif,"query_attack")) {
        write("The griffon is too upset at the moment to ride...\n");
        return 1;
    }
    if (call_other(grif,"id",str)) {
        say(
call_other(this_player(),"query_name") + " gets in the griffon's saddle\n"
+"and flies away.\n");
        write(
"You climb into the griffon's saddle.  The griffon takes to the air once\n"
+"again.  It swiftly takes you back to the spot where it first picked you\n"
+"up and deposits you there.  Having done its duty for the day, the griffon\n"
+"flies off into the mists from whence it came.\n");
        move_object(this_player(),"players/ringer/highpt");
        destruct(grif);
        grif = 0;
        return 1;
    }
    return 0;
}

call_grif() {
    grif = present("griffon");
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
