#define CHICKEN 30
string challengera;
string challengeda;
string titlea;
string wherea;
string mymon;
string level;
string lev;
object challengra;
object challengda;
object weapond;
object money;
int j;
int n;

short() { return "THUNDERHALL"; }

long() {
   write("You are in THUNDERHALL.\n");
   write("Here is the place to accept or decline challenges.\n");
   write("There is a HUGE sign here.\n");
   write("If you are new to THUNDERDOME you should read it.\n");
}

reset(arg) {
   set_light(1);
   set_heart_beat(1);
}

init() {
   add_action("accept"); add_verb("accept");
   add_action("decline"); add_verb("decline");
   add_action("quit"); add_verb("quit");
   add_action("read"); add_verb("read");
   add_action("read"); add_verb("look");
}

read(str) {
  set_vars();
  level = call_other("players/crimson/thunderdome", "query_challev", 0);
  if (str == "at sign" || str == "sign") {
  write("                    *************************************\n");
  write("                    *                                   *\n");
  write("                    *       You are in THUNDERHALL      *\n");
  write("                    *       This is where you may       *\n");
  write("                    *       accept or decline           *\n");
  write("                    *       a challenge. WARNING        *\n");
  write("                    *       should you choose to        *\n");
  write("                    *       accept a challenge, you     *\n");
  write("                    *       will begin fighting         *\n");
  write("                    *       immediately.  WARNING2      *\n");
  write("                    *       should you choose to        *\n");
  write("                    *       decline, your title         *\n");
  write("                    *       will remain THE COWARD      *\n");
  write("                    *                                   *\n");
  write("                    *   'accept challenge' to accept    *\n");
  write("                    *  'decline challenge' to decline   *\n");
  write("                    *                                   *\n");
  write("                    *************************************\n");
  write("                    The last challenge was made by "+challengera+".\n");
  write("                              "+challengera+" is level "+level+".\n");
  return 1;
  } else
  if (!str) {
  return 0;
  } else
  if (str) {
  return 0;
  }
}

set_vars() {
   challengera = call_other("players/crimson/thunderdome", "query_challenger");
   challengeda = call_other("players/crimson/thunderdome", "query_challenged");
   titlea = call_other("players/crimson/thunderdome", "query_challtitle");
   wherea = call_other("players/crimson/thunderdome", "query_where");
   challengra = call_other("players/crimson/thunderdome", "query_challengr");
   challengda = call_other("players/crimson/thunderdome", "query_challengd");
   lev = call_other("players/crimson/thunderdome", "query_nochall");
}

accept(str) {
   set_vars();
   if (str == "challenge" && (call_other(this_player(), "query_name") != challengeda)) {
   write("You can not accept "+challengeda+"'s challenge!\n");
   return 1;
   } else
   if (str == "challenge" && challengera == challengeda) {
   return 0;
   } else
   randweapon();
   mymon = call_other(this_player(), "query_money");
   if (str == "challenge") {
   tell_object(challengda, "You accept "+challengera+"'s challenge.\n");
   tell_object(challengda, "THUNDERDOME!\n");
   tell_object(challengra, challengeda+" has accepted your challenge!\n");
   tell_room(environment(challengra), "THUNDERDOME!\n");
   call_other(challengda, "set_title", titlea);
   call_other("players/crimson/thunderdome", "set_dome", 0);
   tell_room(environment(challengra), "There is a grating sound as the gates close.\n");
   tell_room(environment(challengra), challengeda+ " arrives in a puff of smoke.\n");
   move_object(challengda, "players/crimson/thundereast");
   call_other(this_player(), "heal_self", 200);
   call_other(this_player(), "drop_all");
   call_other(this_player(), "drop_all");
   money = clone_object("obj/money");
   call_other(money, "set_money", mymon);
   move_object(money, "players/crimson/thundereast");
   call_other(this_player(), "add_money", -(mymon));
   move_object(challengda, "players/crimson/thunderdome");
   move_object(weapond, challengda);
   call_other(weapond, "wield", "weapon");
   call_other(challengda, "set_ac", 5);
   tell_object(challengda, "There is a grating sound as the gates close.\n");
   call_other("players/crimson/thunderdome", "set_heart", 0);
   call_other(challengda, "attack_object", challengra);
   return 1;
   } else
   if (!str) {
   return 0;
   } else
   if (str) {
   return 0;
   }
}

decline(str) {
   set_vars();
   if (str == "challenge" && (call_other(this_player(), "query_name") != challengeda)) {
   write("You can not decline for "+challengeda+".\n");
   return 1;
   } else
   if (str == "challenge") {
   tell_object(challengda, "You decline "+challengera+"'s challenge and are returned to your former place.\n");
   tell_object(challengda, "Your title remains THE COWARD.\n");
   tell_room(environment(challengra), challengeda+" has declined to battle "+challengera+".\n");
   tell_object(challengra, challengeda+" wouldn't have been a match for you anyway.\n");
   tell_object(challengra, "You are magically transfered somewhere.\n");
   call_other("players/crimson/thunderdome", "set_heart", 0);
   move_object(challengra, "players/crimson/storeroom");
   call_other(challengra, "stop_wielding", 0);
   call_other(challengra, "drop_all", 0);
   call_other(challengra, "set_wc", 3);
   call_other(challengra, "add_weight", 1);
   call_other(challengra, "set_ac", 0);
   move_object(challengra, "players/crimson/thunderwest");
   call_other("players/crimson/thunderdome", "set_chall", 0);
   tell_room(wherea, challengeda+" arrives in a puff of smoke.\n");
   move_object(challengda, wherea);
   return 1;
   } else
   if (!str) {
   return 0;
   } else
   if (str) {
   return 0;
   }
}

realm() { return "NT"; }

quit() {
   write ("There are no quiters in THUNDERDOME.\n");
   return 1;
}

randweapon() {
    set_vars();
    if (lev<6) {
    j = 0;
    } else
    if ((lev>6 || lev == 6) && (lev<11)) {
    j = random(2);
    } else
    if ((lev>11 || lev == 11) && (lev<16)) {
    j = random(3);
    } else
    if (lev == 16 || lev>16) {
    j = random(4);
    }
   if (j == 0) {
   weapond = clone_object("obj/weapon");
   call_other(weapond, "set_name", "weapon");
   call_other(weapond, "set_short", "A tiny whistle");
   call_other(weapond, "set_class", 20);
   call_other(weapond, "set_weight", 1);
   call_other(weapond, "set_value", 10000);
   } else
   if (j == 1) {
   weapond = clone_object("obj/weapon");
   call_other(weapond, "set_name", "weapon");
   call_other(weapond, "set_short", "A chainsaw");
   call_other(weapond, "set_class", 15);
   call_other(weapond, "set_weight", 1);
   call_other(weapond, "set_value", 1000);
   } else
   if (j == 2) {
   weapond = clone_object("obj/weapon");
   call_other(weapond, "set_name", "weapon");
   call_other(weapond, "set_short", "A HUGE axe");
   call_other(weapond, "set_class", 12);
   call_other(weapond, "set_weight", 1);
   call_other(weapond, "set_value", 300);
   } else
   if (j == 3) {
   weapond = clone_object("obj/weapon");
   call_other(weapond, "set_name", "weapon");
   call_other(weapond, "set_short", "A spear");
   call_other(weapond, "set_class", 9);
   call_other(weapond, "set_weight", 1);
   call_other(weapond, "set_value", 300);
   }
}

heart_beat() {
   n += 1;
   set_vars();
   if ((n>CHICKEN) && call_other("players/crimson/thunderdome", "query_heart") == 1) {
   tell_object(challengra, "Well, "+challengeda+" is not responding.\n");
   tell_object(challengra, "They really are a coward, so I will let you go.\n");
   call_other("players/crimson/thunderdome", "set_chall", 0);
   tell_room(environment(challengra), "No THUNDERDOME today folks.\n");
   tell_object(challengra, "You are magically transfered somewhere.\n");
   move_object(challengra, "players/crimson/storeroom");
   call_other(challengra, "stop_wielding");
   call_other(challengra, "drop_all");
   move_object(challengra, "players/crimson/thunderwest");
   tell_object(challengda, "Time's up, you really are a chicken...\n");
   tell_object(challengda, "You're still a COWARD.\n");
   tell_object(challengda, "You are magically transfered somewhere.\n");
   move_object(challengda, wherea);
   call_other("players/crimson/thunderdome", "set_heart", 0);
   n = 0;
   } else
   if (n>CHICKEN) {
   n = 0;
   }
}
