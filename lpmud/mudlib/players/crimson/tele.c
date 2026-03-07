int charges,i;

id(str) {
       return str == "teleporter";
        }


reset() {
       i=random(6);
       }

long() {
    write("The inscriptions on the teleporter say that it will\n");
    write("take you to somewhere in the prime material plane.\n");
    }

short() {
     return "A tiny teleporter";
}

query_value()
{
    return 100;
}

init() {
    add_action("use"); add_verb("use");
}

use(arg) {
    if (arg && arg != "teleporter") {
       write("read what?\n");
    } else {

       write("As you use the teleporter everything fades away...\n");
    if (i == 1)
        call_other(this_player(), "move_player", "X#players/crimson/underworld10");
    else
    if (i == 2)
        call_other(this_player(), "move_player", "X#room/church");
    else
    if (i == 3)
        call_other(this_player(), "move_player", "X#room/pub2");
    else
    if (i == 4)
        call_other(this_player(), "move_player", "X#players/crimson/entry");
    else
    if (i == 5)
        call_other(this_player(), "move_player", "X#players/crimson/were");
        else
        call_other(this_player(), "move_player", "X#players/crimson/iceunder");
    move_object(this_object(), "players/crimson/storeroom");
    write("The teleporter suddenly disappears !\n");
    return 1;
}
}

get() {
    return 1;
}

query_weight() {
    return 1;
}
