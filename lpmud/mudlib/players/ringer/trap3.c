object bear,b2;
int door;

reset(arg)
{
    door = 1;
    if (!present("bear")) {
        bear = clone_object("obj/monster");
        call_other(bear,"set_name","cave bear");
        call_other(bear,"set_alias","bear");
        call_other(bear,"set_level",5);
        call_other(bear,"set_wc",5);
        call_other(bear,"set_ac",6);
        call_other(bear,"set_long","The Bear is large and mean."
                   +" He looks hungry\n");
        call_other(bear,"set_aggressive",0);
        call_other(bear,"set_al",0);
        move_object(bear,this_object());
        b2 = bear;
        bear = clone_object("obj/monster");
        call_other(bear,"set_name","cave bear");
        call_other(bear,"set_alias","bear");
        call_other(bear,"set_level",5);
        call_other(bear,"set_wc",5);
        call_other(bear,"set_ac",6);
        call_other(bear,"set_long","The Bear is large and mean."
                   +" He looks hungry\n");
        call_other(bear,"set_aggressive",0);
        call_other(bear,"set_al",0);
        move_object(bear,this_object());
    }
}

init()
{
    add_action("south"); add_verb("south");
    add_action("north"); add_verb("north");
    add_action("push"); add_verb("push");
}

north()
{
    if (present("dwarf") || present("bear")) {
        write("What with these guys in the way?\n");
        return 1;
    } 
    call_other(this_player(), "move_player", "north#players/ringer/trap4");
    return 1;
}

push(str) {
    if (str == "button")  {
      if (!call_other(this_player(),"id","dwarf")) {
          if (present("bear") || present("dwarf")) {
              write("What with these guys in the way?\n");
              return 1;
          }
      }
      if (door == 1) {
          door = 0;
          say("A heavy iron door to the south slams shut!\n");
          write("A heavy iron door to the south slams shut!\n");
      }
      else {
          door = 1;
          say("The door to the south opens.\n");
          write("The door to the south opens.\n");
      }
      return 1;
  }
}

query_door() { return door; }

query_trap() { return 1; }


south()
{
    if (door) {
        call_other(this_player(),"move_player", "south#players/ringer/trap2");
        return 1;
    } 
    write("The door is closed!\n");
    return 1;
}

long()
{
    write("This is a small room. It smells like large hairy animals here. A\n"
+"passage extends further south.  A small black button is on the east wall\n");
}

short() {
    return "A small room.";
}

