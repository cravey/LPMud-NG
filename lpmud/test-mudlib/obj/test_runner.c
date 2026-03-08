object owner;
object hb_probe;

int suite_running;
int total_tests;
int passed_tests;
int failed_tests;
int started_at;

int async_callout_seen;
int cancelled_callout_seen;
int input_probe_done;
string expected_input;
string async_order;

announce(msg) {
    if (owner)
        tell_object(owner, msg);
}

record_pass(id, desc) {
    total_tests += 1;
    passed_tests += 1;
    announce("[PASS][" + id + "] " + desc + "\n");
}

record_fail(id, desc, detail) {
    if (!detail)
        detail = "no detail";
    total_tests += 1;
    failed_tests += 1;
    announce("[FAIL][" + id + "] " + desc + " :: " + detail + "\n");
}

start_suite(ob) {
    if (suite_running) {
        tell_object(ob, "[FAIL][HARNESS-000] Test suite already running.\n");
        return 1;
    }
    owner = ob;
    suite_running = 1;
    total_tests = 0;
    passed_tests = 0;
    failed_tests = 0;
    started_at = time();
    async_callout_seen = 0;
    cancelled_callout_seen = 0;
    input_probe_done = 0;
    expected_input = "";
    async_order = "";

    announce("[INFO][HARNESS] Engine test suite started.\n");
    announce("[INFO][HARNESS] Synchronous checks running...\n");

    test_arithmetic_loop();
    test_factorial();
    test_array_and_types();
    test_string_ops();
    test_sscanf_parse();
    test_random_range();
    test_time_ctime();

    test_users_lookup();
    test_clone_and_previous_object();
    test_transfer_and_inventory();
    test_command_dispatch();
    test_file_save_restore();
    test_error_control_flow();
    test_find_object_semantics();
    test_move_object_error_paths();
    test_filesystem_efuns();
    test_messaging_efuns();
    test_network_metadata();
    test_environment_probe_command();
    test_light_accounting();
    test_bit_efuns();

    announce("[INFO][HARNESS] Asynchronous checks running (call_out, heart_beat, input_to)...\n");
    start_async_phase();
    return 1;
}

test_arithmetic_loop() {
    int i, sum;
    sum = 0;
    for (i = 1; i <= 10; i += 1)
        sum += i;
    if (sum == 55)
        record_pass("CORE-001", "for-loop arithmetic and integer math");
    else
        record_fail("CORE-001", "for-loop arithmetic and integer math", "expected 55, got " + sum);
}

fact(n) {
    if (n <= 1)
        return 1;
    return n * fact(n - 1);
}

test_factorial() {
    int got;
    got = fact(6);
    if (got == 720)
        record_pass("CORE-002", "recursive function calls");
    else
        record_fail("CORE-002", "recursive function calls", "expected 720, got " + got);
}

test_array_and_types() {
    int ok;
    ok = sizeof(({ "alpha", 17, ({ "x", "y" }) })) == 3 &&
         stringp(({ "alpha", 17, ({ "x", "y" }) })[0]) &&
         intp(({ "alpha", 17, ({ "x", "y" }) })[1]) &&
         pointerp(({ "alpha", 17, ({ "x", "y" }) })[2]) &&
         objectp(this_object());
    if (ok)
        record_pass("CORE-003", "arrays, allocate, sizeof, type predicates");
    else
        record_fail("CORE-003", "arrays, allocate, sizeof, type predicates", "array/type checks failed");
}

test_string_ops() {
    string joined, low, cap, slice;
    int ok;

    joined = implode(explode("a,b,c", ","), ":");
    low = lower_case("HeLLo");
    cap = capitalize("world");
    slice = extract("abcdef", 2, 4);
    ok = joined == "a:b:c" && low == "hello" && cap == "World" && slice == "cde";
    if (ok)
        record_pass("CORE-004", "explode/implode/lower_case/capitalize/extract");
    else
        record_fail("CORE-004", "explode/implode/lower_case/capitalize/extract",
                    "joined=" + joined + ", low=" + low + ", cap=" + cap + ", slice=" + slice);
}

test_sscanf_parse() {
    string who, what;
    int matched;
    matched = sscanf("bob gives orb", "%s gives %s", who, what);
    if (matched == 2 && who == "bob" && what == "orb")
        record_pass("CORE-005", "sscanf parsing");
    else
        record_fail("CORE-005", "sscanf parsing", "matched=" + matched + ", who=" + who + ", what=" + what);
}

test_random_range() {
    int i, v, bad;
    bad = 0;
    for (i = 0; i < 200; i += 1) {
        v = random(7);
        if (v < 0 || v > 6)
            bad = 1;
    }
    if (!bad)
        record_pass("CORE-006", "random() range constraints");
    else
        record_fail("CORE-006", "random() range constraints", "generated value outside [0,6]");
}

test_time_ctime() {
    int now;
    string ct;
    now = time();
    ct = ctime(now);
    if (stringp(ct) && strlen(ct) > 0)
        record_pass("CORE-007", "time() and ctime()");
    else
        record_fail("CORE-007", "time() and ctime()", "ctime returned empty/non-string");
}

test_users_lookup() {
    object found;
    string rn;

    if (!pointerp(users()) || sizeof(users()) < 1)
        record_fail("EFUN-001", "users()", "expected at least one interactive user");
    else
        record_pass("EFUN-001", "users()");

    rn = call_other(owner, "query_real_name");
    found = find_player(rn);
    if (found == owner)
        record_pass("EFUN-002", "find_player()");
    else
        record_fail("EFUN-002", "find_player()", "did not resolve active player");

    found = find_living(rn);
    if (found == owner)
        record_pass("EFUN-003", "find_living()");
    else
        record_fail("EFUN-003", "find_living()", "did not resolve active living");
}

test_clone_and_previous_object() {
    object d;
    string caller, tail;

    d = clone_object("obj/test_dummy");
    if (!d) {
        record_fail("OBJ-001", "clone_object()", "clone returned 0");
        return;
    }
    caller = call_other(d, "who_called_me");
    if (sscanf(caller, "obj/test_runner%s", tail) == 1)
        record_pass("OBJ-001", "clone_object() and previous_object()");
    else
        record_fail("OBJ-001", "clone_object() and previous_object()", "caller=" + caller);
    destruct(d);
}

test_transfer_and_inventory() {
    object env;
    object d1, d2;
    object it;
    int res, count;

    env = environment(owner);
    d1 = clone_object("obj/test_dummy");
    d2 = clone_object("obj/test_dummy");
    if (!d1 || !d2) {
        record_fail("OBJ-002", "clone for inventory tests", "failed to clone dummy objects");
        return;
    }
    call_other(d1, "set_name", "test_dummy");
    call_other(d2, "set_name", "test_dummy2");
    move_object(d1, env);

    if (present("test_dummy", env) == d1)
        record_pass("OBJ-002", "present() in room");
    else
        record_fail("OBJ-002", "present() in room", "test_dummy not found in environment");

    res = transfer(d1, owner);
    if (res == 0 && present("test_dummy", owner) == d1)
        record_pass("OBJ-003", "transfer() room -> player");
    else
        record_fail("OBJ-003", "transfer() room -> player", "transfer result=" + res);

    move_object(d2, owner);
    count = 0;
    it = first_inventory(owner);
    while (it) {
        if (it == d1 || it == d2)
            count += 1;
        it = next_inventory(it);
    }
    if (count == 2)
        record_pass("OBJ-004", "first_inventory()/next_inventory()");
    else
        record_fail("OBJ-004", "first_inventory()/next_inventory()", "expected 2 test objects, found " + count);

    res = transfer(d1, env);
    if (res == 0 && environment(d1) == env)
        record_pass("OBJ-005", "transfer() player -> room");
    else
        record_fail("OBJ-005", "transfer() player -> room", "transfer result=" + res);

    destruct(d1);
    destruct(d2);
}

test_command_dispatch() {
    string before, after, tail;
    int moved;

    before = file_name(environment(owner));
    if (sscanf(before, "room/church%s", tail) != 1) {
        if (sscanf(before, "room/lab%s", tail) == 1)
            command("west", owner);
        before = file_name(environment(owner));
        if (sscanf(before, "room/church%s", tail) != 1) {
            record_fail("CMD-001", "command() dispatch to room actions",
                        "precondition failed: expected room/church before east, got " + before);
            return;
        }
    }

    moved = command("east", owner);
    after = file_name(environment(owner));
    if (!moved)
        record_fail("CMD-001", "command() dispatch to room actions",
                    "command('east') returned 0 from " + before);
    else if (sscanf(after, "room/lab%s", tail) == 1)
        record_pass("CMD-001", "command() dispatch to room actions");
    else
        record_fail("CMD-001", "command() dispatch to room actions",
                    "expected room/lab, got " + after + " from " + before);
    if (sscanf(after, "room/lab%s", tail) == 1)
        command("west", owner);

    command("ping", owner);
    if (call_other(owner, "query_ping_hits") > 0)
        record_pass("CMD-002", "add_action() player command handlers");
    else
        record_fail("CMD-002", "add_action() player command handlers", "ping handler was not executed");
}

test_file_save_restore() {
    object blob;
    int ok, sz;
    int rn;
    string rt;

    blob = clone_object("obj/state_blob");
    if (!blob) {
        record_fail("FILE-001", "clone state_blob", "clone returned 0");
        return;
    }

    call_other(blob, "set_state", 4242, "engine-ok");
    ok = call_other(blob, "do_save", "data/state_blob_case");
    sz = file_size("data/state_blob_case.o");
    if (sz > 0)
        record_pass("FILE-001", "save_object()");
    else
        record_fail("FILE-001", "save_object()", "do_save=" + ok + ", size=" + sz);

    if (sz > 0)
        record_pass("FILE-002", "file_size() on save artifact");
    else
        record_fail("FILE-002", "file_size() on save artifact", "size=" + sz);

    destruct(blob);
    blob = clone_object("obj/state_blob");
    ok = call_other(blob, "do_restore", "data/state_blob_case");
    rn = call_other(blob, "query_number");
    rt = call_other(blob, "query_text");
    if (ok && rn == 4242 && rt == "engine-ok")
        record_pass("FILE-003", "restore_object()");
    else
        record_fail("FILE-003", "restore_object()",
                    "ok=" + ok + ", number=" + rn + ", text=" + rt);

    destruct(blob);
    rm("data/state_blob_case.o");
}

contains_fragment(hay, needle) {
    string a, b;

    if (!hay || !needle || needle == "")
        return 0;
    if (hay == needle)
        return 1;
    if (sscanf(hay, needle + "%s", a) == 1)
        return 1;
    if (sscanf(hay, "%s" + needle, a) == 1)
        return 1;
    if (sscanf(hay, "%s" + needle + "%s", a, b) == 2)
        return 1;
    return 0;
}

trigger_catch_error() {
    move_object("obj/definitely_missing_for_catch", owner);
    return 0;
}

test_error_control_flow() {
    string trap;

    trap = catch(throw("THROW_PROBE_TOKEN"));
    if (trap == "THROW_PROBE_TOKEN")
        record_pass("ERR-001", "throw() value propagation through catch()");
    else {
        if (!trap)
            trap = "0";
        record_fail("ERR-001", "throw() value propagation through catch()",
                    "expected THROW_PROBE_TOKEN, got " + trap);
    }

    trap = catch(trigger_catch_error());
    if (trap)
        record_pass("ERR-002", "catch() captures runtime error");
    else
        record_fail("ERR-002", "catch() captures runtime error",
                    "catch() returned 0 for forced runtime error");
}

test_find_object_semantics() {
    object bp, cl, found;
    string clone_name;

    bp = find_object("obj/test_dummy");
    if (bp && file_name(bp) == "obj/test_dummy")
        record_pass("FIND-001", "find_object() resolves loaded blueprint");
    else
        record_fail("FIND-001", "find_object() resolves loaded blueprint",
                    "find_object('obj/test_dummy') was 0 or wrong file_name");

    if (!find_object("obj/no_such_object_anywhere"))
        record_pass("FIND-002", "find_object() returns 0 for missing path");
    else
        record_fail("FIND-002", "find_object() returns 0 for missing path",
                    "find_object() unexpectedly resolved missing object");

    cl = clone_object("obj/test_dummy");
    if (!cl) {
        record_fail("FIND-003", "find_object(file_name(clone)) identity", "clone returned 0");
        record_fail("FIND-004", "find_object() clears clone after destruct", "clone returned 0");
        return;
    }
    clone_name = file_name(cl);
    found = find_object(clone_name);
    if (found == cl)
        record_pass("FIND-003", "find_object(file_name(clone)) identity");
    else
        record_fail("FIND-003", "find_object(file_name(clone)) identity",
                    "find_object(" + clone_name + ") did not return the same clone");

    destruct(cl);
    if (!find_object(clone_name))
        record_pass("FIND-004", "find_object() clears clone after destruct");
    else
        record_fail("FIND-004", "find_object() clears clone after destruct",
                    "clone still resolvable after destruct: " + clone_name);
}

test_move_object_error_paths() {
    string trap;
    object dst;

    trap = catch(move_object("obj/missing_move_source", owner));
    if (trap)
        record_pass("MOVE-001", "move_object() errors on missing source object");
    else
        record_fail("MOVE-001", "move_object() errors on missing source object",
                    "move_object missing source did not raise error");

    dst = clone_object("obj/test_dummy");
    if (!dst) {
        record_fail("MOVE-002", "move_object() errors on destructed destination", "clone returned 0");
        return;
    }
    destruct(dst);
    trap = catch(move_object(owner, dst));
    if (trap)
        record_pass("MOVE-002", "move_object() errors on destructed destination");
    else
        record_fail("MOVE-002", "move_object() errors on destructed destination",
                    "move_object to destructed destination did not raise error");
}

test_filesystem_efuns() {
    int sz;

    if (file_size("tmp_probe_file") >= 0)
        rm("tmp_probe_file");
    rmdir("tmp_probe_dir");

    write_file("tmp_probe_file", "line_one_for_cat\n");
    sz = file_size("tmp_probe_file");
    if (sz > 0)
        record_pass("FS-001", "write_file() + file_size()");
    else
        record_fail("FS-001", "write_file() + file_size()",
                    "file_size=" + sz + " after write_file");

    if (cat("tmp_probe_file", 1, 1))
        record_pass("FS-002", "cat() existing file");
    else
        record_fail("FS-002", "cat() existing file", "cat() returned 0 for tmp_probe_file");

    rm("tmp_probe_file");
    if (file_size("tmp_probe_file") < 0)
        record_pass("FS-003", "rm() removes file");
    else
        record_fail("FS-003", "rm() removes file", "file_size after rm was non-negative");

    if (!mkdir("tmp_probe_dir")) {
        record_fail("FS-004", "mkdir()/rmdir() round trip", "mkdir(tmp_probe_dir) returned 0");
        return;
    }
    if (rmdir("tmp_probe_dir"))
        record_pass("FS-004", "mkdir()/rmdir() round trip");
    else
        record_fail("FS-004", "mkdir()/rmdir() round trip",
                    "rmdir(tmp_probe_dir) returned 0 after mkdir");
}

test_messaging_efuns() {
    object env, sink;
    string seen;

    env = environment(owner);
    sink = clone_object("obj/msg_sink");
    if (!sink) {
        record_fail("MSG-001", "tell_object() to catch_tell target", "msg_sink clone returned 0");
        record_fail("MSG-002", "tell_room() room broadcast", "msg_sink clone returned 0");
        record_fail("MSG-003", "say() fan-out to room objects", "msg_sink clone returned 0");
        return;
    }

    move_object(sink, env);

    call_other(sink, "clear_seen");
    tell_object(sink, "TOBJ_PROBE\n");
    seen = call_other(sink, "query_seen");
    if (contains_fragment(seen, "TOBJ_PROBE"))
        record_pass("MSG-001", "tell_object() to catch_tell target");
    else
        record_fail("MSG-001", "tell_object() to catch_tell target",
                    "msg_sink did not receive tell_object payload");

    call_other(sink, "clear_seen");
    tell_room(env, "TROOM_PROBE\n");
    seen = call_other(sink, "query_seen");
    if (contains_fragment(seen, "TROOM_PROBE"))
        record_pass("MSG-002", "tell_room() room broadcast");
    else
        record_fail("MSG-002", "tell_room() room broadcast",
                    "msg_sink did not receive tell_room payload");

    call_other(sink, "clear_seen");
    command("say HARNESS_SAY_PROBE", owner);
    seen = call_other(sink, "query_seen");
    if (contains_fragment(seen, "HARNESS_SAY_PROBE"))
        record_pass("MSG-003", "say() fan-out to room objects");
    else
        record_fail("MSG-003", "say() fan-out to room objects",
                    "msg_sink did not receive say() payload");

    destruct(sink);
}

test_network_metadata() {
    if (query_ip_number(owner) && strlen(query_ip_number(owner)) > 0)
        record_pass("NET-001", "query_ip_number() on interactive user");
    else
        record_fail("NET-001", "query_ip_number() on interactive user",
                    "query_ip_number(owner) returned empty/0");
}

test_environment_probe_command() {
    string e1, e2, e3, tail;

    if (sscanf(file_name(environment(owner)), "room/church%s", tail) != 1)
        command("west", owner);

    command("probeenv", owner);
    e1 = call_other(owner, "query_last_probe_env");

    command("east", owner);
    command("probeenv", owner);
    e2 = call_other(owner, "query_last_probe_env");

    command("west", owner);
    command("probeenv", owner);
    e3 = call_other(owner, "query_last_probe_env");

    if (!e1) e1 = "0";
    if (!e2) e2 = "0";
    if (!e3) e3 = "0";

    if (sscanf(e1, "room/church%s", tail) == 1 &&
        sscanf(e2, "room/lab%s", tail) == 1 &&
        sscanf(e3, "room/church%s", tail) == 1)
        record_pass("ENV-001", "environment(this_player()) via command context");
    else
        record_fail("ENV-001", "environment(this_player()) via command context",
                    "probeenv paths: start=" + e1 + ", moved=" + e2 + ", return=" + e3);
}

test_light_accounting() {
    object probe;
    int before, during, after;

    probe = clone_object("obj/light_probe");
    if (!probe) {
        record_fail("LGT-001", "set_light() increments room total", "light_probe clone returned 0");
        record_fail("LGT-002", "set_light() decrement restores baseline", "light_probe clone returned 0");
        return;
    }

    before = call_other(probe, "query_total_light");
    call_other(probe, "enable_glow");
    during = call_other(probe, "query_total_light");
    call_other(probe, "disable_glow");
    after = call_other(probe, "query_total_light");
    destruct(probe);

    if (during > before)
        record_pass("LGT-001", "set_light() increments room total");
    else
        record_fail("LGT-001", "set_light() increments room total",
                    "expected during > before, got before=" + before + ", during=" + during);

    if (after == before)
        record_pass("LGT-002", "set_light() decrement restores baseline");
    else
        record_fail("LGT-002", "set_light() decrement restores baseline",
                    "expected after == before, got before=" + before + ", after=" + after);
}

test_bit_efuns() {
    string bits;

    bits = "";
    bits = set_bit(bits, 5);
    if (test_bit(bits, 5) && !test_bit(bits, 4))
        record_pass("BIT-001", "set_bit()/test_bit() semantics");
    else
        record_fail("BIT-001", "set_bit()/test_bit() semantics",
                    "bit 5 not set or adjacent bit unexpectedly set");

    bits = clear_bit(bits, 5);
    if (!test_bit(bits, 5))
        record_pass("BIT-002", "clear_bit() semantics");
    else
        record_fail("BIT-002", "clear_bit() semantics", "bit 5 remained set after clear_bit");
}

start_async_phase() {
    int removed;

    hb_probe = clone_object("obj/hb_probe");
    if (!hb_probe) {
        record_fail("ASYNC-001", "clone hb_probe", "clone returned 0");
        finalize_suite();
        return;
    }
    call_other(hb_probe, "start_probe");

    call_out("mark_order_a", 1);
    call_out("mark_callout_seen", 2);
    call_out("mark_order_b", 2);
    call_out("cancelled_callout", 5);
    removed = remove_call_out("cancelled_callout");
    if (removed >= 0)
        record_pass("ASYNC-001", "remove_call_out()");
    else
        record_fail("ASYNC-001", "remove_call_out()", "remove_call_out returned -1");

    call_out("finish_async_phase", 4);
}

mark_callout_seen() {
    async_callout_seen = 1;
}

mark_order_a() {
    async_order += "A";
}

mark_order_b() {
    async_order += "B";
}

cancelled_callout() {
    cancelled_callout_seen = 1;
}

finish_async_phase() {
    int ticks;

    if (async_callout_seen)
        record_pass("ASYNC-002", "call_out() scheduling");
    else
        record_fail("ASYNC-002", "call_out() scheduling", "scheduled callback was not observed");

    if (async_order == "AB")
        record_pass("ASYNC-006", "multiple call_out() ordering");
    else
        record_fail("ASYNC-006", "multiple call_out() ordering", "expected AB, got " + async_order);

    if (!cancelled_callout_seen)
        record_pass("ASYNC-003", "cancelled call_out() did not fire");
    else
        record_fail("ASYNC-003", "cancelled call_out() did not fire", "cancelled callback unexpectedly fired");

    ticks = call_other(hb_probe, "query_ticks");
    if (ticks > 0)
        record_pass("ASYNC-004", "heart_beat() execution");
    else
        record_fail("ASYNC-004", "heart_beat() execution", "ticks=" + ticks);

    call_other(hb_probe, "stop_probe");
    destruct(hb_probe);
    hb_probe = 0;

    expected_input = "ACKNOWLEDGE";
    call_other(owner, "arm_input_probe", this_object(), expected_input);
    if (!command("begin_input_probe", owner)) {
        record_fail("ASYNC-005", "command() + input_to() setup", "begin_input_probe command failed");
        finalize_suite();
        return;
    }
    call_out("input_probe_timeout", 20);
}

receive_input_probe(got, expected) {
    if (input_probe_done)
        return;
    input_probe_done = 1;
    remove_call_out("input_probe_timeout");

    if (got == expected)
        record_pass("ASYNC-005", "input_to() callback round-trip");
    else
        record_fail("ASYNC-005", "input_to() callback round-trip",
                    "expected '" + expected + "', got '" + got + "'");
    finalize_suite();
}

input_probe_timeout() {
    if (input_probe_done)
        return;
    input_probe_done = 1;
    record_fail("ASYNC-005", "input_to() callback round-trip", "timed out waiting for probe token");
    finalize_suite();
}

finalize_suite() {
    int elapsed;

    if (!suite_running)
        return;

    if (hb_probe) {
        call_other(hb_probe, "stop_probe");
        destruct(hb_probe);
        hb_probe = 0;
    }

    elapsed = time() - started_at;
    announce("TEST_SUMMARY total=" + total_tests +
             " pass=" + passed_tests +
             " fail=" + failed_tests +
             " duration=" + elapsed + "s\n");

    if (failed_tests == 0)
        announce("[RESULT] PASS all engine checks succeeded.\n");
    else
        announce("[RESULT] FAIL one or more engine checks failed.\n");

    suite_running = 0;
    if (owner)
        call_other(owner, "suite_finished", this_object());
    destruct(this_object());
}
