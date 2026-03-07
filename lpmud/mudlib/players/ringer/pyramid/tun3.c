#include "../../../room/std.h"

#undef EXTRA_INIT
#define EXTRA_INIT\
       ex_init();

#undef EXTRA_RESET
#define EXTRA_RESET ex_reset();

int pit,mode;

query_pit() { return pit; }

TWO_EXIT("players/ringer/pyramid/tun4","west",
         "players/ringer/pyramid/tun2","east",
         "East/west corridor",
"The corridor continues east and west here.  The walls are smooth and\n"+
"uniform and somewhat dusty.\n",0)

ex_init() {
    if (!pit) {
        add_action("search"); add_verb("search");
    } else {
        write("You fall into a pit.\n");
        if (mode==0) 
          move_object(this_player(),"players/ringer/pyramid/pit1");
        else
          move_object(this_player(),"players/ringer/pyramid/viperpit");
    }
    if (mode>1) 
      do_trap();
}

ex_reset() {
    pit=0;
    mode = random(4);
    if (mode == 0 || mode ==1) 
        set_heart_beat(1);
}

search(str) {
    if (!str) str = "room";
    if (str == "room") {
        osrch("here");
        say(call_other(this_player(),"query_name") + " searches the room.\n");
        return 1;
    }
}

osrch(str) {
    if (random(100) < 20) {write("You see nothing unusual.\n"); return 1;}
    if (str!="here") str="to the "+str;
    if (mode == 0 || mode ==1) 
      if (!pit) { 
          write(
                "You notice something slightly different about the floor \n"+
                str+".\n");
          if (mode==1) write("You here faint sounds somewhere near.\n");
      } else 
        write("There is a pit "+str+".\n");
    if (mode==2) {
        write(
              "You notice something slightly different about the ceiling \n"+
              str+".\n");
    }
    if (mode==3) {
        write(
              "You notice something slightly different about the walls \n"+
              str+".\n");
    }
}

do_trap()
{
    if (call_other("players/ringer/pyramid/tun1","query_stones")==mode) 
      return;
    if (call_other(this_player(),"query_ghost")) return;
    if (mode==2) {
        write(
"Several large stones fall from the ceiling upon your head.\n");
        call_other(this_player(),"hit_player",10+random(25));
    }
    if (mode==3) {
        write(
"A searing blast of flame coming from the passage walls engulfs you.\n");
        call_other(this_player(),"hit_player",10+random(25));
    }
}

heart_beat() {
    object ob;
    if (call_other("players/ringer/pyramid/tun1","query_stones")==mode) 
      return;
    ob = first_inventory(this_object());
    if (ob) {
        pit=1;
        do_pit();
        set_heart_beat(0);
    }
}

do_pit() {
    object ob,obn;
    string dest;
    say(
"\nThe floor falls out from beneath you!  You fall a long way into a pit.\n");
    if (mode==0) dest="players/ringer/pyramid/pit1";
    else dest="players/ringer/pyramid/viper_pit";
    ob = first_inventory(this_object());
    while (ob) {
        obn = next_inventory(ob);
        move_object(ob,dest);
        ob = obn;
    }
}
