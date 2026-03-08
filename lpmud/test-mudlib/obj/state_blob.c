int saved_number;
string saved_text;

reset(arg) {
    if (arg)
        return;
    saved_number = 0;
    saved_text = "";
}

set_state(n, t) {
    saved_number = n;
    saved_text = t;
    return 1;
}

query_number() { return saved_number; }
query_text() { return saved_text; }

do_save(path) {
    return save_object(path);
}

do_restore(path) {
    return restore_object(path);
}
