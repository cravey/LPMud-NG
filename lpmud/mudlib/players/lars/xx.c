int delay;
int a, b, c;

short() { return "xx"; }
id(str) { return str == "xx"; }
get() { return 1; }

apa(str) {
    int i;
    destruct(this_object());
    return 1;
}

static bepa()
{
    int apa;
    apa = ({1, "aa", 2});
    write(apa[0] + "\n");
    write(apa[1] + "\n");
    write(apa[2] + "\n");
    return 1;
}

init() {
    add_action("apa", "apa");
    add_action("bepa", "bepa");
}

xxx(str) {
    if (query_verb() == "hello" || query_verb() == "bepa") {
	write("Yes !\n");
	return 1;
    }
    write("Fail: " + query_verb() + "\n");
    return 0;
}

hello(arg) {
    say("hello: " + arg + "\n");
    write(previous_object());
}

cepa(arg) {
    if (intp(arg))
	write("int\n");
    else if (stringp(arg))
	write("string\n");
    else if (objectp(arg))
	write("object\n");
    else if (pointerp(arg))
	write("pointer");
    else
	write("unknown\n");
}

reset() {
    int i;
    a = 1;
    b = 2;
    c = 3;
    delay = 30;
}

garbage() {
    int apa;
    apa = combine_free_list();
    if (environment() && living(environment()))
	tell_object(environment(), "" + apa + "\n");
    call_out("garbage", delay);
}

bad_code() {
}

query_weight() {
    return 5;
}
long() {
    write("This is Lars' test object.\n");
}
