# 1 "obj/quest_obj.c"
# 1 "<built-in>" 1
# 1 "<built-in>" 3
# 465 "<built-in>" 3
# 1 "<command line>" 1
# 1 "<built-in>" 2
# 1 "obj/quest_obj.c" 2



   

string hint_string, name;

set_hint(h) {
    hint_string = h;
}

set_name(n) {
    name = n;
}

id(str) { return str == name || str == "quest"; }

short() {
    return name;
}

long() {
    write("This is the quest '" + name + "':\n");
    write(hint_string);
}

hint() { return hint_string; }

