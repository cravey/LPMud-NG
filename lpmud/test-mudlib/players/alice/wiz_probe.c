int saved_value;

reset(arg) {
    if (arg)
        return;
    saved_value = 0;
}

set_value(v) {
    saved_value = v;
    return 1;
}

query_value() { return saved_value; }

attempt_save(path) {
    string trap;

    trap = catch(save_object(path));
    if (trap)
        return "ERR:" + trap;
    return "OK";
}

attempt_restore(path) {
    string trap;
    int ok;

    trap = catch(ok = restore_object(path));
    if (trap)
        return "ERR:" + trap;
    if (ok)
        return "OK";
    return "NO";
}
