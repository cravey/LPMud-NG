# 1 "players/morgar/say.c"
# 1 "<built-in>" 1
# 1 "<built-in>" 3
# 465 "<built-in>" 3
# 1 "<command line>" 1
# 1 "<built-in>" 2
# 1 "players/morgar/say.c" 2
id(str) { return str == "say"; }

short() { return "say with echo and quotes"; }

query_value() { return 1; }

long() {
    write("This is the echoer.\n");
}

init() { add_action("talk"); add_verb("say"); }


talk(str) {
    if (str==0) {
        say(call_other(this_player(),"query_name",0)+" has nothing to say.\n");
        write("You have nothing to say.\n");
        return 1;
    }
    say(call_other(this_player(),"query_name",0)+" says \""+str+"\"\n");
    write("You say \""+str+"\"\n");
    return 1;
}

get() { return 1; }

