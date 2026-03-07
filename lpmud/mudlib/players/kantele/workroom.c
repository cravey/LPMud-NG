#include "std.h"

#undef EXTRA_RESET
#define EXTRA_RESET\
       new_reset();\
        if (!present("money")) {\
      object money;\
     money = clone_object("obj/money");\
      call_other(money, "set_money",100000);\
      move_object(money,this_object());\
}

TWO_EXIT("room/pub2" , "pub" ,
        "players/kantele/store.c","south",
         "silent workroom",
        "Kitiaras silent workroom. \n", 1)

new_reset() {
    if (!present("torch")) {
        object torch;
        torch = clone_object("obj/torch");
        call_other(torch, "set_name", "torch");
        call_other(torch, "set_fuel",9999);
        call_other(torch, "set_weight", 1);
        move_object(torch, this_object());
    }
}
