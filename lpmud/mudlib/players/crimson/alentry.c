int kablam;
int newboom;
int desc;
int move;
#define time_to_impact 10
#define time_to_boom 11
#define time_to_booma 12
#define time_to_boomb 13
#define time_to_shout 17
#define time_to_shoutt 20

short() { return "Alchemy Entry"; }

reset(arg) {
    set_light(1);
    extra_reset();
    desc = 0;
    move = 0;
    newboom = newboom -1;
    if (newboom<1) set_heart_beat(1);
}

heart_beat() {
     kablam += 1;
     if (kablam == time_to_impact) {
     shout("KABLAM!\n");
     desc = 1;
     move = 1;
     } else
     if (kablam == time_to_boom) {
     shout("RUMBLE\n");
     } else
     if (kablam == time_to_booma) {
     shout("Rumble\n");
     } else
     if (kablam == time_to_boomb) {
     shout("rumble\n");
     } else
     if (kablam == time_to_shout) {
     shout("Fanwar's junior alchemist shouts: The alchemy shop will be temporarily closed...\n");
     } else
     if (kablam == time_to_shoutt) {
     shout("Fanwar's junior alchemist shouts: due to ... er uhm... technical difficulties.\n");
     kablam = 0;
     set_heart_beat(0);
     }
}

extra_reset() {
     if (newboom<1) {
     newboom = 3;
     return 1;
     } else
     if (newboom>1) {
     return 0;
     }
}

long() {
     if (desc == 1) {
     write("You are in a small flower garden which was once the entryway to\n");
     write("Fanwar the elf's alchemist shop.  Despite the blast when the\n");
     write("shop blew up, this area was saved.\n");
     write("The only obvious exit is east.\n");
     return 1;
     } else
     write("You are in a small flower garden.  To the west you see a brightly\n");
     write("colored sign which reads:\n");
     write("               @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
     write("               @////////////////////\\\\\\\\\\\\\\\\\\\@\n");
     write("               @\\\\\\\\\\\\\\\\\\\\///////////////////@\n");
     write("               @///////////      YE OLD      \\\\\\\\\\@\n");
     write("               @\\\\\\\\\\\                  //////////@\n");
     write("               @///////////  ALCHEMY SHOPPE  \\\\\\\\\\@\n");
     write("               @\\\\\\\\\\\\\\\\\\\\///////////////////@\n");
     write("               @////////////////////\\\\\\\\\\\\\\\\\\\@\n");
     write("               @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
     write("               Obvious exits are north, east, and west.\n");
     return 1;
}

init() {
    add_action("west"); add_verb("west");
    add_action("east"); add_verb("east");
    add_action("north"); add_verb("north");
}

west(str) {
    if (desc == 1) {
    write("It would not be wise to go in there before it is cleaned up.\n");
    return 1;
    } else
    call_other(this_player(), "move_player", "west#players/crimson/alchemist");
    return 1;
}

east(str) {
    call_other(this_player(), "move_player", "east#room/city/sta1");
    return 1;
}

north(str) {
    call_other(this_player(), "move_player", "north#room/city/st13");
    return 1;
}

query_move() { return move; }
