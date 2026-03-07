string long;

short() {
    return "A fingered manual" ;
}

long() {
    write("*****************************\n");
    write("*   This is the Manual of   *\n");
    write("*        the Crystal        *\n");
    write("*     Brought ot you by     *\n");
    write("*  THE SOCIETY FOR BETTER   *\n");
    write("*        ADVENTURING        *\n");
    write("*                           *\n");
    write("* You say you are tired of  *\n");
    write("*  showing up only to find  *\n");
    write("* that your monster is not  *\n");
    write("*  there? Well here is the  *\n");
    write("*  the answer.  The handy   *\n");
    write("*  dandy monster tracker.   *\n");
    write("*  Yes, the crystal knows   *\n");
    write("*  all.  Just point it at   *\n");
    write("*  the monster you want to  *\n");
    write("*  find and gaze into the   *\n");
    write("*  crystal. If he's there   *\n");
    write("*  the crystal will show    *\n");
    write("*  you.                     *\n");
    write("*     'point <monster>'     *\n");
    write("* 'gaze into crystal ball'  *\n");
    write("*****************************\n");
    
}

init() {
    add_action("read"); add_verb("read");
}

id(str) {
    return str == "manual" || str == "manual of the crystal";
}

read(str) {
    if (!id(str))
	return 0;
    say(call_other(this_player(), "query_name") + " reads a manual.\n");
    long();
    return 1;
}

query_weight() { return 1; }

get() { return 1; }

query_value() { return 50; }
