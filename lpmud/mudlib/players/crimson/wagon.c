#define sayi 1
#define sayii 2
#define sayiii 3
#define sayiv 4
#define sayv 5
#define sayvi 6
#define sayvii 7
#define sayviii 8
#define sayix 9
#define sayx 10
#define sayxi 11
#define sayxii 12
#define sayxiii 13
#define sayxiv 14
#define sayxv 15
#define sayxvi 16
#define sayxvii 17
#define sayxviii 18
#define sayxix 19
#define sayxx 20
#define sayxxi 21
string title;
string name;
string what;
string mwhat;
string whati;
string whatii;
string whatiii;
string whativ;
string wheni;
string ewhile;
string next;
string nexti;
string nextii;
string nextiii;
string nextiv;
string morei;
string moreii;
string moreiii;
string moreiv;
string smorei;
string smoreii;
string smoreiii;
string money;
int speak;
int this;
int notnow;
int nospeak;
int n;
int i;
int j;
int k;
int l;
int m;
int o;
int p;
int q;
int r;
int s;
int t;
int u;
int v;
int x;
int w;
int y;
int z;
int b;
int a;
int g;
object player;
object gypsy;
object crystal;
object scroll;

short() { return "Gyspy Wagon"; }
reset(arg) {
   set_light(1);
   extra_reset();
   nospeak = 0;
}

long() { 
    write("The wagon smells musty like an old attic.\n");
    write("There is only one window here and colored\n");
    write("beads decorate the room.  You can tell that incense\n");
    write("has been used in a feeble attempt to disguse the musty\n");
    write("air.  A faded sign is in the corner of the room.\n");
    write("The only obvious exit is south.\n");
    return 1;
}

init() {
   add_action("give"); add_verb("give");
   add_action("south"); add_verb("south");
   add_action("kill"); add_verb("kill");
   add_action("kill"); add_verb("shock");
   add_action("kill"); add_verb("missile");
   add_action("kill"); add_verb("fireball");
   add_action("kill"); add_verb("strike");
   add_action("kill"); add_verb("chill");
   add_action("look"); add_verb("look");
   add_action("read"); add_verb("read");
}

look(str) {
  if (str == "through window" || str == "out window" || str == "at window") {
  move_object(this_player(), "players/crimson/lowwagon");
  call_other(this_player(), "look", 0);
  move_object(this_player(), "players/crimson/wagon");
  return 1;
  } else
  if (str == "at sign") {
  write("There is a sign here.\n");
  write("Maybe you should read it. eh?\n");
  return 1;
  }
}

kill(str) {
  if (str == "gypsy" || str == "woman") {
  nospeak = 1;
  }
}

give(str) {
  
  name = call_other(this_player(), "query_name", 0);
  title = call_other(this_player(), "query_title", 0);
  player = this_player();
  money = call_other(this_player(), "query_money", 0);
  n = random(6);
  i = random(6);
  j = random(6);
  k = random(6);
  l = random(12);
  m = random(31) +1;
  o = random(6);
  p = random(3);
  q = random(6);
  r = random(6);
  s = random(6);
  t = random(6);
  u = random(6);
  w = random(6);
  v = random(6);
  x = random(6);
  y = random(6);
  z = random(6);
  b = random(6);
  a = random(6);
  g = random(15);
  wheni = m;
  if (n == 0) {
  what = "fight";
  } else
  if (n == 1) {
  what = "kiss";
  } else
  if (n == 2) {
  what = "kill";
  } else
  if (n == 3) {
  what = "jump";
  } else
  if (n == 4) {
  what = "race";
  } else
  if (n == 5) {
  what = "escape";
  }
  if (i == 0) {
  mwhat = "huge";
  } else
  if (i == 1) {
  mwhat = "tiny";
  } else
  if (i == 2) {
  mwhat = "green";
  } else
  if (i == 3) {
  mwhat = "chartreuse";
  } else
  if (i == 4) {
  mwhat = "slimy";
  } else
  if (i == 5) {
  mwhat = "shaggy";
  }
  if (j == 0) {
  whati = "rabid";
  } else
  if (j == 1) {
  whati = "leprous";
  } else
  if (j == 2) {
  whati = "mad";
  } else
  if (j == 3) {
  whati = "teething";
  } else
  if (j == 4) {
  whati = "salivating";
  } else
  if (j == 5) {
  whati = "jaundiced";
  }
  if (l == 0) {
  whatiii = "January";
  } else
  if (l == 1) {
  whatiii = "Febuary";
  } else
  if (l == 2) {
  whatiii = "March";
  } else
  if (l == 3) {
  whatiii = "April";
  } else
  if (l == 4) {
  whatiii = "May";
  } else
  if (l == 5) {
  whatiii = "June";
  } else 
  if (l == 6) {
  whatiii = "July";
  } else
  if (l == 7) {
  whatiii = "August";
  } else
  if (l == 8) {
  whatiii = "September";
  } else
  if (l == 9) {
  whatiii = "October";
  } else
  if (l == 10) {
  whatiii = "November";
  } else
  if (l == 11) {
  whatiii = "December";
  }
  if (k == 0) {
  whatii = "kangaroo";
  } else
  if (k == 1) {
  whatii = "elephant";
  } else
  if (k == 2) {
  whatii = "sloth";
  } else
  if (k == 3) {
  whatii = "martian";
  } else
  if (k == 4) {
  whatii = "hila monster";
  } else
  if (k == 5) {
  whatii = "european swallow";
  }
  if (o == 0) {
  whativ = "rain";
  } else
  if (o == 1) {
  whativ = "hail";
  } else
  if (o == 2) {
  whativ = "snow";
  } else
  if (o == 3) {
  whativ = "sand";
  } else
  if (o == 4) {
  whativ = "wind";
  } else
  if (o == 5) {
  whativ = "thunder";
  }
  if (q == 0) {
  next = "wipe";
  } else
  if (q == 1) {
  next = "kick";
  } else
  if (q == 2) {
   next = "bite";
  } else
  if (q == 3) {
  next = "slurp";
  } else
  if (q == 4) {
  next = "rub";
  } else
  if (q == 5) {
  next = "kiss";
  }
  if (p == 0) {
  ewhile = "after";
  } else
  if (p == 1) {
  ewhile = "before";
  } else
  if (p == 2) {
  ewhile = "while";
  }
  if (r == 0) {
  nexti = "sticky";
  } else
  if (r == 1) {
  nexti = "drooling";
  } else
  if (r == 2) {
  nexti = "sneezing";
  } else
  if (r == 3) {
  nexti = "laughing";
  } else
  if (r == 4) {
  nexti = "twittering";
  } else
  if (r == 5) {
  nexti = "babbling";
  }
  if (s == 0) {
  nextii = "grey";
  } else
  if (s == 1) {
  nextii = "white";
  } else
  if (s == 2) {
  nextii = "slinky";
  } else
  if (s == 3) {
  nextii = "acrobatic";
  } else
  if (s == 4) {
  nextii = "polka-dotted";
  } else
  if (s == 5) {
  nextii = "lizard liped";
  }
  if (t == 0) {
  nextiii = "mule";
  } else
  if (t == 1) {
  nextiii = "weasel";
  } else
  if (t == 2) {
  nextiii = "demon";
  } else
  if (t == 3) {
  nextiii = "toad";
  } else
  if (t == 4) {
  nextiii = "osritch";
  } else
  if (t == 5) {
  nextiii = "dog";
  }
  if (u == 0) {
  nextiv = "behind";
  } else
  if (u == 1) {
  nextiv = "lips";
  } else
  if (u == 2) {
   nextiv = "snout";
  } else
  if (u == 3) {
  nextiv = "stomach";
  } else
  if (u == 4) {
  nextiv = "ear";
  } else
  if (u == 5) {
  nextiv = "toe";
  }
  if (v == 0) {
  morei = "tall";
  } else
  if (v == 1) {
  morei = "short";
  } else
  if (v == 2) {
  morei = "lankey";
  } else
  if (v == 3) {
  morei = "fat";
  } else
  if (v == 4) {
   morei = "muscular";
  } else
  if (v == 5) {
  morei = "scrawny";
  }
  if (w == 0) {
   moreii = "pale";
   } else
   if (w == 1) {
   moreii = "dark";
   } else
   if (w == 2) {
   moreii = "grey";
   } else
   if (w == 3) {
   moreii = "red headed";
   } else
   if (w == 4) {
   moreii = "blonde";
   } else
   if (w == 5) {
   moreii = "brunette";
   }
   if (y == 0) {
   moreiv = "Mordred";
   } else
   if (y == 1) {
   moreiv = "Tegra";
   } else
   if (y == 2) {
   moreiv = "Death";
   } else
    if (y == 3) {
    moreiv = "Dogfood";
   } else
   if (y == 4) {
   moreiv = "Stealth";
   } else
   if (y == 5) {
   moreiv = "Fanwar";
   }
   if (b == 0) {
   smorei = "tease";
   } else
   if (b == 1) {
   smorei = "lick";
   } else
   if (b == 2) {
   smorei = "tweek";
   } else
   if (b == 3) {
   smorei = "punt";
   } else
   if (b == 4) {
   smorei = "pulverize";
   } else
   if (b == 5) {
   smorei = "fondle";
   }
   if (z == 0) {
   smoreii = "heavy";
   } else
   if (z == 1) {
   smoreii = "huge";
   } else
   if (z == 2) {
   smoreii = "tiny";
   } else
   if (z == 3) {
   smoreii = "plump";
   } else
   if (z == 4) {
   smoreii = "blunt";
   } else
   if (z == 5) {
    smoreii = "small";
   }
   if (a == 0) {
   smoreiii = "watermelon";
   } else
   if (a == 1) {
   smoreiii = "rose";
   } else
   if (a == 2) {
   smoreiii = "pickle";
   } else
   if (a == 3) {
   smoreiii = "smore";
   } else
   if (a == 4) {
   smoreiii = "gigilo";
   } else
   if (a == 5) {
   smoreiii = "cross-eyed IRS man";
   }
   if (x == 0) {
   moreiii = "man";
   } else
   if (x == 1) {
   moreiii = "woman";
   } else
   if (x == 2) {
   moreiii = "child";
   } else
   if (x == 3) {
   moreiii = "boy";
   } else
   if (x == 4) {
   moreiii = "girl";
   } else
   if (x == 5) {
   moreiii = "thing";
   }
   if (!present("gypsy") && str == "100 coins to gypsy" || !present("gypsy") && str == "100 coins to gypsy woman") {
   write("There is no one here to tell your fortune.\n");
   return 1;
   } else
   if (str == "100 coins to gypsy" && (notnow == 1) || str == "100 coins to gypsy woman" && (notnow == 1)) {
   tell_object(this_player(), "The gypsy tells you: You must wait for me to finish first.\n");
   return 1;
   } else
   if (str == "100 coins to gypsy" && (money<100) || str == "100 coins to gypsy woman" && (money<100)) {
   write("The gypsy tells you: You don't have 100 coins!\n");
   return 1;
   } else
  if (str == "100 coins to gypsy" || str == "100 coins to gypsy woman") {
  call_other(this_player(), "add_money", -(100));
  tell_object(this_player(), "The gypsy smiles happily and pockets the money.\n");
  notnow = 1;
  set_heart_beat(1);
  return 1;
  } else
  if (!str) {
  return 0;
  } else
  if (str) {
  return 0;
  }
}

heart_beat() {
  speak += 1;
  if (speak == sayi && nospeak != 1) {
  say("The gypsy takes in a deep breath.\n");
  } else
  if (speak == sayii && nospeak != 1) {
  say("The gypsy says: The crystal tells me that you "+name+" "+title+"\n");
  } else
  if (speak == sayiii && nospeak != 1) {
  say("The gypsy takes a swig of whiskey.\n");
  } else
  if (speak == sayiv && nospeak != 1) {
  say("The gypsy takes another deep breath.\n");
  } else
  if (speak == sayv && nospeak != 1) {
  say("The gypsy says: will "+what+" a "+mwhat+", "+whati+", "+whatii+" \n");
  } else
  if (speak == sayvi && nospeak != 1) {
  say("The gypsy hiccups.\n");
  } else
  if (speak == sayvii && nospeak != 1) {
  say("The gypsy says: on the "+wheni+" day of "+whatiii+" in a "+whativ+" storm "+ewhile+"\n");
  } else
  if (speak == sayviii && nospeak != 1) {
  say("The gypsy burps.\n");
  } else
  if (speak == sayix && nospeak != 1) {
  say("The gypsy says: you "+next+" a "+nexti+", "+nextii+", "+nextiii+"'s "+nextiv+".\n");
  } else
  if (speak == sayx && nospeak != 1) {
  say("The gypsy leans forward and gazes into the crystal more intently.\n");
  } else
  if (speak == sayxi && nospeak != 1) {
  say("The gypsy looks drunk.\n");
  } else
  if (speak == sayxii && nospeak != 1) {
  say("The gypsy's eyes look more concerned now.\n");
  } else
  if (speak == sayxiii && nospeak != 1) {
  say("The gypsy says: But beware, "+name+" "+title+".\n");
  } else
  if (speak == sayxiv && nospeak != 1) {
  say("The gypsy says: For on the "+wheni+" day of "+whatiii+" you will\n");
  } else
  if (speak == sayxv && nospeak != 1) {
  say("The gypsy says: meet a "+morei+", "+moreii+", "+moreiii+" named, "+moreiv+".\n");
  } else
  if (speak == sayxvi && nospeak != 1) {
  say ("The gypsy says: and the "+moreiii+" will "+smorei+" you with a "+smoreii+" "+smoreiii+".\n");
  } else
  if (speak == sayxvii && (g == 0) && nospeak != 1) {
  say("The gypsy says: Here, "+name+" "+title+" you will need this potion.\n");
  scroll = clone_object("players/crimson/exp");
  move_object(scroll, this_object());
  } else
  if (speak == sayxviii && (g == 0) && nospeak != 1) {
  say("The gypsy says: Use it to fight "+moreiv+" and you will vanquish your foe.\n");
  } else
  if (speak == sayxix && (o == 0)) {
  say ("It suddenly begins to rain.\n");
  } else
  if  (speak == sayxix && (o == 1)) {
  say ("It suddenly begins to hail.\n");
  } else
  if  (speak == sayxix && (o == 2)) {
  say ("It suddenly begins to snow.\n");
  } else
  if  (speak == sayxix && (o == 3)) {
  say ("Sand blows in the window of the wagon.\n");
  } else
  if  (speak == sayxix && (o == 4)) {
  say ("The wagon begins to rock from the howling wind.\n");
  } else
  if  (speak == sayxix && (o == 5)) {
  say ("A clap of thunder shakes the wagon.\n");
  } else
  if (speak == sayxxi) {
  say ("The gypsy slumps back down in her chair.\n");
  speak = 0;
  notnow = 0;
  this = 1;
  nospeak = 0;
  set_heart_beat(0);
  }
}

extra_reset() {
  if (!present("gypsy")) {
  gypsy = clone_object("obj/monster.talk");
  call_other(gypsy, "set_name", "gypsy");
  call_other(gypsy, "set_short", "A gypsy woman");
  call_other(gypsy, "set_alias", "woman");
  call_other(gypsy, "set_level", 7);
  call_other(gypsy, "set_eps", 80000);
  call_other(gypsy, "set_al", 500);
  call_other(gypsy, "set_long", "She appears to be a bit tipsy.  Bright red\n" +
  "lipstick covers her mouth and black wooden beads\n"+
  "are strung around her neck.  She smells of cheap\n"+
  "perfume and looks as though she might have been\n"+
  "much more attractive in her younger years.\n");
  call_other(gypsy, "set_chat_chance", 10);
  call_other(gypsy, "set_a_chat_chance", 30);
  call_other(gypsy, "load_chat", "The gypsy burps.\n");
  call_other(gypsy, "load_chat", "The gyspy hiccups.\n");
  call_other(gypsy, "load_chat", "The gyspy smiles happily.\n");
  call_other(gypsy, "load_a_chat", "The gypsy says: How dare you! I will summon up black magic!\n");
  call_other(gypsy, "load_a_chat", "The gypsy says: Even in my death you will feel my wrath!\n");
  call_other(gypsy, "load_a_chat", "The gypsy says: I will haunt you for ages to come!\n");
  call_other(gypsy, "load_a_chat", "The gypsy says: Guards! Help! Police!\n");
  move_object(gypsy, this_object());
  crystal = clone_object("players/crimson/.crystal");
  move_object(crystal, gypsy);
 }
}

south(str) {
object thingy;
object weapon;
  if(notnow == 1 && nospeak != 1) {
  write("The gypsy tells you: You must wait until I'm done.\n");
  return 1;
  } else
  if (this == 1) {
  call_other(this_player(), "move_player", "south#players/crimson/wag");
  thingy = clone_object("obj/monster");
  if (y == 0) {
  call_other(thingy, "set_name", "mordred");
  } else
  if (y == 1) {
  call_other(thingy, "set_name", "tegra");
  } else
  if (y == 2) {
  call_other(thingy, "set_name", "death");
  } else 
  if (y == 3) {
  call_other(thingy, "set_name", "dogfood");
  } else
  if (y == 4) {
  call_other(thingy, "set_name", "stealth");
  } else
  if (y == 5) {
  call_other(thingy, "set_name", "fanwar");
  }
  call_other(thingy, "set_alias", "nightmare");
  call_other(thingy, "set_short", moreiv+" (Your nightmare come true)");
  call_other(thingy, "set_level", 5);
  call_other(thingy, "set_wc", 12);
  call_other(thingy, "set_ac", 3);
  call_other(thingy, "set_aggressive", 1);
  call_other(thingy, "set_eps", 100000);
  call_other(thingy, "set_long", moreiv+ " is a "+morei+", "+moreii+", "+moreiii+".\n");
  move_object(thingy, "players/crimson/lowwagon");
  weapon = clone_object("obj/weapon");
  if (a == 0) {
  call_other(weapon, "set_name", "watermelon");
  } else
  if (a == 1) {
  call_other(weapon, "set_name", "rose");
  } else
  if (a == 2) {
  call_other(weapon, "set_name", "pickle");
  } else
  if (a == 3) {
  call_other(weapon, "set_name", "smore");
  } else
  if (a == 4) {
  call_other(weapon, "set_name", "gigilo");
  } else
  if (a == 5) {
  call_other(weapon, "set_name", "IRS man");
  }
  call_other(weapon, "set_short", "A "+smoreii+" "+smoreiii+"");
  call_other(weapon, "set_class", 9);
  call_other(weapon, "set_long", "A "+smoreii+" "+smoreiii+".\n");
  if (a == 5) {
  call_other(weapon, "set_alias", "man");
  }
  call_other(weapon, "set_weight", 2);
  call_other(weapon, "set_value", 50);
  move_object(weapon, thingy);
  if (a == 0) {
  call_other(weapon, "wield", "watermelon");
  } else
  if (a == 1) {
  call_other(weapon, "wield", "rose");
  } else
  if (a == 2) {
  call_other(weapon, "wield", "pickle");
  } else
  if (a == 3) {
  call_other(weapon, "wield", "smore");
  } else
  if (a == 4) {
  call_other(weapon, "wield", "gigilo");
  } else 
  if (a == 5) {
  call_other(weapon, "wield", "IRS man");
  }
  this = 0;
  return 1;
  } else
  call_other(this_player(), "move_player", "south#players/crimson/wag");
  return 1;
}

read(str) {
  if (str == "sign") {
  write("                ****************************************\n");
  write("                *                                      *\n");
  write("                *               -FORTUNES-             *\n");
  write("                *                                      *\n");
  write("                *               100 coins              *\n");
  write("                *                                      *\n");
  write("                *     Anthing more or less will be     *\n");
  write("                *     considered a donation.           *\n");
  write("                *                                      *\n");
  write("                ****************************************\n");
  return 1;
  } else
  if (str) {
  return 0;
  } else
  if (!str) {
  return 0;
  }
}
