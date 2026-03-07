int current_page;
int read_magic;
int magic;

reset()
{
    current_page = 0;
    magic = 1;
}

short() { return "A thick ancient tome"; }

long(str) {
     if (current_page == 0) {
    write("                   *****************************\n");
    write("                  **                           *\n");
    write("                 * *                           *\n");
    write("                *  *         AN ANCIENT        *\n");
    write("                *  *                           *\n");
    write("                *  *                           *\n");
    write("                *  *            TOME           *\n");
    write("                *  *                           *\n");
    write("                *  *                           *\n");
    write("                *  *             OF            *\n");
    write("                *  *                           *\n");
    write("                *  *                           *\n");
    write("                *  *           MAGICAL         *\n");
    write("                *  *                           *\n");
    write("                *  *                           *\n");
    write("                *  *           SPELLS          *\n");
    write("                *  *                           *\n");
    write("                *  *                           *\n");
    write("                *  *****************************\n");
    write("                * *---------------------------/\n");
    write("                **---------------------------/\n");
    write("                ******************************\n");
    return 1;
    } else
    if (current_page == 1) {
        cat("/players/crimson/contents");
        return 1;
    } else
    if (current_page == 3) {
        cat("/players/crimson/forward");
       return 1;
    } else
    if (current_page == 5) {
        cat("/players/crimson/read");
        return 1;
    } else
    if (current_page == 7) {
        cat("/players/crimson/rope");
        return 1;
    } else
    if (current_page == 9) {
        cat("/players/crimson/detect");
        return 1;
    } else 
    if (current_page == 11) {
        cat("/players/crimson/light");
        return 1;
    } else
    if (current_page == 13) {
        cat("/players/crimson/missile");
        return 1;
    }
}

init() {
    add_action("open"); add_verb("open");
    add_action("close"); add_verb("close");
    add_action("read_book"); add_verb("read");
    add_action("turn_page"); add_verb("turn");
    add_action("learn"); add_verb("learn");
    add_action("no_find"); add_verb("origin");
    add_action("learn"); add_verb("memorize");
}

id(str) { return str == "spellbook" || str == "tome" || str == "book"; }

open(str) {
    if (!id(str))
	return 0;
    if (current_page > 0) {
	write("The book is already open at page " + current_page + ".\n");
	return 1;
    }
    current_page = 1;
    cat("/players/crimson/contents");
    say(call_other(this_player(), "query_name", 0) +
	" opens a spellbook.\n");
    return 1;
}

no_find(str) {
   if (str == "spellbook" || str == "tome" || str == "book") {
   write("What?\n");
   return 1;
   }
}

learn(str) {
string level;
string magic;
object cold;
object rope;
object detect;
object light;
object missile;
int i;
   
    level = call_other(this_player(), "query_level", 0);
    magic = call_other(this_player(), "query_spell_point", 0);
    if (str == "read magic" && current_page != 5) {
    write("You have to get to the right page first!\n");
    return 1;
    } else
    if ((str == "read magic") && (magic<10)) {
    write("You do not have the constitution for that now.\n");
    return 1;
    } else
    if (str == "read magic" && read_magic == 1) {
    write("You can't learn read magic multiple times before learning another spell.\n");
    return 1;
    } else
    if (str == "read magic") {
    write("You memorize read magic.\n");
    call_other(this_player(), "add_spell_point", -(10));
    read_magic = 1;
    return 1;
    } else
    if (str == "rope trick") i = 2;
    if (str == "rope trick" && current_page != 7) {
    write("You have to get to the right page first!\n");
    return 1;
    } else
    if (str == "rope trick" && read_magic != 1) {
    write("You have to learn read magic first.\n");
    return 1;
    } else
    if (str == "rope trick" && (level<2)) {
    write("You are not high enough level to learn that.\n");
    return 1;
    } else
    if (str == "rope trick" && (magic<40)) {
    write("You don't have the constitution for that now.\n");
    return 1;
    } else
    if (str == "rope trick") {
    write("You memorize rope trick.\n");
    call_other(this_player(), "add_spell_point", -(40));
    rope = clone_object("players/crimson/.ropecast");
    move_object(rope, this_player());
    read_magic = 0;
    return 1;
    } else
    if (str == "detect magic") i = 3;
    if (str == "detect magic" && current_page != 9) {
    write("You have to get to the right page first.\n");
    return 1;
    } else
    if (str == "detect magic" && read_magic != 1) {
    write("You need to learn read magic first.\n");
    return 1;
    } else
    if (str == "detect magic" && (level<3)) {
    write("You are not high enough level to learn that.\n");
    return 1;
    } else
    if (str == "detect magic" && (magic<30)) {
    write("You don't have the constitution for that now.\n");
    return 1;
    } else
    if (str == "detect magic") {
    write("You memorize detect magic.\n");
    detect = clone_object("players/crimson/.detect");
    move_object(detect, this_player());
    call_other(this_player(), "add_spell_point", -(30));
    read_magic = 0;
    return 1;
    } else
    if (str == "light") i = 4;
    if (str == "light" && current_page != 11) {
    write("You have to get to the right page first.\n");
    return 1;
    } else
    if (str == "light" && read_magic != 1) {
    write("You have to learn read magic first.\n");
    return 1;
    } else
    if (str == "light" && (level<4)) {
    write("You are not high enough level to learn that.\n");
    return 1;
    } else
    if (str == "light" && (magic<30)) {
    write("You do not have the constitution for that now.\n");
    return 1;
    } else
    if (str == "light") {
    write("You learn light.\n");
    light = clone_object("players/crimson/.lightcast");
    move_object(light, this_player());
    call_other(this_player(), "add_spell_point", -(30));
    read_magic = 0;
    return 1;
    } else
    if (str == "magic missile") i = 5;
    if (str == "magic missile" && current_page != 13) {
    write("You have to get to the right page first.\n");
    return 1;
    } else
    if (str == "magic missile" && read_magic != 1) {
    write("You have to learn read magic first.\n");
    return 1;
    } else
    if (str == "magic missile" && (level<5)) {
    write("You are not high enough level to learn that.\n");
    return 1;
    } else
    if (str == "magic missile" && (magic<15)) {
    write("You don't have the constitution for that now.\n");
    return 1;
    } else
    if (str == "magic missile") {
    write("You learn magic missile.\n");
    missile = clone_object("players/crimson/.missile");
    move_object(missile, this_player());
    call_other(this_player(), "add_spell_point", -(15));
    read_magic = 0;
    return 1;
    } else
    if (str == "cone of cold" && current_page != 27) {
    write("You have to get to the right page first!\n");
    return 1;
    } else
    if (str == "cone of cold") i = 12;
    if (level<i) {
    write("You are not high enough level to learn that.\n");
    return 1;
    } else
    if (str == "cone of cold" && (magic<20)) {
    write("You do not have the constitution for that now.\n");
    return 1;
    } else
    if (str == "cone of cold" && read_magic != 1) {
    write("You haven't memorized read magic yet!\n");
    return 1;
    } else
    if (str == "cone of cold") {
    write("You memorize cone of cold.\n");
    cold = clone_object("players/crimson/.cold");
    move_object(cold, this_player());
    call_other(this_player(), "add_spell_point", -(20));
    read_magic = 0;
    return 1;
    }
}

close(str)
{
    if (!id(str))
	return 0;
    if (current_page == 0) {
	write("It is already closed.\n");
	return 1;
    }
    current_page = 0;
    say(call_other(this_player(), "query_name", 0) +
	" closes a spellbook.\n");
    long();
    return 1;
}

turn_page(str) {
    if (str == "page") {
    write("What page do you want to turn to?\n");
    return 1;
    } else
    if (!str) 
    return 0;
    if (current_page == 0) {
    write("The book is closed.\n");
    return 1;
    }
    if (str == "to page 1" || str == "to page 2" || str == "to page one" || str == "to page two") {
    cat("/players/crimson/contents");
    current_page = 1;
    return 1;
    }
    if (str == "to page 3" || str == "to page 4" || str == "to page three" || str == "to page four") {
    cat("/players/crimson/forward");
    current_page = 3;
    return 1;
    }
    if (str == "to page 5" || str == "to page 6" || str == "to page five" || str == "to page six") {
    cat("/players/crimson/read");
    current_page = 5;
    return 1;
    }
    if (str == "to page 7" || str == "to page 8" || str == "to page seven" || str == "to page eight") {
    cat("/players/crimson/rope");
    current_page = 7;
    return 1;
    }
    if (str == "to page 9" || str == "to page 10" || str == "to page nine" || str == "to page ten") {
    cat("/players/crimson/detect");
    current_page = 9;
    return 1;
    }
    if (str == "to page 11" || str == "to page 12" || str == "to page eleven" || str == "to page twelve") {
    cat("/players/crimson/light");
    current_page = 11;
    return 1;
    }
    if (str == "to page 13" || str == "to page 14" || str == "to page thirteen" || str == "to page fourteen") {
    cat("/players/crimson/missile");
    current_page = 13;
    return 1;
    }
    if (str == "to page 27" || str == "to page 28" || str == "to page twenty-seven" || str == "to page twenty-eight") {
    current_page = 27;
    return 1;
    }
}

read_book(str) {
    if (!id(str) && str != "page")
	return 0;
    if (current_page == 0) {
	write("It is closed.\n");
	return 1;
    }
    if (current_page == 1) {
        cat("/players/crimson/contents");
        return 1;
    }
    if (current_page == 3) {
        cat("/players/crimson/forward");
        return 1;
     }
     if (current_page == 5) {
         cat("/players/crimson/read");
         return 1;
     }
     if (current_page == 7) {
     cat("/players/crimson/rope");
     return 1;
     }
     if (current_page == 9) {
     cat("/players/crimson/detect");
     return 1;
     }
     if (current_page == 11) {
     cat("/players/crimson/light");
     return 1;
     }
     if (current_page == 13) {
     cat("/players/crimson/missile");
     return 1;
     }
}

get() { return 1; }

query_weight() { return 9; }

query_value() { return 0; }

query_magic() { return magic; }
