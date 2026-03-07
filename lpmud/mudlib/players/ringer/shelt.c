string short_desc, long_desc;
int value, local_weight;
string name, alias_name;
string read_msg;
string c_ob;

id(str)
{
    return str == name || str == alias_name || str == "scroll";
}

short() {
    return short_desc;
}

long() {
    write(long_desc);
}

query_id() { 
   short_desc = "a scroll of shelter"; 
   name = "scroll of shelter";
   return "scroll of shelter"; 
}

query_magic() { return 1; }

query_value() { return value; }

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

init() {
    if (!read_msg)
	return;
    add_action("read"); add_verb("read");
    add_action("shelter"); add_verb("shelter");
    c_ob = "players/ringer/mg_sh";
}

read(str) {
    if (str != name &&  str != alias_name)
	return 0;
    write(read_msg);
    return 1;
}

object shield;
shelter() {
    object ob;
    ob = clone_object(c_ob);
    move_object(ob,this_player());
    call_other(ob,"start_up");
    tell_object(this_player(), short() + " dissolves.\n");
    move_object(this_object(),"players/ringer/mg_void");
    set_heart_beat(1);
    shield = ob;
}


heart_beat() {
    int c;
    string st;
    c = call_other(shield,"query_charges");
    if (c>0) {
      call_other(shield,"set_charges",c-1);
      return 1;
    }
    call_other(shield,"dispell");
    destruct(this_object());
    return 1;
}
