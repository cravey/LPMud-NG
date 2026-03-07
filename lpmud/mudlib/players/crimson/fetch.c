#include "std.h"

object mirror;
object fetch;
int dragon_there;
int linens_there;

#undef EXTRA_RESET
#define EXTRA_RESET\
   dragon_there = 0;\
   linens_there = 0;\
   extra_reset();

#undef EXTRA_INIT
#define EXTRA_INIT\
    add_action("look"); add_verb("look");\
    add_action("take"); add_verb("take");\
    add_action("take"); add_verb("get");\
    add_action("search"); add_verb("search");

ONE_EXIT("players/crimson/westrail", "east",
          "Bedroom",
          "White linens cover a small bed in the corner of the room.\n" +
          "A whatnot shelf is on the north wall.  Boxes are scattered\n" +
          "all around.\n", 1)

extra_reset() {
    if ((!fetch || !living(fetch)) && (!mirror || !environment(mirror))) {
          mirror = clone_object("obj/treasure");
          call_other(mirror, "set_id", "mirror");
          call_other(mirror, "set_short", "A mirror");
          call_other(mirror, "set_long", "A mirror with a funny looking dude in it.\n");
          call_other(mirror, "set_weight", 1000);
          call_other(mirror, "set_value", 100);
          move_object(mirror, this_object());
          }
}

search(str) {
object linens;
object money;
   if (str == "bed" && linens_there == 0 || str == "bed linens" && linens_there == 0 || str == "small bed" && linens_there == 0 || str == "bed sheets" && linens_there == 0) {
   write("You dig through the bed sheets.\n");
   write("You found something!\n");
   linens = clone_object("obj/treasure");
   call_other(linens, "set_id", "linens");
   call_other(linens, "set_alias", "sheets");
   call_other(linens, "set_short", "Bed linens");
   call_other(linens, "set_long", "They look like expensive silky white bed sheets.\n");
   call_other(linens, "set_weight", 1);
   call_other(linens, "set_value", 35);
     move_object(linens, this_object());
     money = clone_object("obj/money");
     call_other(money, "set_money", random(50));
     move_object(money, this_object());
   linens_there = 1;
   return 1;
   } else
   if (str == "bed" && linens_there == 1) {
   write("You search the bed and find nothing.\n");
   return 1;
   } else
   write("What do you want to search?\n");
   return 1;
}

take(str) {
object dragon;
     if(str == "dragon from shelf" && dragon_there == 0 || str == "jade dragon from shelf"
     && dragon_there == 0 || str == "dragon" && dragon_there == 0 || str == "jade dragon" && dragon_there == 0 || str == "dragon from whatnot shelf"
     && dragon_there == 0 || str == "jade dragon from whatnot shelf" && dragon_there == 0) {
     write("The jade dragon is pleasantly cool to the touch.\n");
     dragon_there = 1;
     dragon = clone_object("obj/treasure");
     call_other(dragon, "set_id", "dragon");
     call_other(dragon, "set_alias", "jade dragon");
     call_other(dragon, "set_short", "A tiny jade dragon");
     call_other(dragon, "set_long", "It looks like a green dragon and feels cool to the touch.\n");
     call_other(dragon, "set_weight", 1);
     call_other(dragon, "set_value", 175);
     move_object(dragon, this_player());
     return 1;
     }
}

look(str) {
string killer;
string name;
string title;
string align;
string level;

    killer = this_player();
    name = call_other(this_player(), "query_name", 0);
    title = call_other(this_player(), "query_title", 0);
    align = call_other(this_player(), "query_alignment", 0);
    level = call_other(this_player(), "query_level", 0);
    if (str == "at box" || str == "at boxes") {
    write("Hmm, empty.\n");
    return 1;
    } else
    if (str == "at bed" || str == "at small bed") {
    write("It looks ordinary enough.\n");
    return 1;
    } else
    if (str == "at whatnot shelf" && dragon_there == 0 || str == "at shelf" && dragon_there == 0) {
    write("A minature jade dragon captures your attention.\n");
    return 1;
    } else
    if (str == "at whatnot shelf" || str == "at shelf") {
    write("The the shelf is empty.\n");
    return 1;
    } else
    if(level>20) {
    level = 19;
    } else 
    level = level;
    if (str == "at mirror" && present("mirror") || str == "in mirror" && present("mirror")) {
    destruct(mirror);
    tell_room(this_object(), "The mirror shatters into a bjillion pieces!\n");
    fetch = clone_object("obj/monster");
    call_other(fetch, "set_name", "fetch");
    call_other(fetch, "set_short", name+" "+title+" (mirror image)");
    call_other(fetch, "set_long", "Something doesn't look quite right about this creature's eyes.\n");
    call_other(fetch, "set_level", level);
    call_other(fetch, "set_eps", 1000000);
    call_other(fetch, "set_al", -(align));
    call_other(fetch, "set_wc", 16);
    call_other(fetch, "set_ac", 7);
    call_other(fetch, "set_aggressive", 1);
    move_object(fetch, this_object());
    call_other(fetch, "attack_object", killer);
    } else
    if (str) {
    return 0;
    } else
    if (!str) {
    return 0;
  }
}
