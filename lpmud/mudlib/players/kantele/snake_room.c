#include "std.h"

string rope;
#undef EXTRA_INIT
#define EXTRA_INIT\
    add_action("down"); add_verb("down");\
    add_action("down"); add_verb("climb");\
    add_action("jump_down"); add_verb("jump");

#undef EXTRA_LONG
#define EXTRA_LONG\
    if (str == "pit") {\
        write("You look down in the pit, it seems slipery, ");\
        write("and imposible to climb down.\n");\
        return;\
        }

   
ONE_EXIT("players/kantele/kitiaras_crossing","west",
         "In a room with a pit",
         "You are in a room with a pit, the only exit seems to be west,\n" +
          "and you can see a large pillar standing beside the pit.\n",0)


down() {
    if (!rope) {
        write("You would fall down the pit and possible hurt yourself.\n");
        return 1;
    }
    call_other(this_player(), "move_player", "down#players/kantele/snake_pit");
}

tie(str)
{
    if (str != "pillar")
        return 0;
    rope = 1;
    return 1;
}
untie(str) {
    rope = 0;
    return 1;
}


query_rope() {
    return rope;
}
jump_down(arg) {
    if (arg != "down")  {
       write("You throw yourself into the pit !\n");
       write("You hit the dirt hard......\n");
       call_other(this_player(), "hit_player",20);
call_other(this_player(), "move_player", "jump_down#players/kantele/snake_pit");
    }
}

id(str) {
    if (str == "pillar")
        return 1;
    if (str == "pit")
        return 1;
}
