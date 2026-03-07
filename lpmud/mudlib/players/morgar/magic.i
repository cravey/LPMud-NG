# 1 "players/morgar/magic.c"
# 1 "<built-in>" 1
# 1 "<built-in>" 3
# 465 "<built-in>" 3
# 1 "<command line>" 1
# 1 "<built-in>" 2
# 1 "players/morgar/magic.c" 2
# 1 "./room/std.h" 1









# 37 "./room/std.h"


# 73 "./room/std.h"


# 117 "./room/std.h"


# 2 "players/morgar/magic.c" 2






reset(arg) { if (arg) return; set_light( 1); }short() { if (set_light(0)) return 
         
# 8 "players/morgar/magic.c"
"The magic shop"; return "dark room";}init() { add_action("move",  "south"); add_action("buy"); add_verb("buy"); add_action("list"); add_verb("list");}move() { call_other(this_player(), "move_player",  "south" + "#" +"room/yard"); return 1;}long(str) { if (set_light(0) == 0){ write("It is dark.\n"); return; } write(
         
# 8 "players/morgar/magic.c"
"You are in a magic shop. You can buy spells and items here.\n" +
         
# 8 "players/morgar/magic.c"
"Commands are: 'buy item', 'list',\n"); write("    The only obvious exit is " +  "south" + ".\n");}




buy(item) {
    if (!item)
        return 0;
    call_other("players/morgar/magicstore", "fill", 0);
    call_other("players/morgar/magicstore", "buy", item);
    return 1;
}

list(obj) {
    call_other("players/morgar/magicstore", "fill", 0);
    call_other("players/morgar/magicstore", "inventory", obj);
    return 1;
}

find_item_in_player(i)
{
    object ob;

    ob = first_inventory(this_player());
    while(ob) {
        if (call_other(ob, "id", i))
            return ob;
        ob = next_inventory(ob);
    }
    return 0;
}

