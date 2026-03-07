#include "std.h"

#undef EXTRA_LONG
#define EXTRA_LONG\
    if (str == "painting") {\
     write("The large painting covers the wall from ceiling to the floor.\n");\
   write("It is fastened to the wall with some large nails and it looks\n");\
       write("almost impossible to get loose.\n");\
        return;\
    }

THREE_EXIT("players/kantele/stairway","down",
           "players/kantele/stairway_top","up",
          "players/kantele/upper_hall","north",
           "You are at a landing in the stairway",
           "You are on a landing in the stairway, the stairway continues up\n" +
           "and down, you can go north into the hall. on the east wall you\n" +
           "can se a large painting of Kantele and her sister on a dragon-\n" +
           "hunt they had on one of the outher planes.\n",1)


id(str) {
    if (str == "painting")
        return 1;
}
