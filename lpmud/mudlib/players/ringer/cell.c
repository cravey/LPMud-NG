#include "../../room/std.h"

#undef EXTRA_RESET
#define EXTRA_RESET\
        my_reset();

ONE_EXIT("players/ringer/dgeon", "north",
	   "You are in a small prison cell",
"You are in a small prison cell.  The cell is grimy and dirty.\n",0)

object p;

my_reset() {
    if (!p || !living(p)) {
        p = clone_object("obj/monster.talk.c");
        call_other(p,"set_name","lovely princess");
        call_other(p,"set_alias","princess");
        call_other(p,"set_short","Lovely princess");
      call_other(p,"set_long","The princess is very beautifull to behold.\n");
        call_other(p,"set_level",1);
        call_other(p,"set_object",this_object());
        call_other(p,"set_function","free");
        call_other(p,"set_type","arrives");
        call_other(p,"set_match","");
        move_object(p,this_object());
    }
}

free(str) {
    string who;
    object ob,person;

    sscanf(str,"%s arrives",who);
    person = present(lower_case(who),environment(p));
    if (person) {
        tell_object(person,
                    "The princess stands up as you enter.  She is incredibly lovely to behold.\n"
                    +"She is on the brink of tears as she hugs you.  She explains that the evil\n"
                    +"groo have been holding her prisoner for her magic powers.  But now the \n"
                    +"magical bars that held her are open.  She smiles, hands you a parting gift\n"
                    +"and vanishes before your eyes.\n");
        ob = clone_object("players/ringer/mitem");
        call_other(ob,"set_id","gold bracelet");
        call_other(ob,"set_real_id","gold bracelet");
        call_other(ob,"set_alias","bracelet");
        call_other(ob,"set_short","A gold bracelet");
        call_other(ob,"set_value",50);
        call_other(ob,"set_weight",1);
        move_object(ob,person);
        destruct(p);
    }
}

