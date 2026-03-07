#define MAX_LIST	20

int value;
string name_of_item;

short() {
    return "store room for Grog's shop";
}

init() {
    add_action("east"); add_verb("east");
}

inventory(str)
{
    object ob;
    int max;
    if (!str)
	str = "all";
    max = MAX_LIST;
    ob = first_inventory(this_object());
    while(ob && max > 0) {
	if (str == "all") {
	    list(ob);
	    max -= 1;
	}
	if (str == "weapons" && call_other(ob, "weapon_class", 0)) {
	    list(ob);
	    max -= 1;
	}
	if (str == "armours" && call_other(ob, "armour_class", 0)) {
	    list(ob);
	    max -= 1;
	}
	ob = next_inventory(ob);
    }
}

list(ob)
{
    int value;
    
    value = call_other(ob, "query_value", 0);
    if (value) {
	write(value*2 + ":\t" + call_other(ob, "short") + ".\n");
    }
}

value(item) {
    name_of_item = present(item);
    if (!name_of_item) {
	return 0;
    }
    value = call_other(name_of_item, "query_value", 0);
    if (!value) {
	return 0;
    }
    write("The "); write(item); write(" would cost you ");
    write(value*2); write(" gold coins.\n");
    return 1;
}

buy(item) {
    name_of_item = present(item);
    if (!name_of_item) {
	write("No such item in the store.\n");
	return;
    }
    value = call_other(name_of_item, "query_value", 0);
    if (!value) {
	write("Item has no value.\n");
	return;
    }
    value *= 2;
    if (call_other(this_player(), "query_money", 0) < value) {
	write("It would cost you ");
	write(value); write(" gold coins, which you don't have.\n");
	return;
    }
    if (!call_other(this_player(), "add_weight",
		    call_other(name_of_item, "query_weight"))) {
	write("You can't carry that much.\n");
	return;
    }
    call_other(this_player(), "add_money", 0 - value);
    move_object(name_of_item, this_player());
    write("Ok.\n");
    say(call_other(this_player(), "query_name") + " buys " + item + ".\n");
}

east() {
    call_other(this_player(), "move_player", "X#players/ringer/mg_shop");
    return 1;
}


reset(arg) {
    if (!arg)
	set_light(1);
    if (!present("slug juice")) {
	object p1;
	p1 = clone_object("players/ringer/slug_potion");
	move_object(p1, this_object());
    }
    if (!present("potion of death")) {
	object p;
	p = clone_object("players/ringer/death");
	move_object(p, this_object());
    }
    if (!present("scroll of identify")) {
	object p2;
	p2 = clone_object("players/ringer/scrolli");
        call_other(p2,"set_name","scroll of identify");
        call_other(p2,"set_alias","scroll");
        call_other(p2,"set_value",75);
        call_other(p2,"set_weight",1);
        call_other(p2,"set_short","scroll of identify");
	move_object(p2, this_object());
    }
    if (!present("scroll of shelter")) {
	object p3;
	p3 = clone_object("players/ringer/shelt");
        call_other(p3,"set_id","scroll of shelter");
        call_other(p3,"set_alias","scroll");
        call_other(p3,"set_value",200);
        call_other(p3,"set_weight",1);
        call_other(p3,"set_short","scroll of shelter");
        call_other(p3,"set_read","Usage: shelter\n");
        call_other(p3,"set_action","shelter player/ringer/mg_sh");
	move_object(p3, this_object());
    }
}

long()
{
    write("All things from Grog's shop are stored here.\n");
}

store(item)
{
    string short_desc;
    object ob;

    short_desc = call_other(item, "short");
    ob = first_inventory(this_object());
    while(ob) {
	if (call_other(ob, "short") == short_desc) {
	    destruct(item);
	    return;
	}
	ob = next_inventory(ob);
    }
    move_object(item, this_object());
}
