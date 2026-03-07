#include "std.h"

#undef EXTRA_LONG
#define EXTRA_LONG\
    if (str == "head") {\
       write("This is the head of 'Alf the black knight'\n");\
       write("It is in premium condition, and exactly as Kanteley slayed\n");\
       write("him. head is an Item Kantele likes, so it is not possible to\n");\
       write("steal, even if you would like to.....\n");\
       return;\
}
TWO_EXIT("players/kantele/bar","north",
         "players/kantele/office","south",
         "Kantele's trophy room",
         "This is the room Kantele places some of her trophies, on a\n" +
         "table in the center of the room, you can see a human head ! \n" +
         "On the north wall you can see the head of the 'Great Wyrm' the \n" +
         "famous Gold Dragon Kantele slayed on her 18'th years birtday. \n" +
         "To the north you can see the bar and to the south there is some\n" +
         "sort of workroom.",1)

id(str) {
    if (str == "head")
        return 1;
}
