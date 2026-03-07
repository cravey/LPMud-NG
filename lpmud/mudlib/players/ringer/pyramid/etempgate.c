int ndoor, sdoor;
int mode,rstone,bstone;
string symbol;

reset(arg)
{
    ndoor=call_other("players/ringer/pyramid/wtempgate","query_endoor");
    sdoor=call_other("players/ringer/pyramid/wtempgate","query_esdoor");
    rstone = call_other("players/ringer/pyramid/wtempgate","query_erstone");
    bstone = call_other("players/ringer/pyramid/wtempgate","query_ebstone");
    mode = call_other("players/ringer/pyramid/wtempgate","query_mode");
    if (mode==0) 
      symbol = "<|&^>";
    if (mode==1) 
      symbol = "{&/!}";
    if (mode==2) 
      symbol = ">|&^<";
    if (mode==3)
      symbol = "}!:&{";
    if (mode==4)
      symbol = ">>&<<";
    tell_room(this_object(),
"You hear stones shifting and sliding against one another behind the\n"+
"walls.  You feel that something in the room has changed.\n");
}

init()
{
    add_action("south"); add_verb("south");
    add_action("north"); add_verb("north");
    add_action("push"); add_verb("move");    
    add_action("push"); add_verb("push");    
    add_action("push"); add_verb("pull");    
    add_action("search"); add_verb("search");
    add_action("read"); add_verb("read");
    add_action("read"); add_verb("exa");
    add_action("look"); add_verb("look");
    add_action("look"); add_verb("l");
}

read(str) {
    if (str=="wall" || str=="walls" || str=="writings") {
        say(call_other(this_player(),"query_name") + " looks at wall.\n");
        if (present("amulet of tongues",this_player()) ||
            present("onyx amulet",this_player())) {
            write("\nThe onyx amulet glows brightly!\n");
            write(
"ATTN: Temple gate is to be operated only by certified temple priests and\n"+
"qualified assistants!  Unauthorized personnel will be destroyed!\n");
        } else 
            write("The words are in a language foreign to you.\n");
        
        return 1;
    }
}

look(str) {
    if (str =="at wall" || str =="at walls" || str=="at writings") {
        return read("wall");
    } else return 0;
}


search(str) {
    if (!str) str="room";
    if (str=="room") {
        say(call_other(this_player(),"query_name")+" searches the room.\n");
        write(
"You notice that there is a symbol carefully engraved under one of the\n"+
"stones: "+symbol+"\n");
        return 1;
    }
}


north()
{
    if (ndoor==1) {
        call_other(this_player(), "move_player", 
                   "north#players/ringer/pyramid/temp");
    } else {
        write("The door to the north is closed.\n");
    }
    return 1;
}

south()
{
    if (sdoor==1) {
        call_other(this_player(), "move_player", 
                   "south#players/ringer/pyramid/ecor");
    } else {
        write("The door to the south is closed.\n");
    }
    return 1;
}

long()
{
    write(
"This is a tall narrow room. Its purpose is not altogether obvious.\n");
    write(
"Large single slabs of sandstone make up the east and west walls.\n");
    write(
"There are some writings on the east wall.\n");
    write(
"On the east wall there are two stones set in the wall-- a red one and a\n"+
"black one.  They are in slots and are somewhat movable.\n");
    if (ndoor==0) 
        write(
"A large stone door is closed to the north.\n");
    else write(
"There is an openning to the north leading into a large room.\n");
    if (sdoor==0) 
        write(
"A large stone door is closed to the south.\n");
    else write(
"There is an openning to the south leading into a hallway.\n");
    if (rstone) write("The red stone is at the top of its slot.\n");
    if (bstone) write("The black stone is at the top of its slot.\n");
    if (ndoor==1 && sdoor==1) 
      write("The only obvious exits are north and south.\n");
    else if (ndoor==0 && sdoor==1)
      write("The only obvious exit is south.\n");
    else if (ndoor==1 && sdoor==0)
      write("The only obvious exit is north.\n");
    else 
      write("There are no obvious exits.\n");
}


push(str) {
    if (str == "stone") {
        write("which one?");
        return 1;
    }
    if (str == "red stone" || str == "red") {
        say(call_other(this_player(),"query_name")+ " moves the red stone.\n");
        write("The stone clicks into place.\n");
        if (rstone) rstone = 0;
        else rstone = 1;
        call_other("players/ringer/pyramid/wtempgate","set_erstone",rstone);
        return 1;
    }
    if (str == "black stone" || str == "black") {
      say(call_other(this_player(),"query_name")+ " moves the black stone.\n");
        write("The stone clicks into place.\n");
        if (bstone) bstone = 0;
        else bstone = 1;
        call_other("players/ringer/pyramid/wtempgate","set_ebstone",bstone);
        return 1;
    }
}

do_trap(t) {
    object ob,nob;
    if (t==1) {
        ob = first_inventory(this_object());
        tell_room(this_object(),
"The floor of the room falls out from under you!  You fall a long way\n"+
"into a pit.\n");
        while(ob) {
            nob = next_inventory(ob);
            move_object(ob,"players/ringer/pyramid/pit1");
            ob=nob;
        }
    }
}

changend(open) {
    if (open && !ndoor) {
        ndoor = 1;
        tell_room(this_object(),
                  "A large slab to the north slides upward.\n");
    }
    if (!open && ndoor) {
        ndoor = 0;
        tell_room(this_object(),
                  "A large slab to the north slides downward.\n");
    }
}


changesd(open) {
    if (open && !sdoor) {
        sdoor = 1;
        tell_room(this_object(),
                  "A large slab to the south slides upward.\n");
    } 
    if (!open && sdoor){
        sdoor = 0; 
        tell_room(this_object(),
                  "A large slab to the south slides downward.\n");
    }
}


short() {
    return "Tall narrow room";
}

blink_in() {
    say(
 call_other(this_player(),"query_name") + " appears in a blinding flash.\n");
}

blink_out() {
    say(
call_other(this_player(),"query_name") +" disappears in a blinding flash.\n");
}

