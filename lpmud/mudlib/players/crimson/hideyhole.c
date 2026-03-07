#include "std.h"

#undef EXTRA_INIT
#define EXTRA_INIT\
     if(call_other(this_player(), "query_real_name") != "crimson")\
     call_other(this_player(), "move_player", "down#room/church");

ONE_EXIT("players/crimson/workroom", "west",
         "HIDEY HOLE",
         "If you are here and you aren't Crimson, you won't be here\n"+
         "much longer. :-)\n", 1)
