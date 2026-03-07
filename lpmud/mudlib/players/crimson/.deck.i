# 1 "players/crimson/.deck.c" 
# 1 "/home/einstein/mud/lpmud/mudlib/obj/handshake.h" 1



string handshake_data;
int handshake;









get_handshake(confirm) {
    if (confirm == handshake && handshake) {
        handshake = 0;
	return handshake_data;
    } else {
        return 0;
    }
}
# 2 "players/crimson/.deck.c" 2
int i, charges;
int magic;

id(str) { return str == "cards" || str == "card" || str == "deck" || str == "card from deck" || str == "deck of many things" || str == "from deck"; }

reset(arg) {
    if (!arg)
    charges = random(10);
    magic = 1;
}

long() {
    write("***********\n");
    write("*         *\n");
    write("*        ***********\n");
    write("*       *         *    This is Crimson's Deck of Many Things.\n");
    write("*      *         *     She doesn't mind if you draw from it\n");
    write("*     *    *    *      but be warned, it is not always healthy\n");
    write("*    *   *  *  *       for mortals.\n");
    write("*   *    *    *   \n");
    write("*  *         *    \n");
    write("* *         *     \n");
    write("************      \n");
    write("\n");
    write("**WARNING** The surgeon general has determined that the use of this\n");
    write("product may be hazardous to your health.  In documented cases it has\n");
    write("been proven that instant death may occur.\n");
}

short() {
     return "A deck of many things";
}

query_value()
{
    return 100;
}

query_weight() {
     return 1;
}

shuffle(str) {
string name;
    name = call_other(this_player(), "query_name", 0);
    if (!str) { return 0; }
    if (str == "cards" || str == "deck") {
    write("Be sure to shuffle them well.\n");
    say(name+ " shuffles a deck of cards.\n");
    return 1;
    }
}

init() {
    add_action("draw"); add_verb("draw");
    add_action("shuffle"); add_verb("shuffle");
    add_action("no_find"); add_verb("origin");
}

no_find(str) {
   if (str == "cards" || str == "card" || str == "card from deck" || str == "deck of many things" || str == "deck" || str == "from deck") {
   write("What?\n");
   return 1;
   }
}

draw(str) {
    object card;
    object thisplayer;
    object sickle;
    object money;
    object level;
    object align;
    int n;
    string place;

    if (!id(str)) {
    write("Draw from what?\n");
    return 1;
    }
                thisplayer = this_player();
                place = environment(thisplayer);
                money = call_other(this_player(), "query_money", 0);
                level = call_other(this_player(), "query_level", 0);
                align = call_other(this_player(), "query_alignment", 0);
                if (level<5) {
                write("NO! You don't have the constitution for that!\n");
                write("A Djinni snatches the cards from you and disappears.\n");
                destruct(this_object());
                return 1;
                }
                n = random(15);
                write("You you draw a card from the deck.\n");
                say(call_other(this_player(), "query_name") + " draws a card from the deck of many things.\n");
                if (n == 0) {
                write("You draw the Sun: You receive 2 scrolls of lighting and 3 scrolls of chill.\n");
                write("The cards fade away.\n");
                card = clone_object("players/crimson/.scroll");
                move_object(card, thisplayer);
                card = clone_object("players/crimson/.scroll");
                move_object(card, thisplayer);
                card = clone_object("players/crimson/.scroll");
                move_object(card, thisplayer);
                card = clone_object("players/crimson/.lighting");
                move_object(card, thisplayer);
                card = clone_object("players/crimson/.lighting");
                move_object(card, thisplayer);
                destruct(this_object());
                return 1;
                }
                if (n == 1) {
                write("You draw the Fool: You loose experience points.\n");
                call_other(thisplayer, "add_exp", -(200 * (call_other(this_player(), "query_level", 0))/ 40));
                return 1;
                }
                if (n == 2) {
                write("You draw the Donjon: You are imprisoned.  Don't ask for wiz help. You need to quit.\n");
                write("The cards fade away.\n");
                move_object(thisplayer, "players/crimson/prison");
                destruct(this_object());
                 return 1;
                }
                if (n == 3) {
                write("You draw the Jester: You gain 10,000 eps.\n");
                call_other(thisplayer, "add_exp", 10000);
                return 1;
                }
                if (n == 4) {
                write("You draw the Knight: You gain the service of a Djinni in one battle.\n");
                write("The cards fade away.\n");
                card = clone_object("players/crimson/.djinni");
                move_object(card, thisplayer);
                destruct(this_object());
                return 1;
                }
                if (n == 5) {
                write("You draw the Skull: You summon Death.\n");
                write("The cards fade away.\n");
                card = clone_object("obj/monster");
                call_other(card, "set_name", "death");
                call_other(card, "set_short", "Death");
                call_other(card, "set_level", 19);
                call_other(card, "set_alias", "reaper");
                call_other(card, "set_wc", 30);
                call_other(card, "set_ac", 12);
                call_other(card, "set_eps", 1000000);
                call_other(card, "set_chance", 50);
                call_other(card, "set_spell_mess1", "The reaper's sickle glows brightly.\n");
                call_other(card, "set_spell_mess2", "The reaper's sickle slices into your veins.\n");
                call_other(card, "set_spell_dam", 40);
                call_other(card, "set_whimpy", 1);
                call_other(card, "set_aggressive", 1);
                move_object(card, place);
                call_other(card, "attack_object", thisplayer);
                sickle = clone_object("obj/weapon");
                call_other(sickle, "set_name", "sickle");
                call_other(sickle, "set_alias", "Reaper's sickle");
                call_other(sickle, "set_short", "The Reaper's Sickle");
                call_other(sickle, "set_long", "An extremely sharp weapon.\n");
                call_other(sickle, "set_class", 17);
                call_other(sickle, "set_weight", 10);
                call_other(sickle, "set_value", 4000);
                move_object(sickle, card);
                call_other(card, "attack_object", thisplayer);
                destruct(this_object());
                return 1;
                }
                if (n == 6) {
                write("You draw the Gem: You gain 5 gems.\n");
                card = clone_object("obj/treasure");
                call_other(card, "set_id", "emerald");
                call_other(card, "set_short", "An emerald");
                call_other(card, "set_weight", 1);
                call_other(card, "set_value", 500);
                call_other(card, "set_long", "A tear-cut emerald.\n");
                move_object(card, thisplayer);
                card = clone_object("obj/treasure");
                call_other(card, "set_id", "ruby");
                call_other(card, "set_short", "A ruby");
                call_other(card, "set_weight", 1);
                call_other(card, "set_value", 500);
                call_other(card, "set_long", "A heart-cut ruby.\n");
                move_object(card, thisplayer);
                card = clone_object("obj/treasure");
                call_other(card, "set_id", "diamond");
                call_other(card, "set_short", "A diamond");
                call_other(card, "set_long", "An oval cut diamond.\n");
                call_other(card, "set_weight", 1);
                call_other(card, "set_value", 1000);
                move_object(card, thisplayer);
                card = clone_object("obj/treasure");
                call_other(card, "set_id", "sapphire");
                call_other(card, "set_short", "A sapphire");
                call_other(card, "set_long", "A tear-cut sapphire.\n");
                call_other(card, "set_weight", 1);
                call_other(card, "set_value", 600);
                move_object(card, thisplayer);
                card = clone_object("obj/treasure");
                call_other(card, "set_id", "opal");
                call_other(card, "set_short", "A black opal");
                call_other(card, "set_long", "A smooth black opal with golden flecks.\n");
                call_other(card, "set_weight", 1);
                call_other(card, "set_value", 600);
                move_object(card, thisplayer);
                return 1;
                }
                if (n == 7) {
                write("You draw the Ruin: All of your money disappears.\n");
                call_other(this_player(), "add_money", -(money));
                return 1;
                }
                if (n == 8) {
                write("You draw the Balance: You are now neutal.\n");
                call_other(this_player(), "add_alignment", -(align));
                return 1;
                }
                if (n == 9) {
                write("You draw the Fates: You gain a heal spell.\n");
                card = clone_object("players/morgar/spells/heal");
                move_object(card, thisplayer);
                return 1;
                }
                if (n == 10) {
                write("You draw the Talons: You feel your magical essence draining away.\n");
                call_other(this_player(), "add_spell_point", (-1000));
                return 1;
                }
                if (n == 11) {
                write("You draw the Flames: You now have an enemy from another plane.\n");
                card = clone_object("players/crimson/flames");
                move_object(card, place);
                call_other(thisplayer, "attack_object", card);
                return 1;
                }
                if (n == 12) {
                write("You draw the Key: You get the wizard's eye and a magical sword.\n");
                write("The cards fade away.\n");
                card = clone_object("players/crimson/.crystal");
                move_object(card, thisplayer);
                card = clone_object("players/crimson/manual");
                move_object(card, thisplayer);
                card = clone_object("obj/weapon");
                call_other(card, "set_name", "sword");
                call_other(card, "set_alias", "moon sword");
                call_other(card, "set_short", "A Moon sword");
                call_other(card, "set_class", 21);
                call_other(card, "set_long", "You see a moonstone in the hilt of the sword.\n");
                call_other(card, "set_weight", 1);
                call_other(card, "set_value", 6000);
                move_object(card, thisplayer);
                destruct(this_object());
                return 1;
                }
                if (n == 13) {
                write("You draw The Void: You die.\n");
                write("The cards fade away.\n");
                call_other(thisplayer, "hit_player", 100000);
                destruct(this_object());
                return 1;
                }
                if (n == 14) {
                write("You draw the Moon: This consists of several things.\n");
                write("They are:    The manual of the crystal.\n");
                write("        :    A crystal.\n");
                write("        :    A lamp.\n");
                write("        :    An amulet.\n");
                write("        :    A greystone.\n");
                write("        :    A heal spell.\n");
                write("        :    Seven scrolls of lighting.\n");
                write("        :    10000 coins.\n");
                write("The cards fade away.\n");
                card = clone_object("obj/money");
                call_other(card, "set_money", 10000);
                move_object(card, thisplayer);
                card = clone_object("players/crimson/.lighting");
                move_object(card, thisplayer);
                card = clone_object("players/crimson/.lighting");
                move_object(card, thisplayer);
                card = clone_object("players/crimson/.lighting");
                move_object(card, thisplayer);
                card = clone_object("players/crimson/.lighting");
                move_object(card, thisplayer);
                card = clone_object("players/crimson/.lighting");
                move_object(card, thisplayer);
                card = clone_object("players/crimson/.lighting");
                move_object(card, thisplayer);
               card = clone_object("players/crimson/.lighting");
                move_object(card, thisplayer);
                card = clone_object("players/morgar/spells/heal");
                move_object(card, thisplayer);
                card = clone_object("players/morgar/stone");
                move_object(card, thisplayer);
                card = clone_object("players/morgar/amulet");
                move_object(card, thisplayer);
                card = clone_object("players/crimson/.djinni");
                move_object(card, thisplayer);
                card = clone_object("players/crimson/.crystal");
                move_object(card, thisplayer);
                card = clone_object("players/crimson/manual");
                move_object(card, thisplayer);
                destruct(this_object());
                return 1;
                }
                charges = charges -1;
                if (charges == 1) {
                destruct(this_object());
                    tell_object(thisplayer, "The cards fade away.\n");
                 return 1;
                }
}

get() {
string nono;
       nono = call_other(this_player(), "query_level", 0);
       if (nono<5) {
       return 0;
       } else
       return 1;
}

query_magic() { return magic; }
