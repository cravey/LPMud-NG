reset(arg)
{
    if (!arg)
	set_light(1);
    extra_reset();
}

extra_reset()
{
    object man, robe;
    int i;

    i = 0;
    if (!present("man")) {
	while(i<10) {
	    i += 1;
	    man = clone_object("obj/monster.talk");
	    call_other(man, "set_name", "man");
	    call_other(man, "set_alias", "dirty crap");
	    call_other(man, "set_race", "man");
	    call_other(man, "set_level", random(2) + 1);
	    call_other(man, "set_hp", 30);
	    call_other(man, "set_ep", 1014);
	    call_other(man, "set_al", -60);
	    call_other(man, "set_short", "A man cloaked in a green garb");
            call_other(man, "set_long", "With closer a closer look at the man you can tell he was lobotomized.\n");
	    call_other(man, "set_ac", 0);
	    call_other(man, "set_aggressive", 0);
	    call_other(man, "set_chat_chance", 5);
	    call_other(man, "load_chat", "One man drools on your boots.\n");
	    call_other(man, "load_chat", "One man starts banging his head on the cavern wall.\n");
	    call_other(man, "load_chat", "In the distance you hear a loud gruff voice say: Where is my gruel?!\n");
	    call_other(man, "load_chat", "A voice echos through the halls: I hate this job but, I love the pay! HA HA HA!\n");
	    call_other(man, "load_chat", 
		"In the distance someone grumbles: Ornic, did you hear somethin?\n");
	    call_other(man, "load_chat", 
           "The cavern walls echo back at you: Nah, Grok, its one of the lovelies out there.\n");
	    call_other(man, "load_chat", 
               "A man starts crying.\n");
            robe = clone_object("obj/armor");
            call_other(robe, "set_name", "robe");
            call_other(robe, "set_short", "A green soiled robe");
	    call_other(robe, "set_ac", 0);
            call_other(robe, "set_value", 75);
            call_other(robe, "set_weight", 1);
            call_other(robe, "set_alt_name", "green robe");
            call_other(robe, "set_long", "The robe seems to smell of moth balls.\n");
	    transfer(robe, man);
            call_other(robe, "wear", "robe");
	    move_object(man, this_object());
	}
    }
}
init()
{
    add_action("south"); add_verb("south");
    add_action("north"); add_verb("north");
}

north()
{
    if (present("man")) {
	write("An man bars your way.\n");
	return 1;
    }
    call_other(this_player(), "move_player", "north#room/man_treasure");
    return 1;
}

south()
{
    call_other(this_player(),"move_player", "south#room/man_vall");
    return 1;
}

long()
{
    write("This is the local strong point of the mans.\n");
    write("There is an entrance to a small room to the north.\n");
}

short() {
    return "The man fortress";
}
