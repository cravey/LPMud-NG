/*
 * This is a curse that the player can't get rid of.
 * It prevents you from speaking.
 */

query_auto_load() {
    return "players/ringer/pyramid/tcurse::";
}

start(ob) {
    move_object(this_object(), ob);
    tell_object(ob, "You find it impossible to speak.\n");
}

id(str) {
    return str == "mute_curse" || str=="markings" || str=="throat";
}

long() {
    write("The marks seem like bruises, but are clearly magical in nature.\n");
}

drop() { return 1; }

init() {
    add_action("do_shout"); add_verb("shout");
    add_action("do_shout"); add_verb("tell");
    add_action("do_shout"); add_verb("say");
    add_action("do_shout"); add_verb("whisper");
    add_action("do_shout"); add_verb("scream");
}

do_shout() {
    write("You find it impossible to speak.\n");
    say(call_other(this_player(), "query_name") +
        " moves his lips, but no noise comes forth.\n");
    return 1;
}

init_arg(str) {
}

short() {
    return "Dark markings on the throat";
}
