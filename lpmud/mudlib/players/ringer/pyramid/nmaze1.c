
short() { return "A twisted crystaline maze"; }

long() {
    write(
"This is a crystaline maze.  The walls here are made of clear pure\n"+
"crystal.  Some walls reflect like mirrors, some are completely\n"+
"transparent.  It is not clear which directions you can go.\n");
}

string sd,nd,de,wd,ud,dd,dst,blue,red;
int s,n,e,w,u,d,found;

give_dest(i) {
    if (i==1) return "players/ringer/pyramid/nmaze1";
    if (i==2) return "players/ringer/pyramid/nmaze2";
    if (i==3) return "players/ringer/pyramid/nmaze3";
    if (i==4) return "players/ringer/pyramid/nmaze4";
    if (i==5) return "players/ringer/pyramid/nmaze5";
    if (i==6) return "players/ringer/pyramid/nmaze6";
    if (i==7) return "players/ringer/pyramid/nmaze7";
    if (i==8) return "players/ringer/pyramid/nmaze8";
    if (i==9) return "players/ringer/pyramid/nmaze9";
    if (i==10) return "players/ringer/pyramid/nmaze10";
}

reset(){
    found=0;
    blue = "south";
    s=1;
    sd="south#players/ringer/pyramid/ecor2";
    n = random(6);
    nd="north#"+give_dest(n);
    if (n==5) {found=1; red="north";}
    e = random(6-found);
    de="east#"+give_dest(e);
    if (e==5) {found=1; red="east";}
    w = random(6-found);
    wd="west#"+give_dest(w);
    if (w==5) {found=1; red="west";}
    u = random(6-found);
    ud="up#"+give_dest(u);
    if (u==5) {found=1; red="up";}
    if (!found) {d=5; red="down";} else d = random(6-found);
    dd="down#"+give_dest(d);
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
