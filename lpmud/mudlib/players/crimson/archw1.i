# 1 "players/crimson/archw1.c"
# 1 "/home/lpmud/mudlib/room/std.h"







































































































































































# 2 "players/crimson/archw1.c"
int found;
reset(arg)
{
    if (!arg)
	set_light(1);
    found = found -1;
    search();
}

id(str) {
    return str == "couch" || str == "sofa" || str == "divan";
}

init()
{
    add_action("east"); add_verb("east");
    add_action("south"); add_verb("south");
    add_action("sit"); add_verb("sit");
    add_action("look"); add_verb("look");
    add_action("search"); add_verb("search");
}

look(str) {
   if(str == "at couch" || str == "at sofa") {
   write("It looks nice and soft and cushy.\n");
   return 1;
   } else
   if(str == "at pedastal" || str == "at pulpit") {
   write("You see nothing special.\n");
   return 1;
   }
}

sit(str) {
   if (str == "on couch" || str == "on sofa" || str == "down" || str == "down on couch" || str == "sit down on sofa") {
   write("Ah, your feet are grateful to you!\n");
   return 1;
    }
}

search(arg) {
string money;
    if (!id(arg)) {
    write("What is it that you want to search?\n");
    return 1;
    } else
    if (found == 1) {
    write("There is nothing in the couch right now.\n");
    return 1;
    } else
    if (id(arg) && found <1) {
    write("You found something!\n");
    found = 1;
    if(!money || environment(money)) {
    money = clone_object("obj/money");
    call_other(money, "set_money", random(250));
    move_object(money, this_object());
    return 1;
    }
  }
}

east() {
     call_other(this_player(), "move_player", "east#players/crimson/greeting");
     return 1;
}

long()
{
    write("Rays of sunlight flow throgh the western bay windows and hit the floor\n");
    write("in breathtaking diamond shaped patterns. Several couches and sofas are\n");
    write("scattered about the room. Undoubtedly, this is the funeral service area.\n");
    write("In the far north corner of the room you see pedastal and a pulpit.\n");
    write("Two obvious exits are south and east.\n");

}

short() {
    return "Funeral service area";
}

south() {
    call_other(this_player(), "move_player", "south#players/crimson/back");
    return 1;
}
