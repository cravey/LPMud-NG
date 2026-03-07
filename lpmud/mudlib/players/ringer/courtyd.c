#include "../../room/std.h"

#undef EXTRA_RESET
#define EXTRA_RESET\
        ex_reset();

#undef EXTRA_INIT
#define EXTRA_INIT\
       ex_init();

ONE_EXIT("players/ringer/c_gates", "south",
	   "You are in the courtyard",
"You are in the courtyard of a glittering castle.  You see one doorway\n"
+"leading to the north.  Above this doorway is a sign.  Its words seem to be\n"
+"made of magical silvery dust.\n",1)

object gulf,drag,item;

ex_init() {
    add_action("north"); add_verb("north");
    add_action("read"); add_verb("read");
}

read(str) {
    if (str == "sign") {
        write("The sign says:  No living mortal may enter here!\n");
        return 1;
    }
}

north() {
    if (!call_other(this_player(),"query_ghost")) {
        write("A strong magical force repells you!\n");
        return 1;
    }
    call_other(this_player(),"move_player","north#players/ringer/bsr1");
    call_other(this_player(),"remove_ghost");
    return 1;
}

climb(str){
    if (!str) str = "pillar";
    if (str != "pillar") return 0;
    call_other(this_player(),"move_player","climbing#players/ringer/pillar");
    return 1;
}

ex_reset() {
    int i;
    if (!present("gulf the magician")) {
        gulf = clone_object("obj/monster");
        call_other(gulf,"set_name","gulf the magician");
        call_other(gulf,"set_alias","gulf");
        call_other(gulf,"set_level",15);
        call_other(gulf,"set_ac",3);
        call_other(gulf,"set_long","Gulf is old and peaceful looking.\n");
        call_other(gulf,"set_aggressive",0);
        call_other(gulf,"set_chance",35);
     call_other(gulf,"set_spell_mess1","Gulf throws a molten ball of iron.\n");
     call_other(gulf,"set_spell_mess2","Gulf throws a molten ball of iron.\n");
        call_other(gulf,"set_spell_dam",70);
        call_other(gulf,"set_al",900);
        move_object(gulf,this_object());
        if (present("amber stone")) {
            item = present("amber stone");
            move_object(item,gulf);
        }
        if (!present("amber stone",gulf)) {
            item = clone_object("players/ringer/trapstone");
            call_other(item,"set_id","amber stone");
            call_other(item,"set_alias","stone");
            call_other(item,"set_short","A translucent amber stone");
            call_other(item,"set_long","The stone is translucent.  You find yourself wanting to gaze into it.\n");
            call_other(item,"set_value",100);
            call_other(item,"set_weight",1);
            move_object(item,gulf);       
        }
        item = clone_object("players/ringer/spitem");
        call_other(item,"set_id","crystaline staff");
        call_other(item,"set_real_id","staff of lightning");
        call_other(item,"set_charges",5);
        call_other(item,"set_spell",3);
        call_other(item,"set_alias","staff");
        call_other(item,"set_short","A crystaline staff");
        call_other(item,"set_value",75);
        call_other(item,"set_weight",1);
        move_object(item,gulf);
        if (!present("bronze dragon")) {
            drag = clone_object("obj/monster");
            call_other(drag,"set_name","bronze dragon");
            call_other(drag,"set_alias","dragon");
            call_other(drag,"set_short","A bronze dragon");
            call_other(drag,"set_long","The dragon is very large.  His scales look like they are made of polished\n"
+"bronze.  His stance is very regal.\n");
            call_other(drag,"set_level",17);
            call_other(drag,"set_ac",5);
            call_other(drag,"set_wc",15);
            call_other(drag,"set_hp",500);
            call_other(drag,"set_al",700);
            call_other(drag,"set_aggressive",0);
            move_object(drag,this_object());
        }  
    }
}

