#include "std.h"
int i;
object torch;

id(str) {
    if (str == "torch")
        return 1;
}

#undef EXTRA_LONG
#define EXTRA_LONG\
    if (str == "torch" && torch == 2) {\
        write("You can see some torches, alas they are to high on the wall.\n");\
        return;\
        }\
    if (str == "torch" && torch < 2) {\
        write("You can see some torches.\n");\
        return;\
        }
#undef EXTRA_INIT
#define EXTRA_INIT add_action("get_torch"); add_verb("get");

get_torch(arg) {
if (arg != "torch") {
return 1;
} else {
   if (!present("torch", this_player()) && i < 3) {
       i=i+1;
        torch = clone_object("obj/torch");
        call_other(torch, "set_name", "torch");
        call_other(torch, "set_fuel", 2000);
        call_other(torch, "set_weight", 1);
        move_object(torch, this_player());
        write("Ok.\n");
        return 1;
                                           }
}
}
TWO_EXIT("players/kantele/cellar_east","south",
         "players/kantele/cellar_passage1","north",
         "In a dark passage",
         "In a dark passage, going north/south. The passage is lighted\n" +
         "with torches. ",1)
