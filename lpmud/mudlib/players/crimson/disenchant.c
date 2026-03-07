#include "std.h"

object disenchanter;

#undef EXTRA_RESET
#define EXTRA_RESET\
  extra_reset();

#undef EXTRA_INIT
#define EXTRA_INIT\
  add_action("fireball"); add_verb("fireball");\
  add_action("shock"); add_verb("shock");\
  add_action("missile"); add_verb("missile");\
  add_action("strike"); add_verb("strike");\
  add_action("strike"); add_verb("chill");
  
fireball(str) {
        if(!str || str == "beast" || str == "disenchanter beast" || str == "disenchanter") {
        call_other(disenchanter, "heal_self", random(40));
        call_other(this_player(), "attack_object", disenchanter);
        write("The beast absorbes your magical energies.\n");
        return 1;
        }
}

shock(str) {
       if(!str || str == "beast" || str == "disenchanter beast" || str == "disenchanter") {
       call_other(disenchanter, "heal_self", random(30));
       call_other(this_player(), "attack_object", disenchanter);
       write("The beast absorbes your magical energies.\n");
       return 1;
       }
}

missile(str) {
      if(!str || str == "beast" || str == "disenchanter beast" || str == "disenchanter") {
      call_other(disenchanter, "heal_self", random(20));
      call_other(this_player(), "attack_object", disenchanter);
      write("The beast absorbes your magical energies.\n");
      return 1;
      }
}

strike(str) {
     if(!str || str == "beast" || str == "disenchanter beast" || str == "disenchanter") {
     call_other(disenchanter, "heal_self", 30);
     call_other(this_player(), "attack_object", disenchanter);
     write("The beast bathes in the power of the scroll.\n");
     return 1;
     }
}

extra_reset() {
    if (!disenchanter || !living(disenchanter)) {
        object bag;
	disenchanter = clone_object("obj/monster");
	call_other(disenchanter, "set_name", "disenchanter");
	call_other(disenchanter, "set_short", "A Disenchanter");
        call_other(disenchanter, "set_long", "Fizzywig looks like a tired grey old man.\n");
        call_other(disenchanter, "set_alias", "beast");
	call_other(disenchanter, "set_level", 19);
	call_other(disenchanter, "set_wc", 20);
        call_other(disenchanter, "set_eps", 1000000);
        call_other(disenchanter, "set_ac", 8);
        call_other(disenchanter, "set_al", 1000);
        call_other(disenchanter, "set_chance", 25);
        call_other(disenchanter, "set_spell_mess1", "Fizzywig invokes a cone of cold\n" +
           " from a scroll.\n");
        call_other(disenchanter, "set_spell_mess2", "Fizzywig read a scroll and hits\n" +
           " you with a cone of cold!\n");
        call_other(disenchanter, "set_spell_dam", 40);
        call_other(disenchanter, "set_aggressive", 0);
        move_object(disenchanter, this_object());
        bag = clone_object("players/crimson/newbag");
        move_object(bag, disenchanter);
    }
}

realm() { return "NT"; }
#undef EXTRA_RESET
#define EXTRA_RESET\
    extra_reset();

ONE_EXIT("players/crimson/iceunder4", "west",
         "Fizzywig's hidey hole",
         "How strange! Someone must be hiding out here. There are linens folded\n"+
         "neatly over a floormat.  Light seems to eminate from nowhere.  Various\n" +
         "personals are scattered about the squarely carved cavern room.\n", 1)
