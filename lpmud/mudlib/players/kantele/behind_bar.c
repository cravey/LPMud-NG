#include "std.h"

#undef EXTRA_INIT
#define EXTRA_INIT add_action("drink_beer"); add_verb("drink");

ONE_EXIT("players/kantele/bar","east",
         "Behind the bar",
         "You are behind the bar, you can enter the barroom proper to the\n" +
         "west, there is a very large barrel of beer here, there should be\n" +
         "possible to drink from the tap.....\n",1)

drink_beer(arg) {
    if (arg != "from tap" && arg != "from the tap")
	return 0;
    if (call_other(this_player(), "drink_alcohol",2)) {
       write("It tasted a bit stale\n");
    }
    return 1;
}
