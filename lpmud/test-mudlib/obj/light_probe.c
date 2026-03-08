int lit;

reset(arg) {
    if (arg)
        return;
    lit = 0;
}

short() { return "a light probe"; }
id(str) { return str == "light_probe" || str == "probe"; }
get() { return 1; }

enable_glow() {
    if (!lit) {
        set_light(1);
        lit = 1;
    }
    return set_light(0);
}

disable_glow() {
    if (lit) {
        set_light(-1);
        lit = 0;
    }
    return set_light(0);
}

query_lit() { return lit; }

query_total_light() { return set_light(0); }
