int portal, click, cnt;

reset(arg)
{
    set_light(1);
    portal=0;
    click=0;
    set_heart_beat(0);
}

init()
{
    add_action("search"); add_verb("search");
    add_action("north"); add_verb("north");
    add_action("move"); add_verb("move");
    add_action("move"); add_verb("pull");
    add_action("move"); add_verb("push");
}

move(str) {
    if (str != "lever") return 0;
    write(
"You hear a loud rumbling sound.  The lever disappears.\n");
    write("You hear a loud click.  The ceiling lowers noticeably!\n");
    click = 1;
    cnt=0;
    set_heart_beat(1);
    return 1;
}

search(str) {
    if (!str) str = "room";
    if (str != "room")  return 0;
    write("You find nothing unusual.\n");
    say(call_other(this_player(),"query_name") + " searches the room.\n");
    return 1;
}

short() {
    return "Round portal room";
}

long() {
    write(
"You are in a large round room.  A single, large, circular slab of stone\n"+
"forms the ceiling of the room.  The walls are perfectly smooth and seem\n"+
"carefully polished.  There are few decorations on the walls, except a\n"+
"stylized painting on the north wall.  It is the painting of a magical\n"+
"portal.\n");
    if (click==0) 
      write("There is a lever on the south wall.\n");
    if (click >= 4 && click <= 10) 
      write("A portal is glowing brightly to the north!\n");
}

north() {
    string d;
    if (!portal) return 0;
    if (portal==1 || portal==2) 
      d = "players/ringer/pyramid/viperpit";
    if (portal==3) 
      d = "players/ringer/pyramid/pit1";
    if (portal==4 || portal==6) 
      d = "players/ringer/pyramid/demonpit";
    if (portal==5)
      d = "players/ringer/pyramid/xycrypt";
    if (portal == 7) 
      d = "players/ringer/dk_glen";
    if (portal == 8) 
      d = "players/ringer/garr";
    if (portal > 8)
      d = "players/ringer/desert5";
    call_other(this_player(),"move_player","enters the portal#"+d);
    return 1;
}

heart_beat() {
    if (cnt==0) { cnt=1; return; }
    cnt=0;
    click+=1;
    say("You hear a loud click.  The ceiling lowers noticeably!\n");
    if (click == 3) {
        say("The ceiling is now only 6 feet from the floor.\n");
        say("The painting glows brightly, becoming an actual portal!\n");
        portal=1;
    }
    if (click > 3) portal+=1; 
    if (click == 5) say("The ceiling is now only 5 feet from the floor.\n");
    if (click == 9) say("The ceiling is now only 3 feet from the floor.\n");
    if (click == 11) {
        say("The portal closes leaving only the painting.\n");
        portal=0;
    }
    if (click == 13) 
        smush();
}

smush() {
    object ob,obn;
    ob = first_inventory(this_object());
    say("Crunch!\n");
    while (ob) {
        obn = next_inventory(ob);
        if (living(ob) && !call_other(ob,"query_ghost")) {
            call_other(ob,"hit_player",1000);
        }
        ob = obn;
    }
    say("The ceiling recedes.  The floor tilts and everything is jetisoned\n"+
"into the desert.\n");
    ob = first_inventory(this_object());
    while (ob) {
        obn = next_inventory(ob);
        move_object(ob,"players/ringer/pyramid/desert2");
        ob = obn;
    }
    set_heart_beat(0);
    click=0;
}


