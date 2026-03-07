int charges,i;

id(str) {
       return str == "potion of death" || str == "juice" || str == "potion";
        }


reset() {
       }

long() {
    write(
"The potion is black.  There is a skull and cross bones on the label.\n");
    }

short() {
     return "A potion of death";
}

query_magic() { return 1;}

query_value()
{
    return 100;
}

init() {
    add_action("drink"); add_verb("drink");
}

drink(arg) {
    int m;
    string name;
    object corpse;

    if (!id(arg)) 
       return 0;
    name =call_other(this_player(),"query_name",0);
    say (name + " drinks a potion of death.\n");
    if (!call_other(this_player(), "second_life", 0)) {
        write("You cannot die! You are immortal!!!\n");
        destruct(this_object());
        return 1;
    }
    say(name + " dies.\n");
    corpse = clone_object("obj/corpse");
    call_other(corpse, "set_name", name);
    call_other(this_player(),"transfer_all_to",corpse);
    m = call_other(this_player(),"query_money");
    call_other(this_player(),"add_money",-m);
    move_object(corpse, environment(this_player()));
    if (!call_other(this_player(), "second_life", 0)) 
      destruct(this_player());
    destruct(this_object());
    return 1;
}

get() {
    return 1;
}

query_weight() {
    return 1;
}


