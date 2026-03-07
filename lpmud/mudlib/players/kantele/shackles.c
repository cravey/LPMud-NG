int worn,i;
object worn_by;

id(str) {
       return str == "shackles";
        }

init() {
    add_action("wear"); add_verb("wear");
    add_action("move"); add_verb("north");
    add_action("move"); add_verb("south");
    add_action("move"); add_verb("east");
    add_action("move"); add_verb("west");
    add_action("move"); add_verb("up");
    add_action("move"); add_verb("down");
    add_action("kill"); add_verb("kill");
    add_action("magic"); add_verb("fireball");
    add_action("magic"); add_verb("shock");
    add_action("magic"); add_verb("missile");
    add_action("magic"); add_verb("heal");
}

long() {
    write("Some strange arm and leg shackles.\n");
    }

short() {
    if (worn)
        return "Some strange shackles"  + " (worn)";
    return "Some strange shackles";
}

query_value()
{
    return 0;
}

wear(str)
{
    if (!id(str))
        return 0;
    if (environment() != this_player()) {
       write("You must get it first!\n");
       return 1;
    }
    if (worn) {
        write("You alread wear it!\n");
        return 1;
    }
   worn_by = this_player();
    call_other(this_player(), "wear", this_object());
  worn = 1;
    return 1;
        call_other(worn_by, "stop_wearing", 0);
        worn = 0;
        worn_by = 0;
}

drop(silently) {
    if (worn  && (call_other(this_player(), "query_level", 0) < 20))  {
   write("The shackles is impossible to get of.\n");
    return 1;
  }
    return 0;
}

get() {
   worn_by = this_player();
    call_other(this_player(), "wear", this_object());
  worn = 1;
    return 1;
}

query_weight() {
    return 1;
}

query_name() {
   return "shackles";
   }

magic() {
      if (call_other(this_player(), "query_level", 0) > 20) {
      return 0;
      } else {
write("You can't use magic while shackled.\n");
return 1;
}
}

move() {
      if (call_other(this_player(), "query_level", 0) > 20) {
      return 0;
      } else {
return 1;
}
}

kill() {
write("You can't kill somebody while you are  shackled.\n");
return 1;
}
