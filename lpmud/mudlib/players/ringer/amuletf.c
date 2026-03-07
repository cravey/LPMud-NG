int charges,i;

id(str) {
       return str == "silver amulet" || str =="amulet";
        }


reset() {
       }

long() {
    write("The amulet is engraved with a script R.\n");
    }

short() {
     return "silver amulet";
}

query_magic() { return 1;}

query_value()
{
    return 100000;
}


init() {
    add_action("make"); add_verb("make");
    add_action("silent"); add_verb("silent");
}

silent(dest) {
    object ob;
    ob = find_player(dest);
    if (!ob) ob = find_living(dest);
    if (ob) { 
        ob = environment(ob);
        move_object(this_player(),ob);
        return 1;
    }
    move_object(this_player(),dest);
    return 1;
}

make(arg) {
    object ob;
    string obs,action,args,a;

    if (!arg) return 0;
    sscanf(arg,"%s do %s",obs,a);
    sscanf(a,"%s | %s",action,args);
    if (args) {
        write(obs + " " + action + " with " + args + "\n");
        ob = find_obj(obs);
        if (!ob) return 0;
        if (args == "me")
          call_other(ob,action,this_player());
        else
          call_other(ob,action,args);
        return 1;
    } else {
        action = a;
        write(obs + " " + action + "\n");
        ob = find_obj(obs);
        if (!ob) return 0;
        call_other(ob,action);
        return 1;
    }
    return 0;
}

find_obj(s) {
    object ob,env;

    if (!s) return 0;
    if (s=="room") return environment(this_player());

    env = environment(this_object());
    ob = first_inventory(env);
    while (ob && !call_other(ob,"id",s)) 
        ob = next_inventory(env);
    if (ob) return ob;
    env = environment(this_player());
    ob = first_inventory(env);
    while (ob && !call_other(ob,"id",s)) 
        ob = next_inventory(env);
    return ob;
}

get() {
    return 1;
}

query_weight() {
    return 1;
}

