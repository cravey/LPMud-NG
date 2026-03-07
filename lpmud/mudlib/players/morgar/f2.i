# 1 "players/morgar/f2.c"
id(arg) {return arg=="facade";}

short() {
    return "The magic shop is to the north";
}

long() {
    write("This is a facade.\n");
}

init() {
    add_action("move"); add_verb("north");
}

move(str) {
    call_other(this_player(), "move_player", "north#players/morgar/magic");
    return 1;
}

get() {
    return(call_other(this_player(),"query_level",0) >= 20);
}

reset(arg) {
    if (arg)
        return;
    move_object(this_object(), "room/yard");
}
