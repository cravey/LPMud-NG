#include "../../../room/std.h"

#undef EXTRA_INIT
#define EXTRA_INIT\
       ex_init();

TWO_EXIT("players/ringer/pyramid/desert11","up",
         "players/ringer/pyramid/pyr2","north",
         "Stone entryway",
"This is a long stone entryway.  The cool darkness gives great relief from\n"+
"the desert above.  Light filters through from a stone entrance above. A\n"+
"long dark hall leads north.  There are some ancient writings on the wall\n"+
"here.\n",0)

ex_init() {
    add_action("search"); add_verb("search");
    add_action("read"); add_verb("read");
    add_action("read"); add_verb("exa");
    add_action("look"); add_verb("look");
    add_action("look"); add_verb("l");
}

read(str) {
    if (str=="wall" || str=="walls" || str=="writings") {
        say(call_other(this_player(),"query_name") + " looks at wall.\n");
        if (present("amulet of tongues",this_player()) ||
            present("onyx amulet",this_player())) {
            write("\nThe onyx amulet glows brightly!\n");
            write(
"The writings explain that this is the sacred crypt of Xyphelod, the king\n"+
"of Cyruyns.  Xyphelod, his family and his court magician are buried here.\n");
            write(
"The writings go on to warn outsiders to turn back now.  The passages of\n"+
"this crypt are trapped and a curse has been proclaimed upon all violators\n"+
"of Xyphelod's resting place.\n");
        } else {
            write("The words are in a language foreign to you.\n");
            write("You do get the general impression of danger, however.\n");
        }
        return 1;
    }
}

look(str) {
    if (str =="at wall" || str =="at walls" || str=="at writings") {
        return read("wall");
    }
}

search(str) {
    if (!str) str = "room";
    if (str == "room") {
        write("You find nothing unusual.\n");
        say(call_other(this_player(),"query_name") + " searches the room.\n");
        return 1;
    }
}

