string name;
string alias_name;
string short_desc;
string long_desc;
int local_weight;
int local_value;
int contained_weight;

reset(arg) {
    if (arg)
        return;
    name = "test_dummy";
    alias_name = "dummy";
    short_desc = "a test dummy";
    long_desc = "A generic object used by engine tests.\n";
    local_weight = 1;
    local_value = 42;
    contained_weight = 0;
}

set_name(str) { name = str; short_desc = str; }
set_alias(str) { alias_name = str; }
set_weight(w) { local_weight = w; }
set_value(v) { local_value = v; }

query_name() { return name; }
query_value() { return local_value; }
query_weight() { return local_weight + contained_weight; }

short() { return short_desc; }
long() { write(long_desc); }

id(str) { return str == name || str == alias_name; }

get() { return 1; }
drop() { return 0; }

can_put_and_get() { return 1; }
prevent_insert() { return 0; }

add_weight(w) {
    if (contained_weight + w < 0)
        return 0;
    contained_weight += w;
    return 1;
}

who_called_me() {
    return file_name(previous_object());
}
