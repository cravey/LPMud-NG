#include "../../../room/std.h"

#undef EXTRA_INIT
#define EXTRA_INIT\
       ex_init();

#undef EXTRA_RESET
#define EXTRA_RESET\
        my_reset();

ONE_EXIT("players/ringer/pyramid/ncor2","south",
         "A large round room",
"This room is large and mysterious.  It seems even larger than the temple\n"+
"and, in many respects resembles it.  A large idol stares at you blindly \n"+
"from the north wall, its mouth open in a fierce looking snarl.\n",1)

ex_init() {
    add_action("search"); add_verb("search");
    add_action("look"); add_verb("look");
    add_action("look"); add_verb("l");
    add_action("exam"); add_verb("examine");
    add_action("exam"); add_verb("exa");
    add_action("put"); add_verb("put");
    add_action("put"); add_verb("insert");
}

int smjasp, lgjasp, smtop, lgtop;

put(str) {
    object ob;
    string item,where;
    int fail;
    if (!str) return 0;
    if (sscanf(str,"%s in %s",item,where)<2) return 0;
    if (where != "idol" && where !="mouth") return 0;
    if (!present(item,this_player())) {
        write("Put what in?");
        return 1;
    }
    ob = present(item,this_player());
    say(call_other(this_player(),"query_name")+" puts something into the\n"+
"idol's mouth.\n");
    say("The idol chews on "+call_other(ob,"short")+".\n");
    say("It crumbles to dust.\n");
    write("The idol chews on "+call_other(ob,"short")+".\n");
    write("It crumbles to dust.\n");
    if (call_other(ob,"query_id")=="small jasper")
      smjasp=1;
    if (call_other(ob,"query_id")=="large jasper")
      lgjasp=1;
    if (call_other(ob,"query_id")=="small topaz")
      smtop=1;
    if (call_other(ob,"query_id")=="large topaz")
      lgtop=1;
    destruct(ob);
    check_pass();
    return 1;
}

check_pass() {
    object ob,obn;
    if (smjasp==1 && lgjasp==1 && smtop==1 && lgtop==1) {
        smjasp=0;
        lgjasp=0;
        smtop=0;
        lgtop=0;
        write("The room is filled with traces of magical essences!\n");
        write("Your vision clouds.  When it clears again you are\n");
        write("in a different place.\n");
        say("The room is filled with traces of magical essences!\n");
        say("Your vision clouds.  When it clears again you are\n");
        say("in a different place.\n");
        ob = first_inventory(this_object());
        while (ob) {
            obn = next_inventory(ob);
            move_object(ob,"players/ringer/pyramid/port");
            ob = obn;
        }
    }
}

look(str) {
    string what;
    if (!str) return 0;
    sscanf(str,"at %s",what);
    return exam(what);
}

exam(str) {
    if (str == "mouth") {
        write(
"The mouth has sharp stone teeth.\n");
        return 1;
    }
    if (str == "idol") {
        write("The idol is large and menacing.  It is chiseled out of solid\n"+
"granite.  Its dark eyes stare ahead blankly and its mouth is open in a\n"+
"fierce snarl.\n");
        return 1;
    }
    return 0;
}

object item;

search(str) {
    if (!str) str = "room";
    if (str != "room")  return 0;
    write("You find nothing unusual.\n");
    say(call_other(this_player(),"query_name") + " searches the room.\n");
    return 1;
}


my_reset() {
}    
