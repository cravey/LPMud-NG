short() { return "A large heavy stone"; }
long() {
    write(
"The stone is extremely heavy.  It is about 1 meter in diameter and is\n"+
"solid granite.\n");
}

id(str) { return str=="stone" || str=="rock" || str=="ringer_load_stone";}

query_weight() { return 1; }

get() {
    if (present("strength spell",this_player())) {
        return 1;
    }
    return 0;
}
