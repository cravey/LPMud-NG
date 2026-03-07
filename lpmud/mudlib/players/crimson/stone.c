object player;
string level;
string hps;
int max;
int runn;
int onn;

short() { return "A blackened stone of wisdom"; }

long() {
    write("It is warm to the touch and worn with use.\n");
    return 1;
}

id(str) {
    return str == "stone" || str == "wisdom stone";
}

init() {
    add_action("rub"); add_verb("rub");
}

rub(str) {
    player = this_player();
    level = call_other(this_player(), "query_level");
    max = 42 + level * 8;
    runn = max/4;
    if(str == "stone") {
    write("You rub the stone and feel more confident.\n");
    onn = 1;
    set_heart_beat(1);
    return 1;
    } else
    if (str == "stone" && (onn == 1)) {
    write("You rub the stone and feel more confident.\n");
    return 1;
    } else
    if (!str) {
    return 0;
    } else
    if (str) {
    return 0;
    }
}

get() { return 1; }

heart_beat() {
    hps = call_other(player, "query_hp");
    if ((hps<runn) && !(call_other(player, "query_ghost")) && (call_other(player, "query_attack"))) {
    call_other(player, "run_away");
    }
}

query_value() { return 25; }
