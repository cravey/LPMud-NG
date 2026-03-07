# 1 "players/crimson/.egg.c" 
int charges;

reset(arg) {
    if (!arg) charges = 1;
}

short() { return "A HUGE green egg"; }

long() { 
    write("The egg has a slightly metalic sheen.\n");
    write("It seems to be very heavey.\n");
}

init() {
   add_action("hatch"); add_verb("fireball");
   add_action("hatch"); add_verb("shock");
   add_action("hatch"); add_verb("missile");
   add_action("hatch"); add_verb("kill");
   add_action("hatch"); add_verb("buy");
}

hatch(str) {
object bbdragon;
object where;
object player;
int n;

    player = this_player();
    where = environment(this_player());
    n = random(12);
    if(!str && n == 11 || str == "dragon" || str == "baby dragon" || str == "egg" || str == "nog") {
    write("Crackle, crackle, .... crunch, crunch, crunch....... POP!\n");
    say("Crackle, crackle, .... crunch, crunch, crunch ....... POP!\n");
    write("The egg breaks into a million pieces.\n");
    say("The egg breaks into a million pieces.\n");
    bbdragon = clone_object("obj/monster");
    call_other(bbdragon, "set_name", "baby green dragon");
    call_other(bbdragon, "set_short", "A baby green dragon");
    call_other(bbdragon, "set_alias", "baby dragon");
    call_other(bbdragon, "set_race", "dragon");
    call_other(bbdragon, "set_level", 8);
    call_other(bbdragon, "set_ac", 7);
    call_other(bbdragon, "set_wc", 14);
    call_other(bbdragon, "set_chance", 50);
    call_other(bbdragon, "set_spell_mess1", "The baby green dragon breathed a cloud of gas!\n");
    call_other(bbdragon, "set_spell_mess2", "The baby green dragon breathed a cloud of gas on you!\n");
    call_other(bbdragon, "set_spell_dam", 20);
    call_other(bbdragon, "set_aggressive", 1);
    call_other(bbdragon, "set_move_at_reset",1);
    move_object(bbdragon, where);
    call_other(bbdragon, "attack_object", player);
    if (charges == 1) {
         destruct(this_object());
         }
    }
}

id(str) {
    return str == "egg" || str == "dragon egg" || str == "green egg";
}

query_value() { return 77; }

query_weight() { return 9; }

get() { return 1; }
