object item;
int rstone,bstone,gstone,pstone,passage;

query_stones() { return rstone*8+gstone*4+pstone*2+bstone;}

reset() { 
    passage=0;
    rstone=0;
    bstone=0;
    gstone=0;
    pstone=0;
}

short() { return "End of northwest tunnel"; }

long() {
    write(
"This is the end of a short tunnel.  The way leads back to the southeast.\n"+
"You notice some writings on the wall.\n");
    if (passage) write("There is a passage leading north.\n");
}

init() {
    add_action("search"); add_verb("search");
    add_action("seast"); add_verb("southeast");
    add_action("north"); add_verb("north");     
    add_action("look"); add_verb("look");
    add_action("look"); add_verb("l");
    add_action("read"); add_verb("read");
    add_action("read"); add_verb("exa");
    add_action("move"); add_verb("move");
    add_action("move"); add_verb("push");
}

seast() {
    call_other(this_player(),"move_player",
               "southeast#players/ringer/pyramid/ncor2");
    return 1;
}

north() {
    if (!passage) return 0;
    call_other(this_player(),"move_player",
               "north#players/ringer/pyramid/nwgem");
    return 1;
}
    
search(str) {
    if (!str) str = "room";
    if (str == "room") {
        if (call_other("players/ringer/pyramid/netun","query_stones")==6){
            write("You find a passage leading north.\n");
            passage=1;
        }
        exam();
        say(call_other(this_player(),"query_name") + " searches the room.\n");
        return 1;
    }
}

exam() {
    write(
          "You notice a four small stones carefully hidden in the floor.\n");
    if (rstone) 
      write(
            "The red stone is in the forward position.\n");
    else
      write(
            "The red stone is in the back position.\n");
    if (gstone) 
      write(
            "The green stone is in the forward position.\n");
    else
      write(
            "The green stone is in the back position.\n");
    if (pstone) 
      write(
            "The purple stone is in the forward position.\n");
    else
      write(
            "The purple stone is in the back position.\n");
    if (bstone) 
      write(
            "The black stone is in the forward position.\n");
    else
      write(
            "The black stone is in the back position.\n");
    return 1;
}


move(str) {
     string s;
     if (!str) return 0;
     if (sscanf(str,"%s stone",s)) str=s;
     if (str != "red" && str!="black" && str!="green" && str!="purple") 
     return 0;
     say(call_other(this_player(),"query_name") 
         + " moves the "+str+" stone.\n");
     write("The stone clicks into place.\n");
     if (str=="red") {
        if (rstone) 
          rstone = 0;
        else
          rstone = 1;
        return 1;
    } else if (str=="black") {
        if (bstone) 
          bstone = 0;
        else
          bstone = 1;
    } else if (str=="purple") {
        if (pstone) 
          pstone = 0;
        else
          pstone = 1;
    } else if (str=="green") {
        if (gstone) 
          gstone = 0;
        else
          gstone = 1;        
    }
     return 1;
}

read(str) {
    if (str =="stones") return exam();
    if (str != "wall" && str!="writings" && str!="walls")
      return 0;
    say(call_other(this_player(),"query_name")
        +" looks at the wall.\n");
    write("There are some symbols.\n");
    write("  ,,., \n");
    return 1;
}

look(str) {
    if (str == "at stones") return exam();
    if (str != "at wall" && str != "at writings" && str != "at walls") 
      return 0;
    read("wall");
    return 1;
}

    

