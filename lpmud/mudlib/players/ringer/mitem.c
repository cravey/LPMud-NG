/*
 * This is a generic valuable object. Clone a copy, and
 * setup local values.
 */

string short_desc, long_desc;
int value, local_weight, c_p_g;
string name, alias_name, alias2;
string read_msg, real_name, wear_msg;

query_magic() { return 1;}

query_id() { return real_name; }

set_real_id(n) { real_name = n; }

set_alias2(n) { alias2=n; }

id(str)
{
    return str == name || str == alias_name || str == alias2;
}

short() {
    return short_desc;
}

long() {
    write(long_desc);
}

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

set_wear(str) {
    wear_msg = str;
}

get() {
    return 1;
}

set_can_put_and_get(n) { c_p_g = n; }

can_put_and_get() {
    return c_p_g;
}

query_weight() {
    return local_weight;
}

init() {
    if (!read_msg)
	return;
    add_action("read"); add_verb("read");
    add_action("wear"); add_verb("wear");
}

read(str) {
    if (!id(str))
	return 0;
    write(read_msg);
    return 1;
}

wear(str) {
    if (!id(str))
	return 0;
    write(wear_msg);
    return 1;
}
