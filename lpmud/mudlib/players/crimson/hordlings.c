reset(arg)
{
    if (!arg)
	set_light(1);
    extra_reset();
}

extra_reset()
{
    object hordling, weapon;
    int n,i,class,value,weight;
    int j,k,l;
    string w_name,alt_name;
    string short, mshort, dshort;

    i = 0;
    if (!present("hordling")) {
	while(i<8) {
	    i += 1;
            j = random(6);
            k = random(6);
            l = random(6);
	    hordling = clone_object("obj/monster.talk");
            if (j == 0) {
                short = "black";
            }
            if (j == 1) {
                short = "purple";
            }
            if (j == 2) {
                short = "metallic";
            }
            if (j == 3) {
                short = "crimson";
            }
            if (j == 4) {
                short = "white";
            }
            if (j == 5) {
                short = "blue";
            }
            if (k == 0) {
                mshort = "bat-winged";
            }
            if (k == 1) {
                mshort = "hunch-backed";
            }
            if (k == 2) {
                mshort = "tusked";
            }
            if (k == 3) {
                mshort = "bristle-maned";
            }
            if (k == 4) {
                mshort = "slit-nosed";
            }
            if (k == 5) {
                mshort = "snout-nosed";
            }
            if (l == 0) {
                dshort = "fat";
            }
            if (l == 1) {
                dshort = "lumpy";
            }

            if (l == 2) {
                dshort = "barb-tailed";
            }
            if (l == 3) {
                dshort = "multi-jointed";
            }
            if (l == 4) {
                dshort = "spike-tailed";
            }
            if (l == 5) {
                dshort = "thin";
            }
	    call_other(hordling, "set_name", "hordling");
	    call_other(hordling, "set_alias", "dirty crap");
	    call_other(hordling, "set_race", "hordling");
	    call_other(hordling, "set_level", random(2) + 1);
	    call_other(hordling, "set_hp", 30);
	    call_other(hordling, "set_ep", 1014);
	    call_other(hordling, "set_al", -60);
            call_other(hordling, "set_short", "A "+short+", "+mshort+" and "+dshort+" hordling");

	    call_other(hordling, "set_ac", 0);
	    call_other(hordling, "set_aggressive", 1);
	    call_other(hordling, "set_a_chat_chance", 50);
	    call_other(hordling, "load_a_chat", "A hordling says: Kill him!\n");
	    call_other(hordling, "load_a_chat", "A hordling says: Ugly humans.\n");
	    call_other(hordling, "load_a_chat", "A hordling grins evilly at you.\n");
	    call_other(hordling, "load_a_chat", "A hordling says: Lets eat his flesh!\n");
	    call_other(hordling, "load_a_chat", 
		"A hordling says: Let's take him to Glasya!\n");
	    call_other(hordling, "load_a_chat", 
	       "A hordling says: Destory him quickly!\n");
	    call_other(hordling, "load_a_chat", 
	       "A hordling says: What is that human doing down here?!\n");
	    n = random(3);
	    weapon = clone_object("obj/weapon");
	    if (n == 0) {
		w_name = "crab pincher";
		class = 9;
		value = 100;
		weight = 1;
              alt_name = "pincher";
	    }
	    if (n == 1) {
		w_name = "eagle talon";
		class = 11;
		value = 150;
		weight = 1;
		alt_name = "talon";
	    }
	    if (n == 2) {
		w_name = "bear claw";
		class = 15;
		value = 250;
		weight = 2;
		alt_name = "claw";
	    }
	    call_other(weapon, "set_name", w_name);
	    call_other(weapon, "set_class", class);
	    call_other(weapon, "set_value", value);
	    call_other(weapon, "set_weight", weight);
	    call_other(weapon, "set_alt_name", alt_name);
	    transfer(weapon, hordling);
	    call_other(weapon, "wield", w_name);
	    move_object(hordling, this_object());
	}
    }
}
init()
{
    add_action("east"); add_verb("east");
    add_action("north"); add_verb("north");
}

north()
{
    if (present("hordling")) {
	write("An hordling blocks your way.\n");
	return 1;
    }
    call_other(this_player(), "move_player", "north#players/crimson/glasya");
    return 1;
}

east()
{
    call_other(this_player(), "move_player", "east#players/crimson/underworld10");
    return 1;
}

long()
{
    write("This looks like the nest of some truely foul creatures.\n");
    write("You have to hold your nose to keep from vomiting.\n");
    write("You can see an opening to the north.\n");
}

short() {
    return "The hordling stenchpot";
}

realm() { return "NT"; }

