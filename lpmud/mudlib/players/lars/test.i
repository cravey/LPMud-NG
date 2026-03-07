# 1 "players/lars/test.c"


inherit "room/room";

reset(arg) {
    if (arg) return;

    set_light(1);
    short_desc = "A room";
    no_castle_flag = 0;
    long_desc = 
        "Long desc.\n";
    dest_dir = 
        ({
        "/players/lars/workroom", "north",
        });
}

query_light() {
    return 1;
}
query_room_maker() {
    return 101;
}

















exit() {
    1/0;
}
