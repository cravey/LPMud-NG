string name, alias, short_desc, long_desc, value, weight;
int time_to_heal,membership;

reset(arg)
{
    time_to_heal = 10;
    if(arg)
	return;
    set_heart_beat(1);
    set_name("aa badge");
    set_alias("badge");
    set_short("aa badge");
    set_long(
"The badge says: Will power and negative reenforcement will prevail!\n");
    membership = 100;
}

heart_beat() {
    int hp;

    if (!living(environment())) dispell();
    membership = membership-1;
    if (membership<=0) 
      dispell();
    if (call_other(environment(),"query_intoxination")) {
        tell_object(environment(),"Your badge shocks you. OUCH!\n");
        hp = call_other(environment(),"query_hp");
        if (hp>5) {
            call_other(environment(),"heal_self",-2);
        }
        return;
    }
    time_to_heal = time_to_heal - 1;
    if (time_to_heal<=0) {
        call_other(environment(),"heal_self",3);
        time_to_heal=10;
    }
}


init() {
    add_action("remove"); add_verb("remove");
}

dispell() {
    tell_object(environment(),"The badge disappears.\n");
    destruct(this_object());
}


short() {
    return short_desc + " (worn)";
    return short_desc;
}

long(str) {
    write(long_desc);
}

id(str)
{
    return str == name || str == alias;
}

drop(silent) {
    if (call_other(environment(),"query_intoxination")) {
        if (!silent) write("The badge stubbornly refuses to go!\n");
        return 1;
    }
    if (!silent) 
      dispell();
    else 
      destruct(this_object());
    return 1;
}

query_value() { return value; }

query_name() { return name; }

get() { return 1; }

remove(str) {
    if (!id(str))
	return 0;
    if (call_other(environment(),"query_intoxination")) {
        write("The badge stubbornly refuses to be removed!\n");
        return 1;
    }
    destruct(this_object());
    return 1;
}

query_weight() { return weight; }

set_name(n) { name = n; set_short(n);}
set_short(s) { short_desc = s; long_desc = s + ".\n"; }
set_value(v) { value = v; }
set_weight(w) { weight = w; }
set_alias(a) { alias = a; }
set_long(l) { long_desc = l; }
