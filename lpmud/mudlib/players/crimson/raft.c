#include "std.h"
/*
 * This is a raft.
 * docked on: docks
 */

#define STILL	0
#define DOWN	1
#define UP	2

int level;
int raft_is_anchored;
int time_to_drop_anchor;
int dest;		/* Where we are going. */
int altdest;            /* An alternate name for our destination. */
int moving_time;	/* How long we are going to move. */
int delay_to_reset;	/* Move back to origin automatically after a delay. */

init() {
    add_action("row"); add_verb("row");
    add_action("row"); add_verb("paddle");
    add_action("drop_anchor"); add_verb("drop");
    add_action("weigh_anchor"); add_verb("weigh");
    add_action("go_east"); add_verb("east");
}

short() {
    return "raft";
}

reset(arg) {
    raft_is_anchored = 0;
    if (arg)
	return;
    set_light(1);
    level = 1;
    dest = 1;
    moving_time = 0;
}

/*
 * Return true if weighed anchor.
 */

query_anchor() { return !raft_is_anchored; }

long() {
    write("You are in the elevator. On the wall are three tos,\n");
    write("numbered 1 to 3.\n");
    if (raft_is_anchored)
        write("The anchor is dropped.\n");
    if (!raft_is_anchored)
        write("The anchor is weighed.\n");
}

row(to) {
    string b;
    int docks;
    int sandbar;
    int cove;
    if (!to)
	return 0;
    if (raft_is_anchored) {
	write("The raft is still anchored.\n");
	return 1;
    }
    if (sscanf(to, "to %s", b) != 1)
	b = to;
    if (moving_time > 0) {
	write("The raft is still moving.\n");
	return 1;
    }
    if (b == "docks" || b == "docks")
	dest = 1;
        altdest = docks;
    if (b == "sandbar" || b == "bar")
	dest = 2;
        altdest = sandbar;
    if (b == "cove" || b == "alcove")
	dest = 3;
        altdest = cove;
    if (dest == level) {
        write("You are already there!\n");
	return 1;
    }
    if (dest > level) {
	moving_time = dest - level;
	write("The raft jerks away from the dock.\n");
	say("The raft jerks away from the dock.\n");
    }
    if (level > dest) {
	moving_time = level - dest;
	write("The raft starts moving downstream.\n");
	say("The raft starts moving downstream.\n");
    }
    if (dest == 1 || level == 1)
	moving_time += 10;
    moving_time += 1;
    set_heart_beat(1);
    return 1;
}

heart_beat() {
    if (time_to_drop_anchor > 0) {
	time_to_drop_anchor -= 1;
	if (time_to_drop_anchor == 0) {
            say("The raft starts drifting away.\n");
	    raft_is_anchored = 0;
	}
    }
    if (moving_time <= 0)
	return;
    moving_time -= 1;
    if (moving_time > 0) {
	say("The raft floats along quietly...\n");
	return;
    }
    say("The raft coasts into the dock.\n");
    set_heart_beat(0);
    level = dest;
    if (level == 2)
	call_other("room/church", "elevator_arrives", 0);
    if (level == 1)
	call_other("room/wiz_hall", "elevator_arrives", 0);
}

drop_anchor(str)
{
    if (str != "anchor")
	return 0;
    if (raft_is_anchored) {
	write("It is already dropped!\n");
	return 1;
    }
    if (moving_time > 0) {
	write("The anchor is stuck.\n");
	return 1;
    }
    raft_is_anchored = 1;
    time_to_drop_anchor = 3;
    write("Ok.\n");
    say(call_other(this_player(), "query_name", 0) + " boarded the raft.\n");
    return 1;
}

weigh_anchor(str)
{
    if (str != "anchor")
	return 0;
    if (!raft_is_anchored) {
        write("It would be wise to drop anchor before leaving.\n");
	return 1;
    }
    raft_is_anchored = 0;
    time_to_drop_anchor = 0;
    write("Ok.\n");
    say(call_other(this_player(), "query_name", 0) + " weighed the anchor.\n");
    return 1;
}

go_east() {
    if (moving_time > 0) {
	write("You can't go anywhere when the elevator is moving.\n");
	return 1;
    }
    if (!raft_is_anchored) {
        write("It would be wise to drop anchor before leaving.\n");
	return 1;
    }
    if (level == 2)
	call_other(this_player(), "move_player", "east#room/church");
    if (level == 1)
	call_other(this_player(), "move_player", "east#room/wiz_hall");
    if (level == 3)
	call_other(this_player(), "move_player", "east#room/attic");
    return 1;
}

query_level() { return level; }

/*
 * This routine is called from various rooms that the elevator connects to.
 */
call_elevator(to) {
    if (raft_is_anchored)
	return 0;
    if (moving_time > 0)
	return 0;
    dest = to;
    if (dest == level)
	return 0;
    write("A little white lamp beside the to lights up.\n");
    say("A little white lamp beside the to lights up.\n");
    if (dest > level)
	moving_time = dest - level;
    if (level > dest)
	moving_time = level - dest;
    if (dest == 1 || level == 1)
	moving_time += 10;
    moving_time += 1;
    set_heart_beat(1);
    return 1;
}

id(str) {
    return str == "anchor" || str == "to" || str == "tos";
}

/*
 * Only list inventory if not looking at anything special.
 */
can_put_and_get()
{
    return 0;
}
/*
 * Called by others to see if the elevator is moving
 */
is_moving() { 
    if (level == dest )
	/* Still */
	return 0;
    if(level > dest)
	/* down */
	return 1;
    /* up */
    return 2;
}
