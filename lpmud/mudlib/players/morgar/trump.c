int i, live;
object targetobj;

id(str) {
return ((str == "trump") || (str == "card") || (str == "trump card"));
}

reset() { }

long() {
    write("This is a trump card.\n");
    if (i == 1) {
        write("Looking into the mirror you see:\n");
        call_other(targetobj,"long");
    } else {
        write("It is currently blank.\n");
    }
}

short() { return "A trump card"; }

query_value() { return 300; }

init() {
    add_action("use"); add_verb("use");
    add_action("use"); add_verb("enter");
    add_action("set"); add_verb("set");
    add_action("notdone"); add_verb("contact");
}

notdone() { write("Function not done.\n"); }

use(arg) {
    if (!arg || !id(arg)) {
        return 0;
    } else {
        if (i != 1) {
            write("Mirror has not had a destination set.\n");
            return 1;
        } else {
            write("You step through the mirror.\n");
            say(call_other(this_player(),"query_name") + " steps into a mirror.\n");
            move_object(this_player(), targetloc);
            say(call_other(this_player(),"query_name") + " steps out of a mirror.\n");
            call_other(this_player(),"command","look");
            return 1;
        }
    }
}

set(arg) {
    if (!arg || arg != "mirror") {
        write("Set what?\n");
    } else {
        if(call_other(this_player(),"query_level",0) <20) {
            write("Only a wizard may set a mirror!\n");
        } else {
            write("You set the mirror's destination.\n");
            targetloc = environment(this_player());
            i=1;
        }
    }
    return 1;
}

roomset(arg) {
    targetloc = arg;
    i=1;
}

get() {
    return(call_other(this_player(),"query_level",0) >= 20);
}

query_weight() { return 1; }





int i, charges;
string targetloc;

id(str) { return str == "amulet"; }

reset() {
    charges=random(6);
}

long() {
    write("A small magical amulet is here.\n");
    if (i == 1) {
        write("Looking into the amulet you see:\n");
        call_other(targetloc,"long");
    }
}

short() {
     return "A small magical amulet";
}

query_value()
{
    return 100;
}

init() {
    add_action("use"); add_verb("use");
    add_action("set"); add_verb("set");
}

use(arg) {
    string realm, destrealm;
    if (!arg || arg != "amulet") {
        return 0;
    } else {
        if (i != 1) {
            write("The amulet has not had a destination set.\n");
            return 1;
        } else {
            realm=call_other(environment(this_player()),"realm",0);
            destrealm=call_other(targetloc,"realm",0);
            if ((realm == destrealm) && (realm != "NT")) {
                write("As you use the amulet everything shimmers away...\n");
                say(call_other(this_player(),"query_name") + " shimmers and fades.\n");
                move_object(this_player(), targetloc);
                say(call_other(this_player(),"query_name") + " shimmers into solidity.\n");
		call_other(this_player(),"command","look");
                if (charges == 1) {
                    move_object(this_object(), "players/morgar/store");
                    write("The amulet did not make it through the transport.\n");
                }
                charges=charges-1;
            } else {
                write("The amulet struggles to take you with it, but fails.\n");
                move_object(this_object(), targetloc);
                write("The amulet shimmers and fades!\n");
	    }
            return 1;
        }
    }
}

set(arg) {
    if (!arg || arg != "amulet") {
        return 0;
    } else {
        write("You set the amulet's destination.\n");
        targetloc = environment(this_player());
        i=1;
    }
    return 1;
}

get() {
    return 1;
}

query_weight() {
    return 1;
}
