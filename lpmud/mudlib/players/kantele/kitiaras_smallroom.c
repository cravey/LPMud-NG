#include "std.h"

    int value, cost, strength, heal,found;
    string name, short_desc, mess;
    object potion, drink;

#undef EXTRA_RESET
#define EXTRA_RESET\
found =0;\

#undef EXTRA_INIT
#define EXTRA_INIT\
    add_action("search_rubble"); add_verb("search");\

ONE_EXIT("players/kantele/kitiaras_livingroom","west",
         "A small dirty room",
         "This is a small dirty room, with lots of strange rubble.\n",0)

search_rubble(arg) {
    if (arg == "rubble" && found <1)  {
       found =1;
        drink = clone_object("obj/drink");
        if (!call_other(drink, "set_value", "potion" + "#" +
        "A bottle with strange contents" + "#" + "You are totally healed" +
            "#" + 1000 + "#" + 500 + "#" +0))  {
            write("Error in creating drink.\n");
            destruct(drink);
                                               }
        move_object(drink, this_object());
        write("You found a strange bottle.\n");
        return 1;
    } else
    if (arg == "rubble")  {
        write("You found nothing.\n");
        return 1;
           }
    }
