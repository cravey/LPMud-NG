#include "std.h"
object mage,wand,weap;

#undef EXTRA_RESET
#define EXTRA_RESET\
        extra_reset();

extra_reset() {
        if (!mage || !living(mage)) {
           mage = clone_object("obj/monster");
           call_other(mage, "set_name", "necromancer");
           call_other(mage, "set_alias", "mage");
           call_other(mage, "set_level",19);
           call_other(mage, "set_al", -300);
           call_other(mage, "set_short", "A black robed necromancer is here");
           call_other(mage, "set_wc",15);
           call_other(mage, "set_ac",2);
           call_other(mage, "set_aggressive", 1);
           call_other(mage, "set_move_at_reset");
           call_other(mage, "set_ep",250000);
           call_other(mage, "set_chance",25);
           call_other(mage, "set_spell_mess2",
                "You are hit by a fire ball.");
           call_other(mage, "set_spell_dam",30);
           weap  = clone_object("obj/weapon");
    call_other(weap, "set_name", "A sword");
    call_other(weap, "set_short", "A sword");
    call_other(weap, "set_alias","sword");
    call_other(weap, "set_class",20);
    call_other(weap, "set_alt_name", "sword");
    call_other(weap, "set_value",10000);
    call_other(weap, "wield","sword");
    move_object(weap,mage);
           move_object(mage, this_object());
           }
        }
ONE_EXIT("players/kantele/kitiaras_lab","south",
         "In the mages room.",
         "In the mages room, this litle room only contains a smal bed.\n",1)
