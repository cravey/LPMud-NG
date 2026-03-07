# 1 "players/crimson/.crystal.c" 
# 1 "/home/einstein/mud/lpmud/mudlib/room/std.h" 1







































































































































































# 2 "players/crimson/.crystal.c" 2
# 1 "/home/einstein/mud/lpmud/mudlib/obj/handshake.h" 1



string handshake_data;
int handshake;









get_handshake(confirm) {
    if (confirm == handshake && handshake) {
        handshake = 0;
	return handshake_data;
    } else {
        return 0;
    }
}
# 3 "players/crimson/.crystal.c" 2
int i, charges, price;
int magic;
object targetloc;
string pointo;

id(str) { return str == "crystal" || str == "crystal ball" || str == "ball" || str == "into crystal" || str == "into ball"; }

reset(arg) {
    if (!arg) charges=random(6);
    magic = 1;
}

long() {
    write("A perfectly round crystal ball.\n");
}

short() {
     return "A crystal ball";
}

query_value()
{
    return 200;
}

init() {
    add_action("use"); add_verb("gaze");
    add_action("set"); add_verb("point");
    add_action("stat"); add_verb("stat");
}

use(arg) {
    string realm, destrealm;
    object otherplayer;
    object old_room;

    if (!id(arg)) {
        return 0;
    } else {
        if (i != 1) {
            return 1;
        } else {
            old_room = environment(this_player());
            otherplayer = find_living(pointo);
            if(otherplayer == 0) return 0;
            targetloc = environment(otherplayer);
                write("Gazing into the crystal you see....\n");
                say(call_other(this_player(),"query_name") + " gazes into the crystal.\n");
                move_object(this_player(), targetloc);
                call_other(this_player(), "look", 0);
                move_object(this_player(), old_room);
                if (charges == 0) {
                    destruct(this_object());
                    write("The crystal is exasted and crumbles before you.\n");
                }
                charges=charges-1;
            }
            return 1;
        }
}

set(arg) {

    if (!arg) {
        return 0;
    }
    else {

         pointo = lower_case(arg);
        write("The crystal now knows the knowledge you seek.\n");
        i = 1;
    }
   return 1;
}

get() {
    return 1;
}

query_weight() {
    return 1;
}
stat(arg) {
    if (!arg || arg != "crystal ball" ||
        call_other(this_player(),"query_level",0)<20) return 0;
    write("Crystal stats:\ncharges: "+charges+"\nsetting: "+pointo+"\n");
    return 1;
}

query_magic() { return magic; }
