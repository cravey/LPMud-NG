string name;
int local_weight;
int carried_weight;
int allow_put_get;
int deny_insert;
int allow_add_weight;

reset(arg) {
    if (arg)
        return;
    name = "transfer_gate";
    local_weight = 1;
    carried_weight = 0;
    allow_put_get = 1;
    deny_insert = 0;
    allow_add_weight = 1;
}

set_name(str) { name = str; return 1; }
set_weight(w) { local_weight = w; return 1; }
set_can_put_and_get(v) {
    if (v)
        allow_put_get = 1;
    else
        allow_put_get = 0;
    return 1;
}

set_prevent_insert(v) {
    if (v)
        deny_insert = 1;
    else
        deny_insert = 0;
    return 1;
}

set_allow_add_weight(v) {
    if (v)
        allow_add_weight = 1;
    else
        allow_add_weight = 0;
    return 1;
}

query_weight() { return local_weight + carried_weight; }
short() { return name; }
id(str) { return str == name || str == "transfer_gate"; }

get() { return 1; }
drop() { return 0; }
can_put_and_get() { return allow_put_get; }
prevent_insert() { return deny_insert; }

add_weight(w) {
    if (!allow_add_weight)
        return 0;
    if (carried_weight + w < 0)
        return 0;
    carried_weight += w;
    return 1;
}
