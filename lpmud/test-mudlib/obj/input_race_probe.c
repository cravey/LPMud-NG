arm() {
    input_to("race_cb", 1);
    destruct(this_object());
    return 1;
}

race_cb(str) {
    object ob;

    ob = this_player();
    if (ob)
        call_other(ob, "mark_race_probe_callback", str);
}
