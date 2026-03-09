string name;

reset(arg) {
    if (arg)
        return;
    name = "self_destruct_weight";
}

short() { return "self-destructing weight probe"; }
id(str) { return str == name || str == "sd_weight_probe"; }
get() { return 1; }
drop() { return 0; }
can_put_and_get() { return 1; }
prevent_insert() { return 0; }
add_weight(w) { return 1; }

query_weight() {
    destruct(this_object());
    return 1;
}
