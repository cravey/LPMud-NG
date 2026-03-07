
short() { return "A twisted crystaline maze"; }

long() {
    write(
"This is a crystaline maze.  The walls here are made of clear pure\n"+
"crystal.  Some walls reflect like mirrors, some are completely\n"+
"transparent.  It is not clear which directions you can go.\n");
}

string sd,nd,de,wd,ud,dd,dst,blue,red;
int s,n,e,w,u,d,found;

reset(){
    found=0;
    red = "up";
    u=1;
    s=0;
    ud="south#players/ringer/pyramid/topbalance";
    n = random(4)+6;
    nd="north#"+call_other("players/ringer/pyramid/nmaze1","give_dest",n);
    if (n==9) {found=1; blue="north";}
    e = random(4-found)+6;
    de="east#"+call_other("players/ringer/pyramid/nmaze1","give_dest",e);
    if (e==9) {found=1; blue="east";}
    w = random(4-found)+6;
    wd="west#"+call_other("players/ringer/pyramid/nmaze1","give_dest",w);
    if (w==9) {found=1; blue="west";}
    if (!found) {d=9; blue="down";} else d = random(4-found)+6;
    dd="down#"+call_other("players/ringer/pyramid/nmaze1","give_dest",d);
    if (red == "up" || red=="down") 
      red = "leading "+red;
    else
      red = "to the "+red;
    if (blue == "up" || blue=="down") 
      blue = "leading "+blue;
    else
      blue = "to the "+blue;
}

query_dir() {
    say(call_other(this_player(),"query_name")+" uses an amber orb.\n");
    tell_room(this_object(),
"The orb glows softly.  An openning "+blue+" glows blue. An\n"+
"openning "+red+" glows red.\n");
    return 1;
}

move(dir){
    if (dir<1 || dir>10) {
        write("You bump into a crystal wall.\n");
        return;
    }
    call_other(this_player(),"move_player",dst);
}

init() {
    add_action("search"); add_verb("search");
    add_action("north"); add_verb("north");
    add_action("up"); add_verb("up");
    add_action("down"); add_verb("down");
    add_action("east"); add_verb("east");
    add_action("west"); add_verb("west");
    add_action("south"); add_verb("south");
}

south() {
    dst = sd;
    move(s);
    return 1;
}
north() {
    dst = nd;
    move(n);
    return 1;
}
west() {
    dst = wd;
    move(w);
    return 1;
}
east() {
    dst = de;
    move(e);
    return 1;
}
up() {
    dst = ud;
    move(u);
    return 1;
}
down() {
    dst = dd;
    move(d);
    return 1;
}

search(str) {
    if (!str) str = "room";
    if (str == "room") {
        write("You notice nothing unusual.\n");
        say(call_other(this_player(),"query_name") + " searches the room.\n");
        return 1;
    }
}
