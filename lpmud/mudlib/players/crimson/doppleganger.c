#include "std.h"

object rogue;
object doppleganger;

#undef EXTRA_RESET
#define EXTRA_RESET\
   extra_reset();

#undef EXTRA_INIT
#define EXTRA_INIT\
    add_action("kill"); add_verb("kill");\
    add_action("kill"); add_verb("fireball");\
    add_action("kill"); add_verb("shock");\
    add_action("kill"); add_verb("chill");\
    add_action("kill"); add_verb("strike");\
    add_action("kill"); add_verb("missile");

ONE_EXIT("players/crimson/eastrail", "west",
          "The Nursery",
          "This is the nursery for children too young to attend\n" +
          "mourning services.  Various toys are scattered about\n" +
          "the room. Overall, the whole place looks very sterile.\n", 1)

extra_reset() {
    if ((!doppleganger || !living(doppleganger)) && (!rogue || !living(rogue))) {
          rogue = clone_object("obj/monster");
          call_other(rogue, "set_name", "rogue");
          call_other(rogue, "set_short", "A rogue");
          call_other(rogue, "set_alias", "rogue");
          call_other(rogue, "set_long", "A squint eyed rogue.\n");
          call_other(rogue, "set_level", 4);
          call_other(rogue, "set_wc", 3);
          call_other(rogue, "set_ac", 12);
          call_other(rogue, "set_race", "doppleganger");
          call_other(rogue, "set_al", 300);
          call_other(rogue, "set_eps", 1);
          move_object(rogue, this_object());
          }
}

kill(str) {
object killer;
string name;
string level;
string title;
string align;
string eps;

    killer = this_player();
    level = call_other(this_player(), "query_level", 0);
    name = call_other(this_player(), "query_name", 0);
    title = call_other(this_player(), "query_title", 0);
    align = call_other(this_player(), "query_alignment", 0);
    if (level>20) {
    level = 19;
    }
    eps = 5000*level;
    if (str == "rogue" || str == "rogue" || str == "doppleganger" && present("rogue")) {
    destruct(rogue);
    tell_room(this_object(), "The rogue takes on a transformation!\n");
    if (!doppleganger || !living(doppleganger)) {
    doppleganger = clone_object("obj/monster");
    call_other(doppleganger, "set_name", "doppleganger");
    call_other(doppleganger, "set_short", name+" "+title+" (shady)");
    call_other(doppleganger, "set_alias", "doppleganger");
    call_other(doppleganger, "set_race", "doppleganger");
    call_other(doppleganger, "set_level", level);
    call_other(doppleganger, "set_wc", 24);
    call_other(doppleganger, "set_ac", 9);
    call_other(doppleganger, "set_al", -(align));
    call_other(doppleganger, "set_eps", eps);
    call_other(doppleganger, "set_hp", 200);
    call_other(doppleganger, "set_chance", 40);
    call_other(doppleganger, "set_spell_mess1", "The wolf howls in joy for the blood bath.\n");
    call_other(doppleganger, "set_spell_mess2", "The wolf's claws sink deep into your flesh.\n");
    call_other(doppleganger, "set_spell_dam", 15);
    call_other(doppleganger, "set_aggressive", 0);
    move_object(doppleganger, this_object());
    call_other(doppleganger, "attack_object", killer);
    return 1;
    }
  }
}
