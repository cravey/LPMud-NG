# 1 "players/crimson/workroom.c"
# 1 "/home/lpmud/mudlib/room/std.h"







































































































































































# 2 "players/crimson/workroom.c"
# 1 "players/crimson/lc.h"
special_reset() {
object potion;
int n;
int j;
        n = random(8);
        j = random(3);
    if (!present("potion")) {
        if (j == 0) {
        potion = clone_object("players/crimson/unknowna");
        } else
        if (j == 1) {
        potion = clone_object("players/crimson/unknown");
        } else
        if (j == 2) {
        potion = clone_object("players/crimson/unknownb");
        }
        if (n == 0) {
        call_other(potion, "set_id", "brilliant red potion");
        } else
        if (n == 1) {
        call_other(potion, "set_id", "light blue potion");
        } else
        if (n == 2) {
        call_other(potion, "set_id", "light green potion");
        } else
        if (n == 3) {
        call_other(potion, "set_id", "black potion");
        } else
        if (n == 4) {
        call_other(potion, "set_id", "purple potion");
        } else
        if (n == 5) {
        call_other(potion, "set_id", "multi-colored potion");
        } else
        if (n == 6) {
        call_other(potion, "set_id", "orange potion");
        } else
        if (n == 7) {
        call_other(potion, "set_id", "grey potion");
        }
        if (n == 0) {
        call_other(potion, "set_short", "A brilliant red potion");
        } else
        if (n == 1) {
        call_other(potion, "set_short", "A light blue potion");
        } else
        if (n == 2) {
        call_other(potion, "set_short", "A light green potion");
        } else
        if (n == 3) {
        call_other(potion, "set_short", "A black potion");
        } else
        if (n == 4) {
        call_other(potion, "set_short", "A purple potion");
        } else
        if (n == 5) {
        call_other(potion, "set_short", "A multi-colored potion");
        } else
        if (n == 6) {
        call_other(potion, "set_short", "An orange potion");
        } else
        if (n == 7) {
        call_other(potion, "set_short", "A grey potion");
        }
        move_object(potion, this_object());
    }
}
# 3 "players/crimson/workroom.c"














up(str) {
   if (call_other(this_player(), "query_real_name") != "crimson") {
    return 1;
   } else
   call_other(this_player(), "move_player", "up#players/crimson/storeroom");
   return 1;
}

east(str) {
   if (call_other(this_player(), "query_real_name") != "crimson") {
    return 1;
   } else
   call_other(this_player(), "move_player", "east#players/crimson/hideyhole");
   return 1;
}

thunder(str) {
   call_other(this_player(), "move_player", "to THUNDERDOME#players/crimson/thunderdome");
   return 1;
}

post() {
   if (call_other(this_player(), "query_real_name") != "crimson") {
   return 0;
   } else
   cat("/room/post_dir/crimson.o");
}

extra_reset() {
object orc;
         if (!orc || !living(orc)) {
         orc = clone_object("players/crimson/invaders");
         call_other(orc, "set_name", "orc");
         call_other(orc, "set_short", "An ugly snout nosed ORC!");
         call_other(orc, "set_level", 1);
         call_other(orc, "set_long", "He looks to be a truely foul creature.\n");
         call_other(orc, "set_wc", 9);
         call_other(orc, "set_ac", 3);
         call_other(orc, "set_hp", 30);
         call_other(orc, "set_aggressive", 1);
         move_object(orc, this_object());
         }
}
reset(arg) { special_reset(); if (arg) return; set_light( 1); } short() { if (set_light(0)) return            "Velcome to my Coffin!"; return "dark room"; } init() { add_action("move1",  "west"); add_action("move2",  "north"); add_action("move3",  "south"); add_action("move4",  "down"); add_action("up"); add_verb("up"); add_action("east"); add_verb("east"); add_action("post"); add_verb("post"); add_action("thunder"); add_verb("thunder"); if(call_other("players/crimson/thunderdome", "query_chall") == 1) write("HEY! There is a challenge going on in THUNDERDOME.\n"); } move1() {  call_other(this_player(), "move_player",  "west" + "#" + "room/plane4"); return 1; } move2() {  call_other(this_player(), "move_player",  "north" + "#" +            "room/pub2"); return 1; } move3() {  call_other(this_player(), "move_player",  "south" + "#" +            "room/shop"); return 1; } move4() {  call_other(this_player(), "move_player",  "down" + "#" +            "room/church"); return 1; } long(str) { if (set_light(0) == 0){ write("It is dark.\n"); return; }  write(           "Velcome to my Coffin!\n"+           "..... reads a sign over a shiny black coffin.\n" +           "The CHURCH is DOWN.\n" +           "The shop is south.\n" +           "The pub is to the north.\n" +           "And Crimson's castle is west.\n"); write("There are four obvious exits, " +  "west" + ", " +  "north" + ", " +  "south" + " and " +  "down" + ".\n"); }
# 73 "players/crimson/workroom.c"
