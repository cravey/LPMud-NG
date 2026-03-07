int charges;

reset(arg) {
    if (!arg) charges = 1;
}

short() { return "A HUGE purple egg"; }

long() { 
    write("The egg has a slightly metalic sheen.\n");
    write("It seems to be very heavey.\n");
}

init() {
   add_action("hatch"); add_verb("fireball");
   add_action("hatch"); add_verb("shock");
   add_action("hatch"); add_verb("missile");
   add_action("hatch"); add_verb("kill");
}

hatch(str) {
object bbdragon;
object where;
object player;
int n;

    player = this_player();
    where = environment(this_player());
    n = random(12);
    if(!str && n == 11 || str == "taiamat" || str == "dragon" || str == "lady of dragons" || str == "baby dragon" && n == 11) {
    write("Crackle, crackle, .... crunch, crunch, crunch....... POP!\n");
    say("Crackle, crackle, .... crunch, crunch, crunch ....... POP!\n");
    bbdragon = clone_object("obj/monster");
    call_other(bbdragon, "set_name", "baby chromatic dragon");
    call_other(bbdragon, "set_short", "A baby chromatic dragon");
    call_other(bbdragon, "set_alias", "dragon");
    call_other(bbdragon, "set_level", 8);
    call_other(bbdragon, "set_ac", 7);
    call_other(bbdragon, "set_wc", 17);
    call_other(bbdragon, "set_a_chat_chance", 40);
    call_other(bbdragon, "load_a_chat", "The baby chromatic's white head\n" +
                    "hit you with a cone of cold!\n");
    call_other(bbdragon, "load_a_chat", "The baby chromatic's red head\n" +
                    "hit you with a blast of fire!\n");
    call_other(bbdragon, "load_a_chat", "The baby chromatic's blue head\n" +
                    "hit you with a bolt of lightning!\n");
    call_other(bbdragon, "load_a_chat", "The baby chromatic's black head\n" +
                    "spurted acid at you!\n");
    call_other(bbdragon, "load_a_chat", "The baby chromatic's green head\n"+
                    "breathed a cloud of gas at you!\n");
    call_other(player, "hit_player", random(5));
    call_other(bbdragon, "set_aggressive", 1);
    call_other(bbdragon, "set_move_at_reset",1);
    move_object(bbdragon, where);
    call_other(bbdragon, "attack_object", player);
    if (charges == 1) {
         destruct(this_object());
         tell_object(player, "The egg's shell breaks into a million pieces.\n");
         }
    }
}

id(str) {
    return str == "egg" || str == "dragon egg" || str == "purple egg";
}

query_value() { return 77; }

query_weight() { return 9; }

get() { return 1; }
