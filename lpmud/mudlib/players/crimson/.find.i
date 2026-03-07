# 1 "players/crimson/.find.c" 

object collared;
object me;
string collarname;
string myname;
int i;
int j;
int gived;
int who;

short() {
      if (who == 1) {
      return 0;
      } else
      return "A find familiar spell. (memorized)";
}

long() { 
    write("A find familiar spell. (memorized).\n");
    return 1;
}

id(str) {
    return str == "spell" || str == "find familiar spell";
}

cast(str) {
    
    if (!str) { return 1; }
    me = this_player();
    if (str == "find familiar spell" || str == "spell") {
    myname = call_other(me, "query_name");
    write("You cast find familiar.\n");
    say(myname+" casts a spell.\n");
    collared = clone_object("obj/monster.talk");
    call_other(collared, "set_name", "cat");
    call_other(collared, "set_alias", "black cat");
    call_other(collared, "set_short", "A black cat");
    call_other(collared, "set_level", 1);
    call_other(collared, "set_hps", 30);
    call_other(collared, "set_wc", 5);
    call_other(collared, "set_ac", 1);
    call_other(collared, "set_long", "The black cat eyes you wearliy.\n");
    call_other(collared, "set_chat_chance", 15);
    call_other(collared, "set_a_chat_chance", 40);
    call_other(collared, "load_chat", "The black cat meows loudly.\n");
    call_other(collared, "load_chat", "The black cat purrs contentedly.\n");
    call_other(collared, "load_chat", "The black cat sits down to lick it's paws.\n");
    call_other(collared, "load_a_chat", "The black cat screaches loudly.\n");
    call_other(collared, "load_a_chat", "The black cat HISSES.\n");
    call_other(collared, "load_a_chat", "The black cat growls deeply.\n");
    call_other(collared, "set_can_kill", 1);
    move_object(collared, environment(me));
    set_heart_beat(1);
    who = 1;
    return 1;
   }
}

release(str) {
    if (who != 1) {
    write("What?\n");
    return 1;
    } else
    if (who == 1) {
    write("You release your familiar.\n");
    collared = 0;
    who = 0;
    set_heart_beat(0);
    return 1;
    }
}

init() {
    add_action("cast"); add_verb("cast");
    add_action("no_find"); add_verb("origin");
    add_action("drop_object"); add_verb("drop");
    add_action("drop_object"); add_verb("put");
    add_action("release"); add_verb("release");
    add_action("sic"); add_verb("sic");
}

no_find(str) {
    if (str == "spell" || str == "find familiar spell") {
    write("What?\n");
    return 1;
    }
}

drop() {
    gived += 1;
    if (gived == 2)
    write("You forgot a find familiar spell.\n");
    say(call_other(this_player(), "query_name")+ " forgets a find familiar spell.\n");
    destruct(this_object());
     return 1;
}

drop_object(str) {
    if (str == "all") {
      drop_object("find familiar spell");
      return 1;
      }
      if (!str || !id(str)) {
      return 0;
      write("You forgot a find familiar spell.\n");
      say(call_other(this_player(), "query_name") +" forgot a find familiar spell.\n");
      destruct(this_object());
      return 1;
      }
}

get() { return 1; }

heart_beat() {
   i = 1;
   if((environment(collared) != environment(me)) && (i == 1)) {
   tell_room(environment(collared), "The black cat bounds away after "+myname+".\n");
   tell_room(environment(me), "Black cat arrives.\n");
   move_object(collared, environment(me));
   }
}

sic(str) {
object sicked;
string sicname;
   sicked = present(lower_case(str), environment(me));
   sicname = capitalize(str);
   if (sicked == collared) {
   who = 0;
   collared = 0;
   set_heart_beat(0);
   write("Your familiar breaks free from your power!\n");
   return 1;
   }
   if (!sicked) {
   write("Your familiar looks at you like your mad.\n");
   return 1;
   }
   if (!living(sicked)) {
   write("That's not alive!\n");
   return 1;
   }
   say(myname+" points at "+sicname+" and yells: SIC UM!\n");
   call_other(collared, "attack_object", sicked);
   return 1;
}
