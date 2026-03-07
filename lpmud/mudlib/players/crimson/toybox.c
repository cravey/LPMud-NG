#define MAX_WEIGTH	9
int local_weight;

long() {
    write("A toybox. ");
    if (first_inventory(this_object()))
        write("There seems to be something in it.\n");
    else
        write("You can put something in here if you choose.\n");
}

reset(arg) {
    if (arg)
	return;
    local_weight = 0;
}

query_weight() {
    return 10;
}

add_weight(w) {
    if (local_weight + w > MAX_WEIGTH)
	return 0;
    local_weight += w;
    return 1;
}

short() {
    return "A toybox";
}

id(str) {
    return str == "toybox" || str == "toy box" || str == "toy chest";
}

query_value() {
    return 75;
}

can_put_and_get() { return 1; }

get() {
    return 1;
}

prevent_insert() {
    if (local_weight > 0) {
        write("There are too many things in the toybox.\n");
	return 1;
    }
    return 0;
}
