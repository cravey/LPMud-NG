int i;
object snake;
reset() {
    i=0;
    if (!present("snake")) {
        while (i<10) {
            make_snake();
            i += 1;
        }
    }
}

make_snake() {
    int t,a,w,l;
    string name;
    t = random(4);
    if (t==0) {
        name="black snake";
        a = 2;
        w = 5;
        l = 3;
    }
    if (t==1) {
        name="pit viper";
        a = 3;
        w = 8;
        l = 5;
    }
    if (t==2) {
        name="cobra";
        a = 3;
        w = 16;
        l = 6;
    }
    if (t==3) {
        name="black mamba";
        a = 3;
        w = 18;
        l = 8;
    }
    snake = clone_object("obj/monster");
    call_other(snake, "set_name", name);
    call_other(snake, "set_alias", "snake");
    call_other(snake, "set_level", l);
    call_other(snake, "set_wc",w);
    call_other(snake, "set_ac",a);
    call_other(snake, "set_al", -60);
    call_other(snake, "set_short", "A "+name);
    call_other(snake, "set_aggressive", 1);
    move_object(snake,this_object());
}

short() { return "A viper pit"; }

long() {
    write(
"This is a deep snake pit.  Those who fall in here are not intended to \n"+
"get out.  In fact, they are intended to be eaten by nasty snakes.\n");
    write("You might be able to climb up.\n");
}

init() {
    add_action("search"); add_verb("search");
    add_action("up"); add_verb("up");
    add_action("up"); add_verb("climb");
}

up() {
    if (present("snake") && !call_other(this_player(),"query_ghost")) {
        write("The snakes make it impossible to climb.\n");
        return 1;
    }
    call_other(this_player(),"move_player",
               "up#players/ringer/pyramid/tun4");
    return 1;
}

search(str) {
    if (!str) str = "room";
    if (str == "room") {
        say(call_other(this_player(),"query_name") + " searches the room.\n");
        write("You find snake holes (What a surprise!)\n");
        return 1;
    }
}

