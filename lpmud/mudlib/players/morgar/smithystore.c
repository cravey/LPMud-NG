#define MAX_LIST        30

int value;
string name_of_item;

short() {
    return "store room for the smithy";
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
        if (str == "armors" && call_other(ob, "armor_class", 0)) {
            list(ob);
            max -= 1;
	}
        ob = next_inventory(ob);
    }
}

list(ob)
{
    int value;
    value = call_other(ob, "query_value");
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

fill()
{
    if (!present("dagger")) {
        object dagger;
        dagger = clone_object("obj/weapon");
        call_other(dagger, "set_name", "a dagger");
        call_other(dagger, "set_class", 6);
        call_other(dagger, "set_weight", 2);
        call_other(dagger, "set_value", 12);
        call_other(dagger, "set_alt_name", "weapon");
        call_other(dagger, "set_alias", "dagger");
        call_other(dagger, "set_short", "a dagger");
        call_other(dagger, "set_long", "a plain dagger\n");
        move_object(dagger, this_object());
    }
    if (!present("sword")) {
        object sword;
        sword = clone_object("obj/weapon");
        call_other(sword, "set_name", "a sword");
        call_other(sword, "set_class", 10);
        call_other(sword, "set_weight", 4);
        call_other(sword, "set_value", 250);
        call_other(sword, "set_alt_name", "weapon");
        call_other(sword, "set_alias", "sword");
        call_other(sword, "set_short", "a sword");
        call_other(sword, "set_long", "a plain sword\n");
        move_object(sword, this_object());
    }
    if (!present("longsword")) {
        object longsword;
        longsword = clone_object("obj/weapon");
        call_other(longsword, "set_name", "a longsword");
        call_other(longsword, "set_class", 12);
        call_other(longsword, "set_weight", 2);
        call_other(longsword, "set_value", 1000);
        call_other(longsword, "set_alt_name", "weapon");
        call_other(longsword, "set_alias", "longsword");
        call_other(longsword, "set_short", "a longsword");
        call_other(longsword, "set_long", "an unadorned longsword\n");
        move_object(longsword, this_object());
    }
    if (!present("chainmail")) {
        object chainmail;
        chainmail = clone_object("obj/armor");
        call_other(chainmail, "set_name", "chainmail");
        call_other(chainmail, "set_ac", 3);
        call_other(chainmail, "set_weight", 5);
        call_other(chainmail, "set_value", 500);
        call_other(chainmail, "set_alias", "armor");
        call_other(chainmail, "set_type", "armor");
        call_other(chainmail, "set_short", "a suit of chainmail");
        call_other(chainmail, "set_long", "a suit of chainmail\n");
        move_object(chainmail, this_object());
    }
    if (!present("platemail")) {
        object platemail;
        platemail = clone_object("obj/armor");
        call_other(platemail, "set_name", "platemail");
        call_other(platemail, "set_ac", 4);
        call_other(platemail, "set_weight", 10);
        call_other(platemail, "set_value", 1500);
        call_other(platemail, "set_alias", "armor");
        call_other(platemail, "set_type", "armor");
        call_other(platemail, "set_short", "a suit of platemail");
        call_other(platemail, "set_long", "a suit of platemail\n");
        move_object(platemail, this_object());
    }
    if (!present("shield")) {
        object shield;
        shield = clone_object("obj/armor");
        call_other(shield, "set_name", "medium shield");
        call_other(shield, "set_ac", 1);
        call_other(shield, "set_weight", 4);
        call_other(shield, "set_value", 200);
        call_other(shield, "set_alias", "shield");
        call_other(shield, "set_type", "shield");
        call_other(shield, "set_short", "a medium shield");
        call_other(shield, "set_long", "a medium shield\n");
        move_object(shield, this_object());
    }
    if (!present("helmet")) {
        object helmet;
        helmet = clone_object("obj/armor");
        call_other(helmet, "set_name", "metal helmet");
        call_other(helmet, "set_ac", 1);
        call_other(helmet, "set_weight", 3);
        call_other(helmet, "set_value", 300);
        call_other(helmet, "set_alias", "helmet");
        call_other(helmet, "set_type", "helmet");
        call_other(helmet, "set_short", "a metal helmet");
        call_other(helmet, "set_long", "a metal helmet\n");
        move_object(helmet, this_object());
    }
}

/* set_{class,weight,value,alt_name,alias,short,long,read,hit_func,wield_func}*/
reset(arg) {
    if (!arg)
        set_light(1);
}

long()
{
    write("All things from the smithy are stored here.\n");
}

store(item)
{
    string short_desc;
    object ob;

    short_desc = call_other(item, "short");
    ob = first_inventory(this_object());
    while(ob) {
        if (call_other(ob, "short") == short_desc) {
            /* Move it before destruct, because the weight
               has already been compensated for. */
            move_object(item, this_object());
            destruct(item);
            return;
	}
        ob = next_inventory(ob);
    }
    move_object(item, this_object());
}
