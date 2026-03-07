#include "std.h"

#undef EXTRA_LONG
#define EXTRA_LONG\
    if (str == "blackboard") {\
        write("There is a message on the blackboard the message reads:\n");\
        write("Dear Kit !\n\n");\
        write("Can you visit me tomorow ?\n");\
        write("Please bring my gift !\n");\
        write("unless you want to visit my pets!\n");\
        write("\n /K\n");\
        return;\
    }
ONE_EXIT("players/kantele/kitiaras_livingroom","east",
         "In a strange room",
         "You are in a strange room, the room is small and totaly without\n" +
         "furniture of any kind. It looks like some kind of meditation \n" +
         "room, a large blackboard stands against the north wall.\n",1)
id(str) {
    if (str == "blackboard")
        return 1;
}
