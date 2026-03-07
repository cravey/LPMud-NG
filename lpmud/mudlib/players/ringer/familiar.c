/*
 * A familiar monster. Clone this object,
 * and call local functions to configure it.
 */
#include "../../obj/living.h"
/*---Top-----------------------------------beat is always started in living.h when we are attacked.
/*
 * The heart beat is always started in living.h when we are attacked.
 */

string short_desc, long_desc, alias, fam_mess;
int move_at_reset, aggressive, controled;
object think_ob, kill_ob, control_ob;
object myself;		/* Ourselfs. */

int spell_chance, spell_dam,follow;
string spell_mess1, spell_mess2;

add_money(m) {
  money += m;    
}

id(str) { return str==name || str==alias || str=="familiar";}

valid_attack(ob) 
{
   return 1;
}

reset(arg)
{
    if (arg) {
	if (move_at_reset)
	    random_move();
	return;
    }
    follow=0;
    myself = this_object();
    is_npc = 0;
    enable_commands();
    add_action("go_north"); add_verb("n");
    add_action("go_south"); add_verb("s");
    add_action("go_west"); add_verb("w");
    add_action("go_east"); add_verb("e");
    add_action("go_northwest"); add_verb("nw");
    add_action("go_northeast"); add_verb("ne");
    add_action("go_southwest"); add_verb("sw");
    add_action("go_southeast"); add_verb("se");
    add_action("go_up"); add_verb("u");
    add_action("go_down"); add_verb("d");
    add_action("score"); add_verb("score");
    add_action("give_object"); add_verb("give");
    add_action("kill"); add_verb("kill");
    add_action("communicate"); add_verb("say");
    add_action("put"); add_verb("put");
    add_action("pick_up"); add_verb("get");
    add_action("drop_thing"); add_verb("drop");
    add_action("inventory"); add_verb("i");
    add_action("look"); add_verb("look");
    add_action("examine"); add_verb("examine");
    add_action("examine"); add_verb("exa");
    add_action("whisper"); add_verb("whisper");
    add_action("converse"); add_verb("converse");
    add_action("toggle_whimpy"); add_verb("wimpy");
    add_action("stop_hunting_mode"); add_verb("stop");
    add_action("spell_missile"); add_verb("missile");
    add_action("spell_shock"); add_verb("shock");
    add_action("spell_fire_ball"); add_verb("fireball");
    add_action("emote"); add_verb("emote");
}

score() {
    int intox_level;
    
    writef("You have " + experience + " experience points, " +
	  money + " gold coins,\n");
    writef(hit_point + " hit points (of " + max_hp + "), and ");
    writef(spell_points + " spell points.\nYou are " + short() + " (level " + level + ").\n");
    if (hunter && call_other(hunter, "query_name", 0))
        writef("You are hunted by " + call_other(hunter, "query_name", 0) + ".\n");
    if (whimpy)
	write("Wimpy mode.\n");
    show_age(); write("\n");
    return 1;
}


random_move()
{
    int n;
    n = random(4);
    if (n == 0)
	command("west");
    else if (n == 1)
	command("east");
    else if (n == 2)
	command("south");
    else if (n == 3)
	command("north");
}

short() {
    return short_desc;
}

long() {
    write (long_desc);
    if (controled) write("There is an interesting gleam in its eyes.\n");
}


heart_beat()
{
    age += 1;
    if (kill_ob && present(kill_ob, environment(this_object()))) {
	if (random(2) == 1)
	    return;		/* Delay attack some */
	attack_object(kill_ob);
	kill_ob = 0;
	return;
    }
    if (attacker_ob && present(attacker_ob, environment(this_object())) &&
      spell_chance > random(100)) {
	say(spell_mess1 + "\n", attacker_ob);
	tell_object(this_object(), spell_mess1 + "\n");
	tell_object(attacker_ob, spell_mess2 + "\n");
	call_other(attacker_ob, "hit_player", random(spell_dam));
    }
    attack();
    if (attacker_ob && whimpy && hit_point < max_hp/5)
	run_away();
}

can_put_and_get(str)
{
    if (!str)
	return 0;
    return 1;
}

catch_tell(str) {
    string who,s,mod,s1;

    if (controled) {
        tell_object(environment(control_ob),"% "+str);
    }
    if (control_ob) {
        if (sscanf(str,"%s says: %s %s\n",who,s1,s)==3) {
            if (environment(control_ob)==present(lower_case(who))) 
              if (id(s1)) {
                  say(name+" nods and smiles.\n");
                  command(s);
              }
        }
        if (sscanf(str,"%s tells you: %s\n",who,s)==2) {
            if (environment(control_ob)==find_living(lower_case(who))) {
                say(name+" nods and smiles.\n");
                command(s);
            }
        }
        if (sscanf(str,"%s says:%sfollow%s",who,mod,s)==3) {
            if (environment(control_ob)==present(lower_case(who))) {
                say(name+" nods and smiles.\n");
                if (sscanf(mod,"%sdon't%s",s,s1) == 2
                    || sscanf(mod,"%sstop%s",s,s1) == 2 
                    || sscanf(mod,"%sdo not%s",s,s1) == 2)
                  follow=0;
                else
                  follow=1;
            }
        }
        if (follow && sscanf(str,"%s leaves %s.\n",who,s)==2) {
            if (environment(control_ob)==present(lower_case(who))) {
                if (sscanf(s,"%sing%s",s1,mod)==2) s = s1;
                command(s);
            }
        }
    }
    if (attacker_ob)
      return;
    if (aggressive == 1 && sscanf(str, "%s arrives", who) == 1) {
	kill_ob = this_player();
	set_heart_beat(1);
    }
}

/*
 * Call the following functions to setup the monster.
 * Call them in the order they appear.
 */

set_name(n) {
    name = n;
    alignment = 0;		/* Neutral monster */
    cap_name = capitalize(n);
    short_desc = cap_name;
    long_desc = "You see nothing special.\n";
}

query_name() {  return name; }

/* Optional */
set_alias(a) {
    alias = a;
}

set_level(l) {
    level = l;
    weapon_class = WEAPON_CLASS_OF_HANDS;
    armor_class = 0;
    hit_point = 50 + (level - 1) * 8;	/* Same as a player */
    max_hp = hit_point;
    experience = call_other("room/adv_guild", "query_cost", l-1);
}

/* optional */
set_hp(hp) { max_hp = hp; hit_point = hp; }
/* optional */
set_ep(ep) { experience = ep; }
/* optional */
set_al(al) { alignment = al; }
/* optional */
set_short(sh) { short_desc = sh; long_desc = short_desc + "\n";}
/* optional */
set_long(lo) { long_desc = lo; }
/* optional */
set_wc(wc) { weapon_class = wc; }
/* optional */
set_ac(ac) { armor_class = ac; }
/* optional */
set_move_at_reset() { move_at_reset = 1; }
/* optional
 * 0: Peaceful.
 * 1: Attack on sight.
 */
set_aggressive(a) {
    aggressive = a;
}
/*
 * Now some functions for setting up spells !
 */
/*
 * The percent chance of casting a spell.
 */
set_chance(c) {
    spell_chance = c;
}
/* Message to the victim. */
set_spell_mess1(m) {
    spell_mess1 = m;
}
set_spell_mess2(m) {
    spell_mess2 = m;
}
set_spell_dam(d) {
    spell_dam = d;
}

/* Set the frog curse. */
set_frog() {
    frog = 1;
}

set_control_ob(ob) { control_ob = ob; controled = 0;}

set_control_mode(n) { 
    controled = n; 
    if (n==1) say(name + " gets a new look in its eyes!\n");
    if (n==0) say("The gleam seems fade from " + name +"'s eyes\n");
}

set_familiar_mess(str) { fam_mess = str; }

query_familiar_mess() { return fam_mess; }

/* Set the whimpy mode */
set_whimpy() {
    whimpy = 1;
}

/*
 * Force the monster to do a command. The force_us() function isn't
 * always good, because it checks the level of the caller, and this function
 * can be called by a room.
 */
init_command(cmd) {
    command(cmd);
    if (think_ob) 
      if (random(100) > 97) 
        call_other(think_ob,"think",this_object()); 
    return 1;
}


string it;		/* Last thing referenced. */


go_north() {
    command("north");
    return 1;
}

go_south() {
    command("south");
    return 1;
}

go_east() {
    command("east");
    return 1;
}

go_west() {
    command("west");
    return 1;
}

go_northwest() {
    command("northwest");
    return 1;
}

go_northeast() {
    command("northeast");
    return 1;
}

go_southwest() {
    command("southwest");
    return 1;
}

go_southeast() {
    command("southeast");
    return 1;
}

go_up() {
    command("up");
    return 1;
}

go_down() {
    command("down");
    return 1;
}

kill(str) {
    object ob;
    if (ghost)
	return 0;
    if (!str) {
        writef("Kill what ?\n");
	return 1;
    }
    ob = present(lower_case(str), environment(this_player()));
    if (!ob) {
	writef("No " + str + " here !\n");
	return 1;
    }
    if (!living(ob)) {
	writef(str + " is not a living thing !\n");
	say(cap_name + " tries foolishly to attack " + str + ".\n");
	return 1;
    }
    if (ob == this_object()) {
	writef("What ? Attack yourself ?\n");
	return 1;
    }
    if (attacker_ob == ob) {
	writef("Yes, yes.\n");
	return 1;
    }
    it = str;
    attack_object(ob);
    return 1;
}

communicate(str) {
    if (!str) {
	writef("Say what ?\n");
	return 1;
    }
    writef("Ok.\n");
    if (ghost) {
	say(short() + "says: " + str + ".\n");
	return 1;
    }
    say(cap_name + " says: " + str + "\n");
    return 1;
}


test_dark() {
    return 0;
}

put(str) {
    int i;
    string item;
    string container;
    object item_o;
    object container_o;

    if (!str)
	return 0;
    if (test_dark())
	return 1;
    if (sscanf(str, "%s in %s", item, container) != 2) {
	writef("put what ?\n");
	return 1;
    }
    container = lower_case(container);
    container_o = present(container, this_player());
    if (!container_o)
	container_o = present(container, environment(this_player()));
    if (!container_o) {
	writef("There are no " + container + "s here!\n");
	return 1;
    }
    if (!call_other(container_o, "can_put_and_get", 0)) {
	writef("You can't do that.\n");
	return 1;
    }
    item = lower_case(item);
    item_o = present(item, this_player());
    if (!item_o) {
	writef("You have no " + item + "!\n");
	return 1;
    }
    if (item_o == container_o)
	return 0;
    if (call_other(item_o, "prevent_insert"))
	return 1;
    if (call_other(item_o, "drop"))
	return 1;
    i = call_other(item_o, "query_weight");
    if (call_other(container_o, "add_weight", i)) {
	/* Remove the weight from the previous container. */
	call_other(environment(item_o), "add_weight", -i);
	move_object(item_o, container_o);
	say(cap_name + " puts the " + item + " in the " + container + ".\n");
	writef("Ok.\n");
	it = item;
	return 1;
    }
    writef("There is not room for more.\n");
    return 1;
}

pick_up(str) {
    string item;
    string container;
    object item_o;
    object container_o;

    if (!str) {
	writef("What ?\n");
	return 1;
    }
    if (ghost) {
	writef("You fail.\n");
	return 1;
    }
    if (test_dark())
	return 1;
    if (str == "all") {
	get_all(environment());
	return 1;
    }
    if (sscanf(str, "%s from %s", item, container) != 2) {
	pick_item(str);
	return 1;
    }
    container_o = present(lower_case(container));
    if (!container_o) {
	writef("There is no " + container + " here.\n");
	return 1;
    }
    if (!call_other(container_o, "can_put_and_get", 0)) {
	writef("You can't do that!\n");
	return 1;
    }
    item_o = first_inventory(container_o);
    while(item_o) {
	if (call_other(item_o, "id", item)) {
	    int weight;
            if (!call_other(item_o, "get",0)) {
                writef("You can not take " + item + " from " +
                       container + ".\n");
                return 1;
            }
	    weight = call_other(item_o, "query_weight", 0);
	    if (!add_weight(weight)) {
		writef("You can not carry more.\n");
		return 1;
	    }
	    call_other(container_o, "add_weight", -weight);
	    move_object(item_o, myself);
	    it = item;
	    writef("Ok.\n");
	    say(cap_name + " takes " + item + " from " + container + ".\n");
	    return 1;
	}
	item_o = next_inventory(item_o);
    }
    writef("There is no " + item + " in the " + container + ".\n");
    return 1;
}

pick_item(obj) {
    object ob;
    int i;
    obj = lower_case(obj);
    ob = present(obj, environment(this_player()));
    if (!ob) {
	writef("That is not here.\n");
	return 1;
    }
    if (ghost) {
	writef("You fail.\n");
	return 1;
    }
    if (environment(ob) == myself) {
	writef("You already have it!\n");
	return 1;
    }
    
    if (call_other(ob, "get", 0) == 0) {
        writef("You can not take that!\n");
        return 1;
    }
    i = call_other(ob, "query_weight", 0);
    if (add_weight(i)) {
	move_object(ob, myself);
	say(cap_name + " takes " + obj + ".\n");
	it = obj;
	writef("Ok.\n");
	return 1;
    }
    writef("You can't carry that much.\n");
    return 1;
}

drop_thing(obj) {
    string tmp;
    string tmp2;
    int i;
    if (!obj) {
	writef("What ?\n");
	return 1;
    }
    if (obj == "all") {
	drop_all(1);
	return 1;
    }
    if (sscanf(obj, "%s in %s", tmp, tmp2) == 2) {
	put(obj);
	return 1;
    }
    if (obj == "money" || obj == "coins") {
        if (money == 0) { writef("You have no money!\n"); return 1;}
        say(cap_name + " drops ");
        say(money);
        say(" coins.\n");
        writef("You drop "); writef(money); writef(" coins.\n");
        drop_all_money();
        return 1;
    }
    tmp = obj;
    obj = present(lower_case(obj), this_player());
    if (!obj) {
	writef("That is not here.\n");
	return 1;
    }
    if (drop_one_item(obj)) {
	it = tmp;
	writef("Ok.\n");
	say(cap_name + " drops the " + tmp + ".\n");
    }
    return 1;
}

query_weight() { return 80; }

add_weight(w) {
    int max;

    max = level + 10;
    if (frog)
	max = max / 2;
    if (w + local_weight > max)
	return 0;
    local_weight += w;
    return 1;
}


inventory() {
    object ob;
    if (test_dark())
	return 1;
    ob = first_inventory(myself);
    while(ob) {
	string str;
	str = call_other(ob, "short", 0);
	if (str) {
	    writef(str + ".\n");
	    it = str;
	}
	ob = next_inventory(ob);
    }
    return 1;
}


examine(str) {
    object ob;
    string item;
    int max;
    if (test_dark())
	return 1;
    if (!str) {
        write("%Examine what?\n");
        return 1;
    }
    item = lower_case(str);
    ob = present(item, this_player());
    if (!ob && call_other(environment(this_player()), "id", item))
        ob = environment(this_player());
    if (!ob)
        ob = present(item, environment(this_player()));
    if (!ob) {
        write("%There is no " + item + " here.\n");
        return 1;
    }
    it = item;
    call_other(ob, "long", item);
    if (!call_other(ob, "can_put_and_get", item))
        return 1;
    ob = first_inventory(ob);
    if (ob)
        write("%\t" + item + " contains:\n");
    max = MAX_LIST;
    while(ob && max > 0) {
        string sh;
        sh = call_other(ob, "short", 0);
        if (sh)
            write("%" + sh + ".\n");
        ob = next_inventory(ob);
        max -= 1;
    }
    return 1;
}

look(str) {
    object ob;
    string item;
    int max;
    if (test_dark())
	return 1;
    if (!str) {
	call_other(environment(myself), "long", 0);
	ob = first_inventory(environment(myself));
	max = MAX_LIST;
	while(ob && max > 0) {
	    if (ob != myself) {
		string short_str;
		short_str = call_other(ob, "short", 0);
		if (short_str) {
		    max -= 1;
		    write("%" + short_str + ".\n");
		    it = short_str;
		}
	    }
	    ob = next_inventory(ob);
	}
	return 1;
    }
    if (sscanf(str, "at %s", item) == 1 || sscanf(str, "in %s", item) == 1) {
	item = lower_case(item);
	ob = present(item, myself);
	if (!ob && call_other(environment(myself), "id", item))
	    ob = environment(myself);
	if (!ob)
	    ob = present(item, environment(myself));
	if (!ob) {
	    write("%There is no " + item + " here.\n");
	    return 1;
	}
	it = item;
	call_other(ob, "long", item);
	if (!call_other(ob, "can_put_and_get", item))
	    return 1;
	ob = first_inventory(ob);
	if (ob)
	    write("%\t" + item + " contains:\n");
	max = MAX_LIST;
	while(ob && max > 0) {
	    string sh;
	    sh = call_other(ob, "short", 0);
	    if (sh)
		write("%"+ sh + ".\n");
	    ob = next_inventory(ob);
	    max -= 1;
	}
	return 1;
    }
    writef("Look AT something, or what ?\n");
    return 1;
}


whisper(str)
{
    object ob;
    string who;
    string msg;
    if (ghost) {
	writef("You fail.\n");
	return 1;
    }
    if (!str || sscanf(str, "%s %s", who, msg) != 2) {
	writef("Whisper what ?\n");
	return 1;
    }
    it = lower_case(who);
    ob = find_living(it);
    if (!ob || !present(it, environment(this_player()))) {
	writef("No player with that name in this room.\n");
	return 1;
    }
    tell_object(ob, cap_name + " whispers to you: " + msg + "\n");
    writef("Ok.\n");
    say(cap_name + " whispers something to " + who + ".\n", ob);
    return 1;
}

/*
 * This routine is called from other routines to drop one specified object.
 * We return true if success.
 */

drop_one_item(ob)
{
    int weight;

    if (call_other(ob, "drop", 0))
	return 0;
    weight = call_other(ob, "query_weight", 0);
    if (!weight)
	weight = 0;
    add_weight(-weight);
    move_object(ob, environment(myself));
    return 1;
}

drop_all(verbose)
{
    object ob;
    object next_ob;
    if (!myself || !living(myself))
        return;
    ob = first_inventory(myself);
    while(ob) {
	string out;
	next_ob = next_inventory(ob);
	it = call_other(ob, "short", 0);
	if (drop_one_item(ob) && verbose) {
	    out = it + ".\n";
	    say(cap_name + " drops " + out);
	    tell_object(myself, "drop: " + out);
	}
	ob = next_ob;
    }
}

converse()
{
    writef("Give '**' to stop.\n");
    input_to("converse_more");
    return 1;
}

converse_more(str)
{
    if (!str) {
	input_to("converse_more");
	return;
    }
    if (str == "**") {
	writef("Ok.\n");
	return;
    }
    say(cap_name + " says: " + str + "\n");
    input_to("converse_more");
}

toggle_whimpy()
{
    whimpy = !whimpy;
    if (whimpy)
        writef("Wimpy mode.\n");
    else
        writef("Brave mode.\n");
    return 1;
}

stop_hunting_mode()
{
    if (!hunted) {
        writef("You are not hunting anyone.\n");
	return 1;
    }
    call_other(hunted, "stop_hunter");
    hunted = 0;
    writef("Ok.\n");
    return 1;
}

spell_missile(str)
{
    object ob;
    if (test_dark())
	return 1;
    if (level < 5)
	return 0;
    if (!str)
	ob = attacker_ob;
    else
	ob = present(lower_case(str), environment(this_player()));
    if (!ob || !living(ob)) {
	writef("At whom ?\n");
	return 1;
    }
    if (ob == myself) {
	writef("What ?");
	return 1;
    }
    missile_object(ob);
    return 1;
}

spell_shock(str)
{
    object ob;
    if (test_dark())
	return 1;
    if (level < 10)
	return 0;
    if (!str)
	ob = attacker_ob;
    else
	ob = present(lower_case(str), environment(this_player()));
    if (!ob || !living(ob)) {
	writef("At whom ?\n");
	return 1;
    }
    if (ob == myself) {
	writef("What ?");
	return 1;
    }
    shock_object(ob);
    return 1;
}

spell_fire_ball(str)
{
    object ob;
    if (test_dark())
	return 1;
    if (level < 15)
	return 0;
    if (!str)
	ob = attacker_ob;
    else
	ob = present(lower_case(str), environment(this_player()));
    if (!ob || !living(ob)) {
	writef("At whom ?\n");
	return 1;
    }
    if (ob == myself) {
	writef("What ?");
	return 1;
    }
    fire_ball_object(ob);
    return 1;
}

give_object(str)
{
    string item, dest;
    object item_ob, dest_ob, tmp;
    int weight;
    int coins;
    
    if (!str)
      return 0;
    if (test_dark())
      return 1;
    if (sscanf(str, "%d coins to %s", coins, dest) == 2)
      item = 0;
    else if ( sscanf(str, "1 coin to %s", dest) == 1)
      coins = 1;
    else if ( sscanf(str, "coin to %s", dest) == 1)
      coins = 1;
    else if (sscanf(str, "one coin to %s", dest) == 1)
      coins = 1;
    else if (sscanf(str, "%s to %s", item, dest) != 2) {
	writef("Give what to whom ?\n");
	return 1;
    }
    dest = lower_case(dest);
    if (item) {
	item = lower_case(item);
	item_ob = present(item, this_player());
	if (!item_ob) {
	    writef("There are no " + item + " here.\n");
	    return 1;
	}
	it = item;
	if (environment(item_ob) == this_object() &&
	    call_other(item_ob, "drop", 0) == 1) {
	    return 1;
	} else {
	    if (!call_other(item_ob, "get")) {
		writef("You can't get that !\n");
		return 1;
	    }
	}
    }
    dest_ob = present(dest, environment(this_player()));
    if (!dest_ob) {
	writef("There is no " + capitalize(dest) + " here.\n");
	return 1;
    }
    if (!living(dest_ob)) {
	writef("You can't do that.\n");
	return 1;
    }
    if (!item) {
	if (coins <= 0 && level < 20)
          return 0;
	if (money < coins) {
	    writef("You don't have that much money.\n");
	    return 1;
	}
	money -= coins;
	call_other(dest_ob, "add_money", coins);
	tell_object(dest_ob, cap_name + " gives you " + coins +
	    " gold coins.\n");
	writef("Ok.\n");
	return 1;
    }
    weight = call_other(item_ob, "query_weight", 0);
    if (!call_other(dest_ob, "add_weight", weight)) {
	writef(capitalize(dest) + " can't carry any more.\n");
	return 1;
    }
    add_weight(-weight);
    move_object(item_ob, dest_ob);
    say(cap_name + " gives " + item + " to " + capitalize(dest) + ".\n");
    writef("Ok.\n");
    return 1;
}


emote(str) {
    if (!str) {
	writef("emote what ?\n");
	return 1;
    }
    say(cap_name + " " + str + "\n");
    return 1;
}


/*
 * Get all items here.
 */
get_all(from)
{
    object ob, next_ob;
    ob = first_inventory(from);
    while(ob) {
	string item;
	next_ob = next_inventory(ob);
	item = call_other(ob, "short", 0);
        if (item && call_other(ob, "get", 0)) {
            int weight;
            weight = call_other(ob, "query_weight", 0);
            if (add_weight(weight)) {
                writef(item + ": Ok.\n");
                move_object(ob, this_object());
                say(cap_name + " takes: " + item + ".\n");
            } else {
                writef(item + ": Too heavy.\n");
            }
            it = item;
	}
	ob = next_ob;
    }
}

invis()
{
    if (is_invis) {
	writef("You are already invisible.\n");
	return 1;
    }
    is_invis = 1;
    writef("You are now invisible.\n");
    say(cap_name + " disappears.\n");
    cap_name = "Someone";
    return 1;
}

vis()
{
    if (!is_invis) {
	writef("You are not invisible.\n");
	return 1;
    }
    is_invis = 0;
    writef("You are now visible.\n");
    cap_name = capitalize(name);
    say(cap_name + " appears in puff of smoke.\n");
    return 1;
}

local_commands() {
    localcmd();
}

writef(str) {
    write(str);
    if (controled)
        tell_object(environment(control_ob),"% "+str);
    return 1;
}

set_think(ob) { think_ob = ob; }
