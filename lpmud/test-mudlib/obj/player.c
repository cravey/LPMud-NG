string name;
string cap_name;
int level;
int local_weight;
int ping_hits;
string last_probe_env;

object active_runner;
object input_probe_runner;
string input_probe_expected;

reset(arg) {
    if (arg)
        return;
    name = "logon";
    cap_name = "Logon";
    level = 20;
    local_weight = 0;
    ping_hits = 0;
    last_probe_env = "0";
    active_runner = 0;
    input_probe_runner = 0;
    input_probe_expected = 0;
}

logon() {
    write("LPmud engine test mudlib.\n");
    write("Login name (lowercase letters): ");
    input_to("finish_logon");
    return 1;
}

valid_login_name(str) {
    int i, n;
    if (!str || str == "")
        return 0;
    n = strlen(str);
    if (n > 11)
        return 0;
    for (i = 0; i < n; i += 1) {
        if (str[i] < 'a' || str[i] > 'z')
            return 0;
    }
    return 1;
}

finish_logon(str) {
    if (!str || str == "")
        str = "tester";
    str = lower_case(str);
    if (!valid_login_name(str)) {
        write("Invalid login name. Use only a-z, max length 11.\n");
        write("Login name (lowercase letters): ");
        input_to("finish_logon");
        return;
    }

    enable_commands();
    name = str;
    cap_name = capitalize(str);
    set_living_name(name);
    install_commands();

    move_object(this_object(), "room/church");
    write("Logged in as " + cap_name + ".\n");
    write("Type 'help' for commands. Type 'runtests' to run engine checks.\n");
    look_room();
}

install_commands() {
    add_action("cmd_help", "help");
    add_action("cmd_quit", "quit");
    add_action("cmd_look", "look");
    add_action("cmd_say", "say");
    add_action("cmd_score", "score");
    add_action("cmd_who", "who");
    add_action("cmd_ping", "ping");
    add_action("cmd_runtests", "runtests");
    add_action("cmd_teststatus", "teststatus");
    add_action("cmd_probeenv", "probeenv");
    add_action("begin_input_probe_cmd", "begin_input_probe");
}

cmd_help() {
    write("Commands:\n");
    write("  look [at <target>]\n");
    write("  say <msg>\n");
    write("  who\n");
    write("  score\n");
    write("  ping\n");
    write("  runtests\n");
    write("  teststatus\n");
    write("  probeenv\n");
    write("  quit\n");
    write("Movement is room-defined (east/west in this mudlib).\n");
    return 1;
}

cmd_quit() {
    write("Saving " + cap_name + ".\n");
    destruct(this_object());
    return 1;
}

look_room() {
    object env, ob;
    string sh;

    env = environment(this_object());
    if (!env) {
        write("You are nowhere.\n");
        return 1;
    }
    call_other(env, "long", 0);
    ob = first_inventory(env);
    while (ob) {
        if (ob != this_object()) {
            sh = call_other(ob, "short");
            if (sh)
                write(sh + ".\n");
        }
        ob = next_inventory(ob);
    }
    return 1;
}

cmd_look(str) {
    string what;
    object ob, env;

    if (!str || str == "")
        return look_room();

    if (sscanf(str, "at %s", what) != 1)
        what = str;

    env = environment(this_object());
    ob = present(what, this_object());
    if (!ob && env)
        ob = present(what, env);
    if (ob) {
        call_other(ob, "long");
        return 1;
    }

    if (env) {
        call_other(env, "long", what);
        return 1;
    }
    write("You see nothing special.\n");
    return 1;
}

cmd_say(str) {
    if (!str || str == "") {
        write("Say what?\n");
        return 1;
    }
    write("Ok.\n");
    say(cap_name + " says: " + str + "\n");
    return 1;
}

cmd_score() {
    write("Name: " + cap_name + "\n");
    write("Level: " + level + "\n");
    write("Carry weight: " + local_weight + "\n");
    write("Ping hits: " + ping_hits + "\n");
    return 1;
}

cmd_who() {
    write(cap_name + " the engine tester (level " + level + ")\n");
    return 1;
}

cmd_ping() {
    ping_hits += 1;
    write("pong\n");
    return 1;
}

query_ping_hits() { return ping_hits; }

cmd_teststatus() {
    if (active_runner)
        write("Test suite: running\n");
    else
        write("Test suite: idle\n");
    return 1;
}

cmd_probeenv() {
    object env;

    env = environment(this_player());
    if (env)
        last_probe_env = file_name(env);
    else
        last_probe_env = "0";
    write("probeenv: " + last_probe_env + "\n");
    return 1;
}

cmd_runtests() {
    if (active_runner) {
        write("A test suite is already running.\n");
        return 1;
    }
    active_runner = clone_object("obj/test_runner");
    if (!active_runner) {
        write("Failed to clone test runner.\n");
        return 1;
    }
    call_other(active_runner, "start_suite", this_object());
    return 1;
}

suite_finished(runner) {
    if (runner == active_runner)
        active_runner = 0;
    input_probe_runner = 0;
    input_probe_expected = 0;
    return 1;
}

arm_input_probe(runner, expected) {
    input_probe_runner = runner;
    input_probe_expected = expected;
    return 1;
}

begin_input_probe_cmd() {
    if (!input_probe_runner || !input_probe_expected) {
        write("[FAIL][ASYNC-005] input_to setup missing probe context.\n");
        return 1;
    }
    write("[ACTION][ASYNC-003] Enter token: " + input_probe_expected + "\n");
    input_to("input_probe_cb");
    return 1;
}

input_probe_cb(str) {
    object runner;
    string expected;

    runner = input_probe_runner;
    expected = input_probe_expected;
    input_probe_runner = 0;
    input_probe_expected = 0;

    if (!runner) {
        write("[FAIL][ASYNC-005] input_to callback lost runner context.\n");
        return;
    }
    call_other(runner, "receive_input_probe", str, expected);
}

move_player(dir_dest) {
    string dir, dest;
    if (sscanf(dir_dest, "%s#%s", dir, dest) != 2) {
        write("Bad move format: " + dir_dest + "\n");
        return 1;
    }
    move_object(this_object(), dest);
    look_room();
    return 1;
}

query_real_name() { return name; }
query_name() { return cap_name; }
query_level() { return level; }
query_last_probe_env() { return last_probe_env; }
short() { return cap_name + " (engine tester)"; }
id(str) { return str == name || str == "player"; }

add_weight(w) {
    if (local_weight + w < 0)
        return 0;
    local_weight += w;
    return 1;
}

sanitize_path(path) {
    string a, b;
    if (!path || path == "")
        return "data/invalid";
    while (path[0] == '/')
        path = extract(path, 1);
    if (!path || path == "")
        return "data/invalid";
    if (sscanf(path, "%s..%s", a, b) == 2 || path == "..")
        return "data/invalid";
    return path;
}

valid_read(path) {
    return sanitize_path(path);
}

valid_write(path) {
    return sanitize_path(path);
}

catch_tell(str) {
    write(str);
}
