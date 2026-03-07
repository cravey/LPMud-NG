# 1 "players/kantele/castle.c"
# 1 "<built-in>" 1
# 1 "<built-in>" 3
# 465 "<built-in>" 3
# 1 "<command line>" 1
# 1 "<built-in>" 2
# 1 "players/kantele/castle.c" 2


int manservant_dead, gate_open;
object manservant;

# 14 "players/kantele/castle.c"
   
id(str) { return str == "mansion" || str == "sign" || str == "gate"; }

short() {
    return "The Mansion of Kantele is to the south";
}

long(str) {
    if (str == "sign") {
        write("\nBEWARE OF THE DOGS\n\n"+
               "SQUATTERS WILL BE EATEN !\n\n");
	return;
    }
    write("A large fence topped by barbed wire surronds the house, the only\n");
    write("entrance is a huge iron gate leading into the garden. There is a\n");
    write("sign on the gate. You can also see a bell rope.\n");
}

init() {
    add_action("enter");add_verb("south");
    add_action("pull_rope");add_verb("pull");
}

pull_rope(str) {
    if (str != "rope")
       return 1;
    else {
       if (gate_open == 1) {
          write("nothing happens...\n");
          return 1;
       } else {
          gate_open = 1;
	  say(call_other(this_player(), "query_name") +
	    " pulls the rope...\n");
          say("The gate opens !\n");
          say("Kanteles manservant arrives.\n");
          say("Kanteles manservant says: get lost punk !\n");
          write("The gate opens !\n");
          write("Kanteles manservant arrives.\n");
          write("Kanteles manservant says: get lost punk !\n");
	  if (!manservant || !living(manservant))
	      manservant = clone_object("players/kantele/manservant");
          move_object(manservant, environment(this_object()));
	}
    }
}

enter(str) {
    if (manservant && environment(manservant) == environment(this_object())) {
	write("Kanteles servant bars your way !\n");
    } else if (gate_open == 0) {
	write("The gates are closed.\n");
    } else {
	call_other(this_player(), "move_player",
            "south#players/kantele/brick_road");
    }
    return 1;
}

reset(arg) {
    

       
    if (manservant && living(manservant))
	call_other(manservant, "move_player", "X#players/kantele/castle");
    gate_open = 0;
    if (arg)
	return;
    move_object(this_object(), "room/vill_track");
}

