int charges,i;

id(str) {
       return str == "slug juice" || str == "juice" || str == "potion";
        }


reset() {
       }

long() {
    write("The potion is sort of green and goopy.\n");
    }

short() {
     return "A potion of slug juice";
}

query_magic() { return 1;}

query_value()
{
    return 200;
}


init() {
    add_action("drink"); add_verb("drink");
}

drink(arg) {
    if (!id(arg)) 
       return 0;
    say(call_other(this_player(),"query_name",0) 
                     + " slugs down some juice.\n");
    write("You feel much better...\n");
    call_other(this_player(),"heal_self",15+random(25));
    destruct(this_object());
    return 1;
}

get() {
    return 1;
}

query_weight() {
    return 1;
}
