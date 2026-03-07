#include "std.h"

int gate_is_open1,gate_is_open2,gate_is_open3;
object watchman,sturdy_sword,dog;
#undef EXTRA_INIT
#define EXTRA_INIT add_action("pull_ring"); add_verb("pull");

#undef EXTRA_LONG
#define EXTRA_LONG\
    if (str == "ring" || str == "rings") {\
       write("it looks like the rings is fastened to some sort of wire.\n");\
       return 1;\
   }
#undef EXTRA_RESET
#define EXTRA_RESET\
gate_is_open1 = 0;\
gate_is_open2 = 0;\
gate_is_open3 = 0;\
        extra_reset();

extra_reset() {
   if (!watchman || !living(watchman)) {
       watchman = clone_object("obj/monster");
       call_other(watchman, "set_name", "watchman");
       call_other(watchman, "set_level",14);
       call_other(watchman, "set_short", "The night watchman is here");
       call_other(watchman, "set_ac", 4);
       call_other(watchman, "set_wc",15);
       call_other(watchman, "set_aggressive", 1);
     move_object(watchman, this_object());
       sturdy_sword  = clone_object("obj/weapon");
    call_other(sturdy_sword, "set_name", "A sword");
    call_other(sturdy_sword, "set_short", "A sturdy sword");
    call_other(sturdy_sword, "set_class",15);
    call_other(sturdy_sword, "set_alt_name", "sword");
    call_other(sturdy_sword, "set_alias","sword");
    call_other(sturdy_sword, "set_value",700);
    call_other(sturdy_sword, "wield","sword");
    move_object(sturdy_sword, watchman);
    dog  = clone_object("obj/monster");
       call_other(dog, "set_name", "dog");
       call_other(dog, "set_short", "A guard dog is here");
       call_other(dog, "set_ac",1);
        call_other(dog, "set_level",8);
       call_other(dog, "set_wc",10);
     move_object(dog, this_object());
}
}

THREE_EXIT("players/kantele/torture_chamber","west",
           "players/kantele/west_bridge.c","east",
           "players/kantele/dungeon_corridor.c","south",
           "A guard room",
           "A guard room, you can leave to east into a hall, west into a torture room\n" +
"and south into a dungeon corridor. Three large iron rings is fastend in the\n" +
           "wall.",0)

pull_ring(str) {
             if (str == "ring 1" && gate_is_open1 == 1) {
             write("you pull the ring.\n");
             gate_is_open1 = 0;
            say(call_other(this_player(), "query_name") + " pulls ring 1.\n");
            tell_room(this_object(), "You hear a grinding sound.\n");
             return 1;
             }
             if (str =="ring 1" && gate_is_open1 ==  0) {
             write("you pull the ring.\n");
             gate_is_open1 = 1;
            say(call_other(this_player(), "query_name") + " pulls ring 1.\n");
            tell_room(this_object(), "You hear a crunshing sound.\n");
            return 1;
            }

             if (str == "ring 2" && gate_is_open2 == 1) {
             write("you pull the ring.\n");
             gate_is_open2 = 0;
            say(call_other(this_player(), "query_name") + " pulls ring 2.\n");
            tell_room(this_object(), "You hear a grinding sound.\n");
             return 1;
             }
             if (str =="ring 2" && gate_is_open2 ==  0) {
             write("you pull the ring.\n");
             gate_is_open2 = 1;
            say(call_other(this_player(), "query_name") + " pulls ring 2.\n");
            tell_room(this_object(), "You hear a crunshing sound.\n");
            return 1;
            }

             if (str == "ring 3" && gate_is_open3 == 1) {
             write("you pull the ring.\n");
             gate_is_open3 = 0;
           say(call_other(this_player(), "query_name") + " pulls ring 3.\n");
            tell_room(this_object(), "You hear a grinding sound.\n");
             return 1;
             }
             if (str =="ring 3" && gate_is_open3 ==  0) {
             write("you pull the ring.\n");
             gate_is_open3 = 1;
            say(call_other(this_player(), "query_name") + " pulls ring 3.\n");
            tell_room(this_object(), "You hear a crunshing sound.\n");
            return 1;
}
}

id(str) { 
return str ==  "ring 1" || str ==  "ring 2" || str == "ring 3" || str == "ring"|| str == "rings";
}


query_gate2() { return !gate_is_open2; }
query_gate1() { return !gate_is_open1; }
query_gate3() { return !gate_is_open3; }
