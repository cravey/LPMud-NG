string name, alias, short_desc, long_desc, value, weight;
string type;
int worn, ac, charges;
object worn_by;
object next;

reset(arg)
{
    if(arg)
	return;
    type = "magical armour";
}

link(ob)
{
    next = ob;
}

remove_link(str)
{
    object ob;

    if (str == name) {
	ob = next;
	next = 0;
	return ob;
    }
    if (next)
	next = call_other(next, "remove_link", str);
    return this_object();
}

init() {
    add_action("wear"); add_verb("wear");
    add_action("remove"); add_verb("remove");
    set_name("magic shield");
    set_alias("shield");
    set_ac(20);
}

start_up() {
    wear(name);
    charges=10;
}

set_charges(c) { charges = c; }

query_charges() { return charges; }

dispell() {
    tell_object(environment(this_object()),"Magical shield dissipates!\n");
    remove(name);
    
}


rec_short()
{
    if(next)
	return name + ", " + call_other(next, "rec_short");
    return name;
}

short() {
    if (worn)
	return short_desc + " (worn)";
    return short_desc;
}

long(str) {
    write(long_desc);
}

id(str)
{
    return str == name || str == alias || str == type;
}

test_type(str)
{
    if(str == type)
	return this_object();
    if(next)
	return call_other(next, "test_type", str);
    return 0;
}

tot_ac()
{
    if(next)
	return ac + call_other(next, "tot_ac");
    return ac;
}

drop() {
      return 1;
}

query_type() { return type; }

query_value() { return value; }

query_worn() { return worn; }

query_name() { return name; }

armour_class() { return ac; }

wear(str)
{
    object ob;

    if (!id(str))
	return 0;
    if (environment() != this_player()) {
	write("You must get it first!\n");
	return 1;
    }
    if (worn) {
	write("You already wear it!\n");
	return 1;
    }
    next = 0;
    ob = call_other(this_player(), "wear", this_object());
    if(!ob) {
	worn_by = this_player();
	worn = 1;
	return 1;
    }
    write("You already have an armour of class " + type + ".\n");
write("Worn armour " + call_other(ob,"short") + ".\n");
    return 1;
}

get() { return 1; }

remove(str) {
    if (!id(str))
	return 0;
    if (worn) {
        call_other(worn_by, "stop_wearing",name);
    }
    worn_by = 0;
    worn = 0;
    destruct(this_object());
    return 1;
}

query_weight() { return weight; }

set_name(n) { name = n; set_short(n);}
set_short(s) { short_desc = s; long_desc = s + ".\n"; }
set_value(v) { value = v; }
set_weight(w) { weight = w; }
set_ac(a) { ac = a; }
set_alias(a) { alias = a; }
set_long(l) { long_desc = l; }
set_type(t) { type = t; }
set_arm_light(l) { set_light(l); }
