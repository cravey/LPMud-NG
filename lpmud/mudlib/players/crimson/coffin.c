#define MAX_WEIGTH	6
int local_weight;

long() {
    write("A flossy black coffin. ");
    if (first_inventory(this_object()))
        write("There seems to be something or someone here.\n");
    else
        write("You can put something here if you choose.\n");
}

reset(arg) {
    if (arg)
	return;
    local_weight = 0;
}

query_weight() {
    return 10000;
}

add_weight(w) {
    if (local_weight + w > MAX_WEIGTH)
	return 0;
    local_weight += w;
    return 1;
}

short() {
    return "A coffin";
}

id(str) {
    return str == "coffin";
}

query_value() {
    return 12;
}

can_put_and_get() { return 1; }

get() {
    return 1;
}

prevent_insert() {
    if (local_weight > 0) {
	write("You can't when there are things in the bag.\n");
	return 1;
    }
    return 0;
}
