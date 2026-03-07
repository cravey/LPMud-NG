string fruit;
object fruit;
string tree;
string nose;
reset(arg)
{
    if (!arg)
	set_light(1);
    extra_reset();
}

extra_reset()
{
    object squirrel, nut;
    int i;

    i = 0;
    if (!present("squirrel")) {
	while(i<2) {
	    i += 1;
	    squirrel = clone_object("obj/monster");
	    call_other(squirrel, "set_name", "squirrel");
            call_other(squirrel, "set_short", "A grey squirrel");
	    call_other(squirrel, "set_alias", "grey squirrel");
	    call_other(squirrel, "set_race", "rodent");
            call_other(squirrel, "set_level", 9);
	    call_other(squirrel, "set_hp", 90);
	    call_other(squirrel, "set_ep", 8000);
	    call_other(squirrel, "set_al", 0);
	    call_other(squirrel, "set_ac", 3);
            call_other(squirrel, "set_wc", 13);
	    call_other(squirrel, "set_aggressive", 0);
            move_object(squirrel, this_object());
            nut = clone_object("obj/treasure");
            call_other(nut, "set_id", "nut");
            call_other(nut, "set_short", "A small nut");
            call_other(nut, "set_weight", 1);
            call_other(nut, "set_value", 250);
            transfer(nut, squirrel);
	}
    }
}
init()
{
    add_action("climb"); add_verb("climb");
    add_action("jump"); add_verb("jump");
    add_action("enter"); add_verb("enter");
    add_action("look"); add_verb("look");
    add_action("pick"); add_verb("pick");
}

look(str)
{
    if (str == "at tree") {
    write("Before you is the tree of knowledge. It's fruits shall do thee well in thy\n");
    write("travels. Aquiring it's bounty is not always easy however.\n");
    write("You can see a large opening at the bottom of the tree.\n");
    return 1;
    } else
    if (str == "at willow") {
    write("Before you is the tree of knowledge. It's fruits shall do thee well in thy\n");
    write("travels. Aquiring it's bounty is not always easy however.\n");
    write("You can see a large opening at the bottom of the tree.\n");
    return 1;
    } else
    if (str == "at weeping willow") {
    write("Before you is the tree of knowledge. It's fruits shall do thee well in thy\n");
    write("travels. Aquiring it's bounty is not always easy however.\n");
    write("You can see a large opening at the bottom of the tree.\n");
    return 1;
    } else
    if (str == "at opening") {
    write("You gaze into the opening at the base of the tree. At first you\n");
    write("see nothing but darkness, with closer examination however, you\n");
    write("see hazy colored patterns inside. You feel compelled enter.\n");
    return 1;
    } else
    if (str == "at stream") {
    write("Staring into the stream you see the image of yourself and the tree. The tree\n");
    write("maintains truth in it's reflection but your likeless is distorted as it is with\n");
    write("the way man perceives himself. Here lies the fork between truth and knowledge.\n");
    write("A true and knowledgable wizard chooses his direction well, reflects upon himself\n");
    write("as a stream reflects images and uses his knowledge to maintain truth.\n");
    return 1;
    }
}

climb(str)
{
   if (str == "tree") {
    call_other(this_player(), "move_player", "up the tree#players/crimson/treetop");
    write("You climb up the tree and learn yet another lesson. Sometimes when one\n");
    write("looks at things in a different way one finds the knowledge which leads\n");
    write("him to the answer.\n");
    return 1;
    }
}

pick(str) {
   if (str == "nose") {
     write("You're a sick puppy.... aren't you?\n");
     return 1;
   } else
   if (str == "fruit") {
   write("You are not high enough to aquire that.\n");
      return 1;
   }
}

jump(str) {
   if (str == "in stream") {
   write("You jump into the stream and splash around. You are now wet and\n");
   write("cold and you feel pretty silly.\n");
   return 1;
   }
}

long()
{
    write("The air here is heavy with a glittery mist. A cold wind blows about\n");
    write("the base of a the tree before you. This place's magic makes your skin\n");
    write("tingle. The tree has the appearance of a weeping willow. Its branches\n");
    write("slump over a small stream. The stream burbles like the sound of ancient\n");
    write("voices. Occasionally, you see a pair of grey squirrels scitter about\n");
    write("the branches. Mystery and awe enshroud this place so much that you find\n");
    write("it difficult to breath.\n");
}

short() {
    return "The greeting hall";
}

enter(str) {
    if (str != "tree") {
    write("What do you want to enter?\n");
      return 1;
    } else
    if (!present("fruit", this_player())) {
        write("A gust of wind propels you back.\n");
    return 1;
    } else
    if (present("fruit", this_player())) {
    call_other(this_player(), "move_player", "down through the vat#players/crimson/underworld");
    write("You are now worthy of the underlands. Treat this privelege well.\n");
    return 1;
    }
}

id(str) {
    return str == "at tree" || str == "at stream";
}

query_fruit() {
    return fruit;
}

realm() { return "NT"; }
