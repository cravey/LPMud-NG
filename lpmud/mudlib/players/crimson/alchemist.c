int moveme;
object ob;

reset(arg) {
    set_light(1);
    extra_reset();
    moveme = 1;
    set_heart_beat(1);
}

short() { return "Ye Old Alchemy Shoppe"; }

init() {
     add_action("buy"); add_verb("buy");
     add_action("east"); add_verb("east");
}

east() {
     call_other(this_player(), "move_player", "east#players/crimson/alentry");
     return 1;
}

test_if_any_here() {
    ob = first_inventory(this_object());
    while(ob) {
       if (ob != this_object() && living(ob)  && !call_other(ob, "query_npc"))
       return 1;
       ob = next_inventory(ob);
    }
    return 0;
}
heart_beat() {
     if(test_if_any_here()) {
     if (moveme == (call_other("players/crimson/alentry", "query_move"))) {
     tell_object(ob, "You are thrown free from a blast!\n");
     move_object(ob, "players/crimson/alentry");
     call_other(ob, "hit_player", 40);
     } else
     if (!test_if_any_here()) {
     set_heart_beat(0);
   }
 }
}

buy(str) {
object strength;
object serious;
object player;
object minor;
object heal;
object coffee;
object booze;
object polymorph;
string money;
string name;
     money = call_other(this_player(), "query_money", 0);
     player = this_player();
     name = call_other(this_player(), "query_name", 0);
     if (!present("fanwar")) {
     write("There is no one here to serve you.\n");
     return 1;
     } else
     if (!str) {
     write("Fanwar says: What is it you wish to purchase?\n");
     return 1;
     } else
     if (str == "potion of giant strength" && (money<2000) || str == "potion of strength" && (money< 2000)) {
     write("Fanwar says: I can't afford to give hand outs.\n");
     return 1;
     } else
     if (str == "potion of giant strength" || str == "potion of strength") {
     strength = clone_object("players/crimson/exp");
     move_object(strength, this_player());
     call_other(this_player(), "add_money", -(2000));
     write("Fanwar says: You have choosen wisely.  Do you wish to make another selection?\n");
     say(name+ " buys a brilliant red potion.\n");
     return 1;
     } else
     if (str == "potion of cure serious wounds" && (money<10000)) {
     write("Fanwar says: What? Are you crazy? No less than 10000! Firm!\n");
     return 1;
     } else
     if (str == "potion of cure serious" && (money<10000)) {
     write("Fanwar says: Come back when you are richer.\n");
     return 1;
     } else
     if (str == "potion of cure serious wounds" || str == "potion of cure serious") {
     write("Fanwar says: I couldn't have made a better selection myself!\n");
     serious = clone_object("players/crimson/exp3");
     move_object(serious, this_player());
     call_other(this_player(), "add_money", -(10000));
     say(name+ " buys a light blue potion.\n");
     return 1;
     } else
     if (str == "potion of hallucination" && (money<250)) {
     write("Fanwar says: Typical! Go beg for your booze somewhere else!\n");
     return 1;
     } else
     if (str == "potion of hallucinations" && (money<250)) {
     write("Fanwar says: Hah! Go back to the gutter where you belong!\n");
     return 1;
     } else
     if (str == "potion of hallucinations" || str == "potion of hallucination") {
     write("Fanwar says: You can't find better stuff than this.\n");
     booze = clone_object("players/crimson/exp7");
     move_object(booze, this_player());
     call_other(this_player(), "add_money", -(250));
     say(name+ " buys a multi-colored potion.\n");
     return 1;
     } else
     if (str == "potion of healing" && (money<2000)) {
     write("Fanwar says: (under his breath) Vagabonds! Always want a freebie.\n");
     return 1;
     } else
     if (str == "potion of healing") {
     write("Fanwar says: It's from one of my best batches!\n");
     heal = clone_object("players/crimson/exp5");
     move_object(heal, this_player());
     call_other(this_player(), "add_money", -(2000));
     say(name + " buys an orange potion.\n");
     return 1;
     } else
     if (str == "potion of minor healing" && (money<250)) {
     write("Fanwar says: You don't have the money for that?  Get out!\n");
     return 1;
     } else
     if (str == "potion of minor healing") {
     write("Fanwar says: Every little bit counts when you are adventuring!\n");
     minor = clone_object("players/crimson/exp4");
     move_object(minor, this_player());
     say(name + " buys a light green potion.\n");
     call_other(this_player(), "add_money", -(250));
     return 1;
     } else
     if (str == "potion of vitality" && (money<200)) {
     write("Fanwar says: Less than 200? Absurd!\n");
     return 1;
     } else
     if (str == "potion of vitality") {
     write("Fanwar says: Now that will get you hopping!\n");
     coffee = clone_object("players/crimson/exp6");
     move_object(coffee, this_player());
     call_other(this_player(), "add_money", -(200));
     say(name+ " buys a black potion.\n");
     return 1;
     } else
     if (str == "potion of polymorph" && (money<100) || str == "potion of random polymorph" && (money<100)) {
     write("Fanwar says: That does it! Get out of my shop now!!!\n");
     return 1;
     } else
     if (str == "potion of polymorph" || str == "potion of random polymorph") {
     write("Fanwar says: You look fine the way you are! But sure.\n");
     polymorph = clone_object("players/crimson/exp8");
     move_object(polymorph, this_player());
     call_other(this_player(), "add_money", -(100));
     say(name+ " buys a purple potion.\n");
     return 1;
     } else
     if (str == "potion") {
     write("Fanwar says: Which potion is it that you wish to buy?\n");
     return 1;
     } else
     if (str) {
     write("Fanwar says: Excuse me? What is it you wish to purchase?\n");
     return 1;
     }
}

long() {
    write("                             Ye Old Alchemy Shoppe.             \n");
    write("                                Name your poison.                 \n");
    write("               ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
    write("               ^                                              ^\n");
    write("               ^  Potion of Minor Healing          250 coins  ^\n");
    write("               ^  Potion of Healing               2000 coins  ^\n");
    write("               ^  Potion of Cure Serious Wounds  10000 coins  ^\n");
    write("               ^  Potion of Vitality               200 coins  ^\n");
    write("               ^  Potion of Hallucinations         250 coins  ^\n");
    write("               ^  Potion of Fire Breath           1000 coins  ^\n");
    write("               ^  Potion of Random Polymorph       100 coins  ^\n");
    write("               ^  Potion of Giant Strength        2000 coins  ^\n");
    write("               ^                                              ^\n");
    write("               ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
    write("         \n");
    write("                        The only obvious exit is east.\n");
    return 1;
}

extra_reset() {
object fanwar;

    if(!present("fanwar")) {
    fanwar = clone_object("obj/monster.talk");
    call_other(fanwar, "set_name", "fanwar");
    call_other(fanwar, "set_short", "Fanwar the Alchemist (Elvish)");
    call_other(fanwar, "set_level", 19);
    call_other(fanwar, "set_eps", 1000000);
    call_other(fanwar, "set_wc", 20);
    call_other(fanwar, "set_ac", 7);
    call_other(fanwar, "set_al", 1000);
    call_other(fanwar, "set_race", "elf");
    call_other(fanwar, "set_long", "Fanwar looks small and wirey.  His complexion\n"+
    "is slightly pale and he has nearly white hair.\n"+
    "His pale blue eyes dart about happily.\n");
    call_other(fanwar, "set_chat_chance", 5);
    call_other(fanwar, "set_a_chat_chance", 25);
    call_other(fanwar, "load_chat", "Fanwar says: Have you tried the potion of Giant Strenth?\n"+
    "Fanwar says: It will put hair on your chest yes!\n");
    call_other(fanwar, "load_chat", "Fanwar says: I put only the finest ingredients in my potions.\n");
    call_other(fanwar, "load_chat", "Fanwar smiles happily.\n");
    call_other(fanwar, "load_a_chat", "Fanwar says: You will pay for this!  I have friends!\n");
    call_other(fanwar, "load_a_chat", "Fanwar grunts.\n");
    call_other(fanwar, "load_a_chat", "Fanwar says: Take that!\n");
    call_other(fanwar, "load_a_chat", "Fanwar says: Guards! Guards! Help!\n");
    move_object(fanwar, this_object());
    }
}

realm() { return "NT"; }
