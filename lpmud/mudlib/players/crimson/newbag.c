#define MAX_WEIGTH	6
int n;
string name;
object where;
string disbag;
object bag;
object player;
int local_weight;

reset(arg) {
    set_light(1);
    if (arg)
	return;
    local_weight = 0;
}

long(str) {

     player = this_player();
     name = call_other(this_player(), "query_name", 0);
     where = environment(this_player());
     disbag = this_object();
     n = random(4);
     if (n == 1 && where != disbag) {
     move_object(disbag, where);
     write("You are sucked into a bag of devouring!\n");
     say(name+" is sucked into a bag of devouring!\n");
     move_object(player, disbag);
     bag = clone_object("obj/monster");
     call_other(bag, "set_name", "bag");
     call_other(bag, "set_short");
     call_other(bag, "set_level", 10);
     call_other(bag, "set_eps", 10000);
     call_other(bag, "set_ac", 3);
     call_other(bag, "set_wc", 12);
     call_other(bag, "set_long");
     call_other(bag, "set_alias", "creature");
     move_object(bag, disbag);
     call_other(player, "attack_object", bag);
     return 1;
     } else
     if (!id(str)) {
     return 0;
     } else
     write("A bag. ");
     if (first_inventory(this_object())) {
        write("There is something in it.\n");
     } else
     if (!first_inventory(this_object())) {
        write("You can put things in it.\n");
      return 1;
      }
}

query_where() {
object where;
       where = environment(this_player());
       return where;
}

init() {
    add_action("climb"); add_verb("climb");
    add_action("sell"); add_verb("sell");
    add_action("look"); add_verb("look");
    add_action("exit"); add_verb("exit");
}

here() {
object dest;
object player;
 
     dest = query_where();
     player = this_player();
     move_object(player, dest);
     return 1;
}

look(str) {
    if (!str && environment(this_player()) == this_object() && present("creature")) {
    write("You are inside a bag of devouring.\n");
    write("To exit 'climb out'.\n");
    return 0;
    } else
    if(!str && environment(this_player()) == this_object()) {
    write("You are inside the remains of a bag of devouring.\n");
    write("In reality, it is one of the feeding orifices of\n");
    write("an extra-dimensional creature.\n");
    write("The obvious thing to do here is 'climb out'.\n");
    return 0;
    } else
    if (str) {
    return 0;
    } else
    return 0;
}

climb(str) {
     if (str == "out" && (environment(this_player()) == this_object())) {
     write("Someone casts phase door and pulls you out of the ethereal plane.\n");
     move_object(this_player(), environment(this_object()));
     move_object(this_object(), "players/crimson/storeroom");
     destruct(this_object());
     call_other(this_player(), "look", 0);
     return 1;
     }
}

exit(str) {
   if (str != "climb out") {
   return 1;
   }
   return 1;
}

sell(str) {
    if (str == "bag") {
    write("You get 12 coins.\n");
    move_object(disbag, "players/crimson/storeroom");
    destruct(disbag);
    destruct(player);
    destruct(this_player());
    call_other(this_player(), "add_money", 12);
    return 1;
    }
}

query_weight() {
    return 1;
}

add_weight(w) {
    if (local_weight + w > MAX_WEIGTH)
	return 0;
    local_weight += w;
    return 1;
}

short() {
    return "bag";
}

id(str) {
    return str == "bag";
}

query_value() {
    return 0;
}

can_put_and_get() { return 1; }

get() {
    return 1;
}

prevent_insert() {
    if (local_weight > 0) {
        write("There are too many things in the bag.\n");
	return 1;
    }
    return 0;
}
