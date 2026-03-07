#include "std.h"
object fruit;
string tree;
int found;

#undef EXTRA_LONG
#define EXTRA_LONG\
    if (str == "tree") {\
       write("The tree looks as though it had been picked nearly clean.\n");\
       return;\
   }

#undef EXTRA_RESET
#define EXTRA_RESET\
found = found-1;\

#undef EXTRA_INIT
#define EXTRA_INIT\
    add_action("pick_fruit"); add_verb("pick");\

ONE_EXIT("players/crimson/wiz_world1", "down",
         "The Treetops",
      "You are in the very top of the tree. Only one fruit remains. You\n" +
      "have tried to reach it from various directions but this seems to be\n" +
      "the best angle.\n", 1)

pick_fruit(arg) {
    if (arg == "fruit" && found <1)  {
       found =3;
       fruit = clone_object("obj/treasure");
       call_other(fruit, "set_id", "fruit");
       call_other(fruit, "set_short", "A rare fruit");
       call_other(fruit, "set_weight", 1);
       call_other(fruit, "set_value", 0);
       call_other(fruit, "set_long", "It is perfectly smooth and it's smell is tantalizing.\n");
        move_object(fruit, this_object());
        write("You manage to pick it but loose your grasp on it! Better look quick!\n");
      return 1;
    } else
    if (arg == "fruit")  {
        write("You can't quite reach it.\n");
        return 1;
           }
    }
id(str) {
    if (str == "fruit")
        return 1;
}
