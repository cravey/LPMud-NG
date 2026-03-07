#include "../../../room/std.h"

#undef EXTRA_INIT
#define EXTRA_INIT\
       ex_init();

#undef EXTRA_RESET
#define EXTRA_RESET\
        my_reset();

ONE_EXIT("players/ringer/pyramid/tun4","east",
         "Magic room of the court magician",
"Here is the magic room of the court magician.  You feel that the magic\n"+
"of those days is still very much active in this room.  The walls are\n"+
"decorated with magical symbols.  To the west two bowls are built into\n"+
"the wall.  Above each are some writings and paintings.\n",1)

ex_init() {
    add_action("search"); add_verb("search");
    add_action("look"); add_verb("look");
    add_action("look"); add_verb("l");
    add_action("read"); add_verb("read");
    add_action("read"); add_verb("exa");
    add_action("put"); add_verb("put");
    add_action("put"); add_verb("insert");
}

put(str) {
    object ob;
    string item,where;
    if (sscanf(str,"%s in %s bowl",item,where)<2) return 0;
    if (where != "red" && where !="white") return 0;
    if (item == "ruby") {
        if (present("ringer_ruby",this_player())) {
            say(call_other(this_player(),"query_name")
            + " puts a ruby into the "+where+" bowl.\n");
            if (where == "red") {
                write(
                      "The ruby slides into the bowl and disappears.\n");
                say(
                    "The ruby slides into the bowl and disappears.\n");
                destruct(present("ringer_ruby",this_player()));
                if (!present("strength spell",this_player())) {
                    write(
"A voilet glow envelopes your body starting at your feet and working its\n"+
"way up to your head.  The glow slowly fades, but persistently remains\n"+
"around your hands.  You feel stronger.\n");
                    say(
"A voilet glow envelopes "+call_other(this_player(),"query_name")+".\n");
                    ob=clone_object("players/ringer/pyramid/str_spell");
                    move_object(ob,this_player());
                }
            }
            if (where == "white") {
                write(
"The ruby slides into the bowl and disappears. There is a deep rumble\n"+
                      "and dark putrid smoke.\n");
                say(
"The ruby slides into the bowl and disappears. There is a deep rumble\n"+
                    "and dark putrid smoke.\n");
                destruct(present("ringer_ruby",this_player()));
                make_demon();
            }
        return 1;
        }
    }
    if (item == "pearl") {
        if (present("ringer_pearl",this_player())) {
            say(call_other(this_player(),"query_name")
            + " puts a pearl into the "+where+" bowl.\n");
            if (where == "white") {
                write(
                      "The pearl rolls into the bowl and disappears.\n");
                say(
                    "The pearl rolls into the bowl and disappears.\n");
                destruct(present("ringer_pearl",this_player()));
                write("An orb appears and floats gently into your hand.\n");
                say("An orb appears and floats to "
+call_other(this_player(),"query_name")+".\n");
                ob = clone_object("players/ringer/spitem");
                call_other(ob,"set_id","amber orb");
                call_other(ob,"set_alias","orb");
                call_other(ob,"set_alias2","path_orb");
                call_other(ob,"set_real_id","pathfinder orb");
                call_other(ob,"set_short","A large amber orb");
                call_other(ob,"set_value",50);
                call_other(ob,"set_weight",4);
                call_other(ob,"set_charges",5+random(5));
                call_other(ob,"set_spell",5);
                call_other(ob,"set_object",this_object());
                move_object(ob,this_player());
            }
            if (where == "red") {
                write(
"The pearl rolls into the bowl and disappears. There is a deep rumble\n"+
                      "and dark putrid smoke.\n");
                say(
"The pearl rolls into the bowl and disappears. There is a deep rumble\n"+
                    "and dark putrid smoke.\n");
                destruct(present("ringer_pearl",this_player()));
                make_demon();
            }
        return 1;
        }
    }
    return 0;
}

read(str) {
    if (str != "wall" && str!="writings" && str!="bowls" && str!="walls")
      return 0;
    say(call_other(this_player(),"query_name")+" examines the wall.\n");
    write(
"To the west two bowls are built right into the wall. One bowl is white\n"+
"and above it is a painting of a large pearl.  The other bowl is red\n"+
"and above it is a painting of a large ruby.  Between them is a strange\n"+
"hieroglyph:\n");
    write(" /--\ @\nD    |- - - -\n|    | |X| |+|\n"+
"D    |- - - -\n \--/ *\n");
    write("Under the red bowl another hieroglyph: ");
    write("@ .. +\n");
    write("And, under the white bowl a hieroglyph: ");
    write("* ,, +\n");
    return 1;
}

look(str) {
    if (str == "at bowl" || str == "at white bowl" || str == "at red bowl") {
        write("You see nothing unusual about the bowl.\n");
        return 1;
    }
    if (str != "at wall" && str != "at writings" && str != "at bowls") 
      return 0;
    read("wall");
    return 1;
}

object item;

search(str) {
    if (str=="white bowl" || str=="red bowl") str="room";
    if (!str) str = "room";
    if (str != "room")  return 0;
    write("You find nothing unusual.\n");
    say(call_other(this_player(),"query_name") + " searches the room.\n");
    return 1;
}


my_reset() {
}    

use() {
    int r;
    r = call_other(environment(this_player()),"query_dir");
    if (!r)
      write("Nothing appears to happen.\n");
    return r;
} 

object demon,tp;

make_demon() {
    int t;
    
    tp = this_player();
    t = random(5);
    write("A large demon appears!\n");
    say("A large demon appears!  He decides to shread "+
        call_other(this_player(),"query_name")+".\n");
    demon = clone_object("obj/monster");
    call_other(demon,"set_name","demon");
    call_other(demon,"set_alias","ringer_mg_demon");
    call_other(demon,"set_short","A large demon");
    call_other(demon,"set_level",6+t);
    call_other(demon,"set_ac",3+t);
    call_other(demon,"set_wc",10+t);
    call_other(demon,"set_aggressive",1);
    call_other(demon,"set_align",-400);
    move_object(demon,this_object());
    call_other(demon,"attack_object",tp);
}
