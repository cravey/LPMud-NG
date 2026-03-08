int ticks;

reset(arg) {
    if (arg)
        return;
    ticks = 0;
}

start_probe() {
    ticks = 0;
    set_heart_beat(1);
    return 1;
}

stop_probe() {
    set_heart_beat(0);
    return 1;
}

heart_beat() {
    ticks += 1;
}

query_ticks() { return ticks; }
