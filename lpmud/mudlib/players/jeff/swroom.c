#include "room.h"

object black_sword;

#undef EXTRA_RESET
#define EXTRA_RESET fix_sword();

fix_sword()
{
/*    if (!black_sword ) { */
	black_sword = clone_object("obj/weapon");
	call_other(black_sword, "set_name", "Runeblade");
	call_other(black_sword, "set_alias", "sword");
	call_other(black_sword, "set_short", "Runeblade");
	call_other(black_sword, "set_alt_name", "runeblade");
	call_other(black_sword, "set_long", "This is a very old blade.\n"+
		"It's covered with blood red ancient runes.\n");
	call_other(black_sword, "set_read", 
		"Danger sits with this blade\n"+
		"It can help its master but woe to its slave!\n");
	call_other(black_sword, "set_class", 20);
	call_other(black_sword, "set_weight", 3);
	call_other(black_sword, "set_value", 4000);
	call_other(black_sword, "set_hit_func", this_object());
        move_object(black_sword, this_object());
/*    } */
}

ONE_EXIT("room/ruin", "south",
	 "The sword room",
	 "You are in the room of the black sword.\n", 1)

weapon_hit(attacker)
{
    string alig;
    int num;
    object tp;
    tp=this_player();
    num=random(20); 
    if (num == 0) {
       if (tp->query_level() > 10) {
            write("Runeblade tryes to reject you.\n");
            return 0;
       }
       write("Runeblade rejects you as master!\n");
       write("and returns home.....\n");
       call_other(black_sword,"drop",0);
       move_object(black_sword, this_object());
       return 0;
    }
    if ( num <= 10) {
       write("Runeblade glows black.\n");
       tell_room(this_object(),"The Runeblade starts glowing black\n");
       return 1;
    }
    if ( num <= 15) {
       write("Runeblade moans darkly.\n");
       tell_room(this_object(),"The Runeblade moans darkly.\n");
       return 15;
    }
    if (num <=19) {
       write("Runeblade howls malevolence.\n");
       write("Runeblade stole some life force\n");
       write("You feel stronger!\n");
       call_other(this_player(),"heal_other", 200);
       return 20;
    }
}
