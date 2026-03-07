#include "../../room/std.h"

#undef EXTRA_INIT
#define EXTRA_INIT\
       ex_init();

ONE_EXIT("players/ringer/en_valley", "south",
	   "You are in the local chapter of Alcoholics Anonymous.",
"You are in the local chapter of alcoholics anonymous.  A small coin-\n"
+"operated machine is imbedded in the wall in front of you.  There is\n"
+"a sign above the machine.\n",1)

ex_init() {
    if (call_other(this_player(),"query_intoxination")) {
        write("Get out and come back when you're sober!\n");
        write("You are flung out by a magical force!\n");
        move_object(this_player(),"players/ringer/en_valley");
        return 1;
    }
    add_action("read"); add_verb("read");
    add_action("look"); add_verb("look");
    add_action("put"); add_verb("put");
    add_action("buy"); add_verb("buy");
}


buy(str) {
    if (str=="badge" || str == "membership") {
        put("20 coins in machine");
        return 1;
    }
}


look(str) {
    if (str=="at sign") {
        read("sign");
        return 1;
    }
}

read(str) {
    if (str=="sign") {
    write("Welcome to the alcoholics anonymous discount membership center.\n");
    write("To become a member deposit 20 coins in the machine (all at once\n");
    write("please). You will be given your official membership badge.\n");
    write("Memberships last 5 minutes. Remember, we're trying to help you\n");
    write("stop drinking. Badges allow you to heal at the same rate as if\n");
  write("you were intoxicated.  Do not drop your badge at any time as this\n");
    write("will terminate your membership.\n");
    }
}

put(str) {
    object ob;
    int m;
    if (!str) {
        write("Put what?\n");
        return 1;
    }
    if (sscanf(str,"%d coins in machine",m)) {
        if (present("aa badge",this_player())) {
            write("You are already a member!\n");
            return 1;
        }
        if (call_other(this_player(),"query_money")<m) {
            write("You don't have that much!\n");
            return 1;
        }
        if (m>=20) {
          write("The machine whirrs.  A badge flies out and sticks itself.\n");
            write("to your shirt.\n");
            say(call_other(this_player(),"query_name") + " buys a badge.\n");
            ob = clone_object("players/ringer/badge");
            move_object(ob,this_player());
            call_other(this_player(),"add_money",-m);
            return 1;
        } else {
            write("That's not enough.\n");
            return 1;
        }
    }
}
