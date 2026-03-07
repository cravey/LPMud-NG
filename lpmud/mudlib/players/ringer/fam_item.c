/*
 * This is a familiar controller object. Clone a copy, and
 * setup local values.
 */

string short_desc, long_desc;
int value, local_weight, trance;
string name, alias_name;
string read_msg;
object familiar, save_loc;

id(str)
{
    return str == name || str == alias_name || str == "fam_item";
}

short() {
    return short_desc;
}

long() {
    write(long_desc);
}

query_value() { return value; }

query_id() { 
    return "familiar stone";
}

set_id(str) {
    local_weight = 1;
    name = str;
}

set_alias(str) {
    alias_name = str;
}

set_short(str) {
    short_desc = str;
    long_desc = "You see nothing special.\n";
}

set_long(str) {
    long_desc = str;
}

set_value(v) {
    value = v;
}

set_weight(w) {
    local_weight = w;
}

set_read(str) {
    read_msg = str;
}

get() {
    return 1;
}

query_weight() {
    return local_weight;
}

query_magic() { return 1; }

init() {
    if (read_msg) {add_action("read"); add_verb("read");}
    add_action("gaze"); add_verb("gaze");
    add_action("brk"); add_verb("break");
    add_action("x"); add_verb("x");
}

read(str) {
    if (str != name &&  str != alias_name)
	return 0;
    write(read_msg);
    return 1;
}

gaze(str) {
    string s1,s2;
    if (!str) return 0;
    sscanf(str,"into %s",s1);
    sscanf(str,"at %s",s2);
    if (id(s1)) str=s1;
    if (id(s2)) str=s2;
    if (!present(this_object(),this_player())) 
      return 0;
    
    if (!familiar || !living(familiar)) {
        write("You have a strange feeling of frustration, then it passes.\n");
        return 1;
    }
    if (id(str) && !trance) {
        write("You gaze deeply into the " + name + "\n");
        write("You fall into a deep trance.  You feel like you are no longer in your own\n");
        write("body.  Your body disappears.  Your vision becomes blurred.\n");
        write(call_other(familiar,"query_familiar_mess") + "\n");
        write("It seems you are controlling some other creature!\n");
        write("To issue commands, preface them with 'x '\n");
        write("'Break trance' will bring you back to reality...\n");
        save_loc = environment(this_player());
        move_object(this_player(),"players/ringer/mg_void");
        call_other(familiar,"set_control_mode",1);
        trance=1;
        return 1;
    }
}

brk(str){
    if (str == "trance") {
        if (!trance) return 0;
        write("You return to reality.\n");
        move_object(this_player(),save_loc);
        call_other(familiar,"set_control_mode",0);
        trance = 0;
        return 1;
    }
    return 0;
}

x(str) {
    int c;
    string s,rest;
    object save;

    if (!str) return 0;
    if (!trance) return 0;
    if (!familiar) {
        write("It seems your familiar has died...\n");
        write("You return to reality\n");
        move_object(this_player(),save_loc);
        trance = 0;
        return 1;
    }
    if (str == "n" || str == "s" || str == "w" || str == "e"
      || str == "u" || str == "d" || str == "north" || str == "south"
      || str == "east" || str == "west" || str == "up" || str == "down"
      || str == "enter" || str == "exit") {
        c = call_other(familiar,"init_command",str); 
        save = environment(this_player());
        move_object(this_player(), familiar);
        call_other(familiar,"look");
        move_object(this_player(),save);
        return 1;
    }
    if (str == "l" || str == "look") {
        s = str;
        rest = 0;
    }
    else 
      sscanf(str,"%s %s",s,rest);
    if (s == "look" || s=="l") {
        save = environment(this_player());
        move_object(this_player(),familiar);
        c = call_other(familiar,"look",rest);
        move_object(this_player(),save);
    } else if (s == "examine" || s == "exa") {
        save = environment(this_player());
        move_object(this_player(),familiar);
        c = call_other(familiar,"examine",rest);
        move_object(this_player(),save);
    } else {
        save = environment(this_player());
        move_object(this_player(),familiar);
        c = call_other(familiar,"init_command",str);
        move_object(this_player(),save);
    }
    return c;
}

set_familiar(ob) {
    familiar = ob;
}
