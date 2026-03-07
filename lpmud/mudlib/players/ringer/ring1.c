string name, alias, short_desc, long_desc, value, weight;
int worn, ac, charges;
object worn_by;

query_id() { return "ring of calling"; }

init() {
    alias = "ring";
    short_desc = name;
    long_desc = name + ".\n";
    value = 425;
    charges = 4;
    add_action("un_wear"); add_verb("remove");
    add_action("wear"); add_verb("wear");
    add_action("call"); add_verb("call");
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
    return str == name || str == alias || str == "ring of calling";
}

query_value()
{
    return value;
}

wear(str)
{
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
    worn_by = this_player();
    worn = 1;
    return 1;
}

get() { return 1; }

query_name() { return name; }

query_worn() { return worn; }

query_magic() { return 1;}

drop(silently) {
    if (worn) {
	worn = 0;
	worn_by = 0;
	if (!silently)
	    write("You drop your worn ring.\n");
    }
    return 0;
}

call(str) {
    object ob,room,old_room,other_ring,o1;
    if (!worn) return 0;
    if (charges <= 0) {write("Sorry, ring is worn out.\n"); return 1;}
    if (!str) {
        write("A soft voice from the ring echoes: Who?\n");
        return 1;
    }
    ob = find_living(str);
    if (ob) {
        other_ring = present("ring of calling",ob);
        if (other_ring) 
          if (call_other(other_ring,"query_worn")) {
              if (call_other(environment(ob),"realm")=="NT") {
                  write("A magical force frustrates your attempt.\n");
                  return 1;
              }
            value = value - 100;
            charges = charges - 1;
            write("The ring of calling glows blue.  Its light increases"
                 + " until its brilliance\npeaks in a blinding flash.\n");
            old_room = environment(this_player());
            room = environment(ob);
            say(call_other(ob,"query_name",0) 
                    + " appears in a flash of blue light.\n");
            move_object(this_player(), room);
            move_object(ob, old_room);
            tell_object(ob,"Your ring glows blue.  Its light envelopes you.\n"
                        + "Your surroundings suddenly change.\n");
            say(call_other(ob,"query_name",0) 
                    + " disappears in a flash of blue light.\n");
            move_object(this_player(), old_room);
            write(call_other(ob,"query_name",0) 
                    + " appears in a flash of blue light.\n");
            return 1;
        }
    } else write("Who is that?\n");
    return 0;
}


un_wear() {
    if (worn)
	worn = 0;
    worn_by = 0;
    return 1;
}

query_weight() { return 1; }

set_name(s) { name=s; }
set_short(s) { short_desc = s; long_desc = s + ".\n"; }
set_long(l) { long_desc = l; }


