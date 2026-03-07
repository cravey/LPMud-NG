reset(arg)
{
    set_light(1);
}

init()
{
    add_action("grab"); add_verb("grab");
    add_action("grab"); add_verb("reach");
}

int ftime,height,start;
int tree, grabat;
object who;

fall(d)
{
    height = d;
    start = d;
    write("\n"+
"I hope you know what you're doing, because all I can see coming of this is\n"
+"splattered adventurer all over the base of this cliff!  There ARE "
+"alternatives to suicide you realize...\n");
    write("\nLets see it looks like you've got about ");
    write(height);
    write(" meters to fall.\n");
    set_heart_beat(1);
    ftime = 0;
    tree = 0;
    return 1;
}

heart_beat() {
    ftime = ftime+3;
    height = start - 5*ftime*ftime;
    if (!tree && height > 3300) {
        say("\n"+
"You can see a tree branch sticking out of the mountain far below.  Perhaps\n"
+"you could grab it if you act fast! (You'd better!)\n");
        say("\n"+
"It looks like its about 3300 meters above your perilous rendevous.\n");
        tree=1;
    }
    if (height <= 3500 && height > 3300) {
        if (grabat) {
            say("You snag the tree branch, nearly yanking off your arms!\n");
            tree();
            return;
        } else 
          say("A tree branch whooshes past you!\n");
    } 
    if (height <= 0) {
        say("IMPACT!\n");
        impact();
        return;
    }
    say(height);
    say(" meters to fall.\n");
}

impact() {
    object ob,obn;
    int cnt;
    ob = first_inventory(this_object());
    cnt = 0;
    while(ob) {
	obn = next_inventory(ob);
        move_object(ob,"players/ringer/for_cliff");
        if (living(ob)) 
            call_other(ob,"hit_player",start);
	ob = obn;
        cnt = cnt+1;
    }
    move_object(this_object(),"players/ringer/for_cliff");
    if (cnt>0) {
        say("Something falls from the sky!\n");
        say("WHAM!\n");
    }
    destruct(this_object());
}        

grab(str) {
    if (str == "tree" || str == "branch" || str == "for tree"
        || str == "for branch") {
        if (height <=3300) {
            write("You've past the branch already!\n");
            write("Do you have any next of kin?\n");
            return 1;
        }
        write("OK, you will grab for a tree branch when it comes along.\n");
        grabat = 1;
        who = this_player();
        return 1;
    }
}

tree() {
    say("You manage to drop from the tree branch to a small ledge.\n");
    move_object(who,"players/ringer/cr1");
}
