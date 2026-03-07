/*
 */

string short_desc, long_desc;
int value, local_weight, c_p_g;
string name, alias_name, alias2;
string read_msg, real_name, wear_msg;
int raven_flag;

query_magic() { return 1;}

query_id() { short_desc = real_name; return real_name; }

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

get() {
    return 1;
}

add_weight() { return 1; }

can_put_and_get() {
    return 1;
}

query_weight() {
    return local_weight;
}

init() {
    enable_commands();
}

query_flag() { return raven_flag; }

catch_tell(str) {
    string item,cont;
    object i,c;
    if (sscanf(str, "Black raven puts the %s in the %s.", item,cont) == 2) {
        if (id(cont)) {
            c = this_object();
            i = present(item,c);
            if (i) {
                if (call_other(i,"id","silver ring") &&
                    call_other(c,"id","caldron")) {
                    raven_flag = 1;
                    say("A voice from the caldron says 'Emosfen'.\n");
                }
            }
        }
    }
}

