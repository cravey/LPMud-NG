string challengera;
string challengeda;
string levelr;
string leveld;

short() { return "THUNDER ENTRY"; }

reset(arg) {
   set_light(1);
}

long() {
   write("You are standing before THUNDERDOME.\n");
   write("There is a bulletin board here.\n");
   if (call_other("players/crimson/thunderdome", "query_dome") == 1) {
   write("The dome's gates lie to the south and are open.\n");
   } else
   if (call_other("players/crimson/thunderdome", "query_dome") == 0) {
   write("The dome's gates lie to the south and are closed.\n");
   }
}

init() {
   add_action("south"); add_verb("south");
   add_action("read"); add_verb("read");
   add_action("read"); add_verb("look");
   add_action("open"); add_verb("open");
   add_action("get"); add_verb("get");
   add_action("north"); add_verb("north");
   add_action("get"); add_verb("take");
}

set_vars() {
    challengera = call_other("players/crimson/thunderdome", "query_challenger");
    challengeda = call_other("players/crimson/thunderdome", "query_challenged");
    levelr = call_other("players/crimson/thunderdome", "query_challev");
    leveld = call_other("players/crimson/thunderdome", "query_chall");
}

read(str) {
    set_vars();
   if (str == "at board" || str == "at bulletin board" || str == "board" || str == "bulletin board") {
    write("            ********************************************\n");
    write("\n");
    write("                     *THUNDERDOME ANOUNCEMENTS*\n");
    write("\n");
    write("                     Last Challenger: "+challengera+"\n");
    write("\n");
    write("                     Last Challenged: "+challengeda+"\n");
    write("\n");
    if(call_other("players/crimson/thunderdome", "query_chall") == 1)
    write("                     *THIS CHALLENGE IN PROCESS*\n");
    write("                     Notice: Please do not bring\n");
    write("                     pets into the THUNDERDOME.\n");
    write("                     Signed,\n");
    write("                          The management. \n");
    write("            ********************************************\n");
    say(call_other(this_player(), "query_name")+ " reads the bulletin board.\n");
    return 1;
    } else
    if (!str) {
    return 0;
    } else
    if (str) {
    return 0;
    }
}

open(str) {
    if (str == "gates" && call_other("players/crimson/thunderdome", "query_dome") == 1 || str == "gate" && call_other("players/crimson/thunderdome", "query_dome") == 1) {
    write("The gates are already open.\n");
    return 1;
    } else
    if (str == "gates" && call_other("players/crimson/thunderdome", "query_chall") == 1 || str == "gate" && call_other("players/crimson/thunderdome", "query_chall") == 1) {
    write("You may not open the gates while a battle is in process.\n");
    return 1;
    } else
    write("You open the dome's gates.\n");
    say(call_other(this_player(), "query_name")+" opens the dome gates.\n");
    call_other("players/crimson/thunderdome", "set_dome", 1);
    return 1;
}

south(str) {
    if (call_other("players/crimson/thunderdome", "query_dome") == 0) {
    write("The gates are closed.\n");
    return 1;
    } else
    call_other(this_player(), "move_player", "south#players/crimson/thunderdome");
    return 1;
}

get(str) {
   if (str == "board" || str == "bulletin board") {
   write("The bulletin board is secured to the dome's exterior.\n");
   return 1;
   } else
   if (str) {
   return 0;
   } else
   if (!str) {
   return 1;
   }
}

north(str) {
   call_other(this_player(), "move_player", "north#room/plane4");
   return 1;
}
