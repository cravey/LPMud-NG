#include "std.h"
int door_open;
int trapdoor_open;

reset(arg) {
     if (!arg)
     door_open = 0;
     trapdoor_open = 0;
}

#undef EXTRA_INIT
#define EXTRA_INIT\
    add_action("look"); add_verb("look");\
    add_action("look"); add_verb("peek");\
    add_action("west"); add_verb("west");\
    add_action("down"); add_verb("down");\
    add_action("open"); add_verb("open");

TWO_EXIT("players/crimson/stairs", "up",
         "players/crimson/margoyle", "east",
         "Secret passage central",
         "The walls here are plain grey. Dust hampers your\n" +
         "breathing.  To the east you can make out an opening\n" +
         "in a wall.  On the west wall you see a small peekhole\n" +
         "and next to it is a door.  The floor is uneven.  With a \n" +
         "closer look it becomes evident that there is some kind\n" +
         "of cellardoor in here.  The entire place gives you the\n" +
         "feeling of a tomb.  It is still possible to tread up the\n"+
         "stairs from here.\n", 0)

look(str) {
    
    if (str == "into peekhole" || str == "in peekhole" || str == "through peekhole" || str == "in the peekhole" || str == "through the peekhole") {
    move_object(this_player(), "players/crimson/back");
    call_other(this_player(), "look", 0);
    move_object(this_player(), "players/crimson/stairs2");
    return 1;
    } else
    if (str == "at door" && door_open == 1) {
    write("An ordinary looking door.  It is open.\n");
    return 1;
   } else
   if (str == "at door") {
   write("The door is closed. It is perfectly flush with the wall except for a tiny handle.\n");
   return 1;
    } else 
    if (str == "at cellardoor" && trapdoor_open == 1) {
    write("An open cellardoor in the floor.\n");
    return 1;
    } else
    if (str == "at cellardoor") {
    write("The cellardoor is closed.\n");
    return 1;
    }
}

open(str) {
  
    if (str == "door" && str != "cellardoor" && door_open == 1) {
    write("The door is already open.\n");
    return 1;
    } else
    if (str == "door") {
    door_open =1;
    write("You open the door.\n");
    return 1;
    } else
    if (str == "cellardoor" && trapdoor_open == 1) {
    write("The cellardoor is already open.\n");
    return 1;
    } else
    if (str == "cellardoor") {
    trapdoor_open = 1;
    write("You swing the cellardoor open and reveal yet another set of steps.\n");
    return 1;
    }
}

west() {

    if (door_open == 0) {
    write("The door is closed.\n");
    return 1;
    } else
    if (door_open == 1) {
    call_other(this_player(), "move_player", "through the doorway#players/crimson/back");
    write("The door slams shut behind you!\n");
    return 1;
    }
}

down() {

    if (trapdoor_open == 0){
    write("The cellardoor is closed.\n");
    return 1;
    } else
    if (trapdoor_open == 1) {
    write("You cautiously travel down the huge winding staircase.\n");
    call_other(this_player(), "move_player", "down through the cellardoor#players/crimson/underworld8");
    return 1;
    }
}

realm() { return "NT"; }
