/*
 * Trap stone
 */

string short_desc, long_desc;
int value, local_weight, trance;
string name, alias_name;
string read_msg;
object familiar, save_loc;

id(str)
{
    return str == name || str == alias_name || str == "trapstone";
}

short() {
    return short_desc;
}

long() {
    write(long_desc);
}

query_value() { return value; }

query_id() { 
    return "Stone of trapping";
}

set_id(str) {
    local_weight = 1;
    name = str;
}

set_alias(str) {
    alias_name = str;
}

set_short(str) {
    short_desc = str;
    long_desc = "You see nothing special.\n";
}

set_long(str) {
    long_desc = str;
}

set_value(v) {
    value = v;
}

set_weight(w) {
    local_weight = w;
}

get() {
    return 1;
}

query_weight() {
    return local_weight;
}

query_magic() { return 1; }

init() {
    add_action("gaze"); add_verb("gaze");
    add_action("brk"); add_verb("break");
    add_action("brk"); add_verb("crush");
    add_action("brk"); add_verb("destroy");
}

gaze(str) {
    string s1,s2;
    if (!str) return 0;
    sscanf(str,"into %s",s1);
    sscanf(str,"at %s",s2);
    if (id(s1)) str=s1;
    if (id(s2)) str=s2;
    if (!present(this_object(),this_player())) return 0;
    if (id(str)) {
        write("You gaze deeply into the " + name + "\n");
        write("You feel yourself being pulled into the stone!\n");
        write("You are magically transfered somewhere.\n");
        move_object(this_player(),"players/ringer/sp_lair");
        return 1;
    }
}

brk(str) {
    int r;
    string dest;
    if (id(str)) {
        write("You crush the stone.  It explodes!\n");
        write("In the explosion you are magically transfered elsewhere.\n");
        r = random(10)+1;
        if (r==1) dest = "players/ringer/mg_shop";
        if (r==2) dest = "players/ringer/for_cliff";
        if (r==3) dest = "players/ringer/en_stream";
        if (r==4) dest = "players/ringer/en_forest";
        if (r==5) dest = "room/vill_green";
        if (r==6) dest = "room/shop";
        if (r==7) dest = "room/yard";
        if (r==8) dest = "room/plane1";
        if (r==9) dest = "room/clearing";
        if (r==10) dest = "room/vill_track";
        call_other(this_player(),"move_player","X#"+dest);
        call_other(this_player(),"hit_player",100);
        destruct(this_object());
        return 1;
    }
}
