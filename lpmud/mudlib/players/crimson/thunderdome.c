#define corp 1
string challenger;
string challenged;
string title;
string where;
string nochal;
string level;
string dome;
string chall;
string mymon;
string heart;
string challengera;
string challengeda;
object challengr;
object challengd;
object challengra;
object challengda;
object ob;
object corpse;
object weapon;
object money;
int i;
int j;

short() { return "THUNDERDOME"; }

long() {
   write("You are in THUNDERDOME.\n");
   write("The place of challenges and battle.\n");
   if (call_other("players/crimson/thunderdome", "query_dome") == 0) {
   write("The dome's north gates are closed.\n");
   } else
   if (call_other("players/crimson/thunderdome", "query_dome") == 1) {
   write("The dome's north gates are open.\n");
   return 1;
   }
}

reset(arg) {
   set_light(1);
   set_heart_beat(1);
   dome = 0;
   i = 1;
   call_other("players/crimson/thunderdome", "set_chall", 0);
}

open(str) {
  if (str == "gates" && (call_other("players/crimson/thunderdome", "query_chall") == 1)) {
  write("You may not open the gates while a challenge is in process.\n");
  return 1;
  } else
  if (str == "gate" && (call_other("players/crimson/thunderdome", "query_chall") == 1)) {
  write("You may not open the gates while a challenge is in process.\n");
  return 1;
  } else
  write("You open the dome's gates.\n");
  say(call_other(this_player(), "query_name") + " opens the dome's gates.\n");
  call_other("players/crimson/thunderdome", "set_dome", 1);
  return 1;
}

set_dome(str) {
    dome = str;
}

set_chall(str) {
    chall = str;
}

init() {
   add_action("challenge"); add_verb("challenge");
   add_action("quit"); add_verb("quit");
   add_action("north"); add_verb("north");
   add_action("spell"); add_verb("fireball");
   add_action("kill"); add_verb("kill");
   add_action("spell"); add_verb("strike");
   add_action("spell"); add_verb("missile");
   add_action("spell"); add_verb("shock");
   add_action("spell"); add_verb("chill");
   add_action("drop"); add_verb("drop");
   add_action("give"); add_verb("give");
   add_action("open"); add_verb("open");
   if (!call_other(this_player(), "is_player"))
   call_other(this_player(), "move_player", "north#players/crimson/thunderentry");
}

challenge(str) {
   if (!str) { return 1; }
   challengd = find_living(str);
   if (!challengd) {
   write("I can not find that player.\n");
   return 1;
   }
   if (!living(challengd)) {
   write("That is not even alive!\n");
   return 1;
   } 
   if (!call_other(challengd, "is_player")) {
   write("That is not a player.\n");
   return 1;
   }
   if (challengd == this_player()) {
   challengr = this_player();
   tell_object(challengr, "You start to attack yourself then decide against it.\n");
   return 1;
   }
   if (call_other(this_player(), "query_level")<3) {
   tell_object(this_player(), "You do not have the reputation to challenge yet.\n");
   return 1;
   }
   if (call_other(this_player(), "query_level")>19) {
   tell_object(this_player(), "Battles to the death are for mortals.\n");
   return 1;
   }
   nochal = call_other(challengd, "query_level");
   level = call_other(this_player(), "query_level");
   if (nochal<3) {
   write("They are not high enough to be challenged yet.\n");
   return 1;
   }
   if (nochal>19) {
   write("You can't challenge a wizard!\n");
   return 1;
   }
   if (call_other("players/crimson/thunderdome", "query_chall") == 1) {
   write("Only one challenge may be made at a time.\n");
   return 1;
   }
   randweapon();
   mymon = call_other(this_player(), "query_money");
   challenger = call_other(this_player(), "query_name", 0);
   challenged = capitalize(str);
   challengr = this_player();
   title = call_other(challengd, "query_title", 0);
   where = environment(challengd);
   tell_object(challengr, "You challenge "+challenged+" to a fight in THUNDERDOME.\n");
   tell_object(challengd, challenger+ " has challenged you to a fight in THUNDERDOME.\n");
   tell_object(challengd, "You have approximately one minute to 'accept challenge' or 'decline challenge'.\n");
   shout(challenger+" has challenged "+challenged+" to a battle in THUNDERDOME.\n");
   move_object(this_player(), "players/crimson/thunderwest");
   call_other(this_player(), "drop_all");
   call_other(this_player(), "drop_all");
   money = clone_object("obj/money");
   call_other(money, "set_money", mymon);
   move_object(money, "players/crimson/thunderwest");
   call_other(this_player(), "add_money", -(mymon));
   move_object(this_player(), "players/crimson/thunderdome");
   call_other(this_player(), "set_ac", 5);
   move_object(weapon, this_player());
   call_other(weapon, "wield", "weapon");
   call_other(challengd, "set_title", "THE COWARD");
   tell_object(challengd, "You are magically transfered somewhere.\n");
   move_object(challengd, "players/crimson/thunderhall");
   call_other("players/crimson/thunderdome", "set_chall", 1);
   call_other("players/crimson/thunderdome", "set_heart", 1);
   return 1;
}

query_challenger() { return challenger; }

query_challenged() { return challenged; }

query_challtitle() { return title; }

query_where() { return where; }

query_challengr() { return challengr; }

query_challengd() { return challengd; }

query_dome() { return dome; }

query_chall() { return chall; }

query_challev() { return level; }

query_nochall() { return nochal; }

test_if_any_here() {
   ob = first_inventory(this_object());
   while(ob) {
      if(ob != this_object() && living(ob) && !call_other(ob, "query_npc"))
      return 1;
      ob = next_inventory(ob);
   }
   return 0;
}

heart_beat() {
    i = 1;
    corpse = present("corpse");
    challengra = call_other("players/crimson/thunderdome", "query_challengr");
    challengda = call_other("players/crimson/thunderdome", "query_challengd");
    challengeda = call_other("players/crimson/thunderdome", "query_challenged");
    challengera = call_other("players/crimson/thunderdome", "query_challenger");
    if (test_if_any_here() && present("corpse") && (i == corp)) {
    tell_room(environment(ob), "The battle is decided.\n");
    tell_room(environment(ob), "There is a grating sound as the dome gates open.\n");
    move_object(corpse, "players/crimson/storeroom");
    tell_object(challengra, "You are magically transfered somewhere.\n");
    tell_object(challengda, "You are magically transfered somewhere.\n");
    move_object(challengda, "players/crimson/storeroom");
    call_other(challengda, "stop_wielding");
    call_other(challengda, "drop_all");
    call_other(challengda, "set_wc", 3);
    call_other(challengda, "add_weight", 1);
    call_other(challengda, "set_ac", 0);
    move_object(challengda, "players/crimson/thundereast");
    move_object(challengra, "players/crimson/storeroom");
    call_other(challengra, "stop_wielding");
    call_other(challengra, "drop_all");
    call_other(challengra, "set_wc", 3);
    call_other(challengra, "add_weight", 1);
    call_other(challengra, "set_ac", 0);
    move_object(challengra, "players/crimson/thunderwest");
    say(challengeda+" disappears in a puff of smoke.\n");
    say(challengera+" disappears in a puff of smoke.\n");
    call_other("players/crimson/thunderdome", "set_dome", 1);
    call_other("players/crimson/thunderdome", "set_chall", 0);
    log_file("CHALLENGES", challengera+" challenged "+challengeda+".\n");
    }
}

quit() {
    write("There are no quiters in THUNDERDOME.\n");
    return 1;
}

north(str) {
    if (call_other("players/crimson/thunderdome", "query_dome") == 0) {
    write("The dome gates are closed and bolted.\n");
    return 1;
    } else
    if (call_other(this_player(), "query_name") == challenger && (call_other("players/crimson/thunderdome", "query_chall") == 1)) {
    write("You can not leave after you have made a challenge!\n");
    return 1;
    } else
    if (call_other("players/crimson/thunderdome", "query_dome") == 1) {
    call_other(this_player(), "move_player", "north#players/crimson/thunderentry");
    return 1;
    }
}

kill(str) {
     if(str) {
     return 1;
     }
}

spell(str) {
    if (str) {
    write("Battle magic is not allowed in THUNDERDOME.\n");
    return 1;
    } else
    if (!str) {
    write("Battle magic is not allowed in THUNDERDOME.\n");
    return 1;
    }
}

drop(str) {
    if (str) {
    write("Please don't, we like to keep our THUNDERDOME clean. :-)\n");
    return 1;
    } else
    if (!str) {
    return 1;
    }
}

give(str) {
   if (str) {
   write("Your possesions remain your own in THUNDERDOME.\n");
   return 1;
   }
}

randweapon() {
   if (level<6) {
   j = 0;
   } else
   if ((level>6 || level == 6) && (level<11)) {
   j = random(2);
   } else
   if ((level>11 || level == 11) && (level<16)) {
   j = random(3);
   } else
   if (level == 16 || level>16) {
   j = random(4);
   }
   if (j == 0) {
   weapon = clone_object("obj/weapon");
   call_other(weapon, "set_name", "weapon");
   call_other(weapon, "set_short", "A tiny whistle");
   call_other(weapon, "set_class", 20);
   call_other(weapon, "set_weight", 1);
   call_other(weapon, "set_value", 10000);
   } else
   if (j == 1) {
   weapon = clone_object("obj/weapon");
   call_other(weapon, "set_name", "weapon");
   call_other(weapon, "set_short", "A chainsaw");
   call_other(weapon, "set_class", 15);
   call_other(weapon, "set_weight", 1);
   call_other(weapon, "set_value", 5);
   } else
   if (j == 2) {
   weapon = clone_object("obj/weapon");
   call_other(weapon, "set_name", "weapon");
   call_other(weapon, "set_short", "A HUGE axe");
   call_other(weapon, "set_class", 12);
   call_other(weapon, "set_weight", 1);
   call_other(weapon, "set_value", 100);
   } else
   if (j == 3) {
   weapon = clone_object("obj/weapon");
   call_other(weapon, "set_name", "weapon");
   call_other(weapon, "set_short", "A spear");
   call_other(weapon, "set_class", 9);
   call_other(weapon, "set_weight", 1);
   call_other(weapon, "set_value", 100);
   }
}

set_heart(str) {
   heart = str;
}

query_heart() { return heart; }
