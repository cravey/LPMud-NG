# 1 "players/crimson/.death.c" 
# 1 "/home/einstein/mud/lpmud/mudlib/room/std.h" 1







































































































































































# 2 "players/crimson/.death.c" 2
int i, charges, price;
object targetloc;
string pointo;

id(str) { return str == "pearl" || str == "black pearl" || str == "grim reaper" || str == "death" || str == "reaper"; }

reset(arg) {
    if (!arg) charges= 1;
}

long() {
    write("This is a black pearl shaped in the form of a heart.  It is unusually cold to the touch.\n");
}

short() {
     return "A black pearl";
}

query_value()
{
    return 30000;
}

init() {
    add_action("summon"); add_verb("summon");
    add_action("set"); add_verb("point");
    add_action("stat"); add_verb("stat");
}

summon(arg) {
    string realm, destrealm;
    object otherplayer;
    object death;

    if (!id(arg)) {
        return 0;
    } else {
        if (i != 1) {
            return 1;
        } else {
            otherplayer = find_living(pointo);
            if(otherplayer == 0) return 0;
            targetloc = environment(otherplayer);
                write("You summon the reaper to do your bidding.\n");
                say(call_other(this_player(),"query_name") + " clutches a tiny black pearl.\n");
                tell_room(targetloc, "A dark robbed figure appears in a blinding flash of red mist.\n");
                 death = clone_object("obj/monster");
                call_other(death, "set_name", "death");
                call_other(death, "set_alias", "reaper");
                call_other(death, "set_short", "Grim reaper");
                call_other(death, "set_level", 18);
                call_other(death, "set_al", -1000);
                call_other(death, "set_wc", 35);
                call_other(death, "set_ac", 11);
                call_other(death, "set_aggressive", 1);
                move_object(death, targetloc);
                call_other(death, "attack_object", otherplayer);
                if (charges == 1) {
                    destruct(this_object());
                    write("You watch as the pearl crumbles to dust.\n");
                    call_other(this_player(),"add_weight", -1);
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
        write("The reaper understands.\n");
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
    if (!arg || arg != "black pearl" ||
        call_other(this_player(),"query_level",0)<20) return 0;
    write("Pearl stats:\ncharges: "+charges+"\nsetting: "+pointo+"\n");
    return 1;
}
