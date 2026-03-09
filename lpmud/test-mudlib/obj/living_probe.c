string living_name;
int init_heartbeat;
int init_hits;
int hb_ticks;
int local_weight;
int carried_weight;

reset(arg) {
    if (arg)
        return;
    living_name = "living_probe";
    init_heartbeat = 0;
    init_hits = 0;
    hb_ticks = 0;
    local_weight = 1;
    carried_weight = 0;
    enable_commands();
    set_living_name(living_name);
    set_heart_beat(0);
}

short() { return "living probe"; }
id(str) { return str == living_name || str == "living_probe"; }

set_probe_name(str) {
    if (!str || str == "")
        return 0;
    living_name = str;
    set_living_name(living_name);
    return 1;
}

set_init_heartbeat(v) {
    if (v)
        init_heartbeat = 1;
    else
        init_heartbeat = 0;
    return 1;
}

init() {
    init_hits += 1;
    if (init_heartbeat)
        set_heart_beat(1);
}

heart_beat() {
    hb_ticks += 1;
}

query_init_hits() { return init_hits; }
query_hb_ticks() { return hb_ticks; }

query_weight() { return local_weight + carried_weight; }
get() { return 1; }
drop() { return 0; }
can_put_and_get() { return 1; }
prevent_insert() { return 0; }
add_weight(w) {
    if (carried_weight + w < 0)
        return 0;
    carried_weight += w;
    return 1;
}
