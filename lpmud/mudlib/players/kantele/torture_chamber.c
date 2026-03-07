#include "std.h"

int iron_maiden;
#undef EXTRA_LONG
#define EXTRA_LONG\
    if (str == "iron maiden" && iron_maiden == 1) {\
       write("The iron maiden is open.\n");\
       return;\
   }\
    if (str == "iron maiden" && iron_maiden ==0) {\
       write("The iron maiden is closed.\n");\
        return;\
   }

#undef EXTRA_INIT
#define EXTRA_INIT\
    add_action("enter"); add_verb("enter");\
    add_action("open_maiden"); add_verb("open");\

#undef EXTRA_RESET
#define EXTRA_RESET\
    iron_maiden =0;

id(str) {
    if (str == "maiden" || str == "stretch benk")
        return 1;
}

ONE_EXIT("players/kantele/guard_room.c","east",
         "In a torture chamber",
         "In a torture chamber, the only exit is east into the guardroom. You\n" +
         "can see a well used stretch benk and an ironmaiden.\n",0)

open_maiden(str) {
    if (str == "iron maiden" && iron_maiden == 1) {
       write("The iron maiden is already open.\n");
     }
    if (str == "iron maiden" && iron_maiden ==0) {
       write("You open the iron maiden.\n");
say(call_other(this_player(), "query_name") + " opens the iron maiden.\n");
       iron_maiden == 1;
     }
     return 1;
}

enter(str) {    
    if (str == "iron maiden") {
       write("You can't do a so stupid thing.......\n");
say(call_other(this_player(), "query_name") + " thinks he is playing ABERmud.\n");
     return 1;
}
}
