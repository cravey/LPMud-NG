int ndoor, sdoor, endoor, esdoor;
int mode,rstone,bstone,erstone,ebstone;
string symbol;

query_endoor() { return endoor; }
query_ndoor() { return ndoor; }
query_sdoor() { return sdoor; }
query_esdoor() { return esdoor; }
query_erstone() { return erstone; }
query_ebstone() { return ebstone; }
query_mode() { return mode; }

set_erstone(s) { erstone=s; do_change();}
set_ebstone(s) { ebstone=s; do_change();}

reset(arg)
{
    ndoor=0;
    sdoor=1;
    endoor=0;
    esdoor=1;
    rstone = 0;
    bstone = 0;
    erstone = 0;
    ebstone = 0;
    mode = random(5);
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
    call_other("players/ringer/pyramid/etempgate","reset",1);
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
    }
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
                   "south#players/ringer/pyramid/wcor");
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
        do_change();
        return 1;
    }
    if (str == "black stone" || str == "black") {
      say(call_other(this_player(),"query_name")+ " moves the black stone.\n");
        write("The stone clicks into place.\n");
        if (bstone) bstone = 0;
        else bstone = 1;
        do_change();
        return 1;
    }
}

do_change() {
    int code, open, here,eastopen,other,trap,blink;


    open = 0;
    here = 0;
    eastopen = 0;
    other = 0;
    trap = 0;
    blink = 0;
    code = rstone*8 + bstone*4 + erstone*2 + ebstone;
    if (mode == 0) {
        if (code == 0) { here=1; open=1; other=1; eastopen=1;}
        if (code == 11) { 
            changesd(0); other=1; eastopen=0;
            here=2; open=1; 
            call_other("players/ringer/pyramid/etempgate","changend",1);
            endoor=1;
        } else {
            call_other("players/ringer/pyramid/etempgate","changend",0);
            endoor=0;
            changend(0);
        }
        if (code == 6) {here=1; open=0;}
        if (code == 14) {other=1; eastopen=0;}
        if (code == 7 || code==12) {blink=1;}
        if (code == 15) {blink=2;}
        if (code == 8) { trap = 3;}
        if (code == 9 || code == 2) { trap = 2;}
        if (code == 10 || code==13) { trap = 1;}
    }
    if (mode == 1) {
        if (code == 11) { trap = 1;}
        if (code == 12) { trap = 2;}
        if (code == 3) { trap = 3;}
        if (code == 5) { here=1; open=1; changend(0); }
        if (code == 10) { here=2; open=1; other=1; eastopen=0;}
        if (code == 14) { here=1; open=0; other=2; eastopen=1;}
        if (code == 8 || code==1) { blink = 1;}
        if (code == 9) blink=2;
        if (code == 7) { 
            other=1; open=1; 
            call_other("players/ringer/pyramid/etempgate",
                       "changend",0); 
            endoor=0;
        }
    }
    if (mode == 2) {
        if (code == 14) { trap = 1;}
        if (code == 13) { trap = 2;}
        if (code == 7) { trap = 3;}
        if (code == 4) { 
            here=1; open=1; changend(0); 
            call_other("players/ringer/pyramid/etempgate",
                       "changend",0);
            endoor=0;
        }
        if (code == 2) { 
            here=2; open=0; other=1; eastopen=1;
            call_other("players/ringer/pyramid/etempgate",
                       "changend",0);
            endoor=0;
        }
        if (code == 11 || code == 6 || code == 15) {
            changesd(0);
            call_other("players/ringer/pyramid/etempgate",
                       "changesd",0);
            esdoor=0;
        }
        if (code == 15) { other=2; eastopen=1; here=2; open=1;}
        if (code == 5 || code ==12) blink = 1;
        if (code == 13) blink=2;
    }
    if (mode == 3) {
        if (code == 12) { trap = 1;}
        if (code == 9) { trap = 2;}
        if (code == 10) { trap = 3;}
        if (code == 7) blink=2;
        if (code == 3) blink=1;
        if (code == 0) { here=1; open=1; other=1; eastopen=1;}
        else { here=1; open=0; other=1; eastopen=0;}
        if (code==14) changend(1); else changend(0);
        if (code==15) {
            call_other("players/ringer/pyramid/etempgate",
                       "changend",1);
            endoor=1;
        }
        if (code != 15 && code !=14)  {
            call_other("players/ringer/pyramid/etempgate",
                       "changend",0);
            endoor=0;
        }
    }
    if (mode == 4) {
        if (code == 2 || code==8) { trap = 1;}
        if (code == 3 || code==7) { trap = 2;}
        if (code == 9 || code==1) { trap = 3;}
        if (code == 12 || code==5) blink=2;
        if (code == 13) blink=1;
        if (code == 0) { here=1; open=1; other=1; eastopen=1;}
        else { here=1; open=0; other=1; eastopen=0;}
        if (code==10) changend(1); else changend(0);
        if (code==11) {
            call_other("players/ringer/pyramid/etempgate",
                       "changend",1);
            endoor=1;
        }
        if (code != 10 && code !=11)  {
            call_other("players/ringer/pyramid/etempgate",
                       "changend",0);
            endoor=0;
        }
    }

    if (trap) do_trap(trap);
    if (blink) do_blink(blink);
    if (other==1) {
        call_other("players/ringer/pyramid/etempgate","changesd",eastopen);
        esdoor = eastopen;
    }
    if (other==2) {
        call_other("players/ringer/pyramid/etempgate","changend",eastopen);
        endoor = eastopen;
    }
    if (here==1) {
        changesd(open);
        sdoor = open;
    }
    if (here==2) {
        changend(open);
        ndoor = open;
    }
}

do_blink(b) {
    if (b==1 && !present(this_player(),this_object())) {
        write("A brilliant flash temporarily blinds you.\n");
        call_other("players/ringer/pyramid/etempgate","blink_out");
        say(
call_other(this_player(),"query_name") + " appears in a blinding flash.\n");
        move_object(this_player(),this_object());
        return;
    }
    if (b==2 && present(this_player(),this_object())) {
        write("A brilliant flash temporarily blinds you.\n");
        say(
call_other(this_player(),"query_name") + " disappears in a blinding flash.\n");
        move_object(this_player(),"players/ringer/pyramid/etempgate");
        call_other("players/ringer/pyramid/etempgate","blink_in");
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
        call_other("players/ringer/pyramid/etempgate","do_trap",t);
    }
    if (t==2) {
        write("You are hit with a volley of darts.\n");
        call_other(this_player(),"hit_player",20 + random(10));
    }
    if (t==3) {
        write("You are hit with a searing blast of flame.\n");
        call_other(this_player(),"hit_player",25 + random(20));
    }
}

short() {
    return "Tall narrow room";
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
    if (!open && sdoor) {
        sdoor = 0;
        tell_room(this_object(),
            "A large slab to the south slides downward.\n");
    }
}
    
