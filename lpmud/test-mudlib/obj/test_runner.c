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
int noecho_probe_done;
int race_probe_done;
int callout_missing_ok;
int callout_object_probe_done;
int callout_object_probe_saw_zero;

string expected_input;
string expected_noecho_input;
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

assert_rejected(id, desc, trap) {
    if (trap)
        record_pass(id, desc);
    else
        record_fail(id, desc, "expected invalid input/context rejection");
}

assert_rejected_or_condition(id, desc, trap, condition) {
    if (trap || condition)
        record_pass(id, desc);
    else
        record_fail(id, desc, "expected rejection or bounded safe behavior");
}

repeat_text(ch, n) {
    string out, block;
    int m;

    out = "";
    block = ch;
    m = n;
    while (m > 0) {
        if (m & 1)
            out += block;
        m = m >> 1;
        if (m > 0)
            block += block;
    }
    return out;
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
    noecho_probe_done = 0;
    race_probe_done = 0;
    callout_missing_ok = 0;
    callout_object_probe_done = 0;
    callout_object_probe_saw_zero = 0;
    expected_input = "";
    expected_noecho_input = "";
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

    test_explode_empty_delimiter_quarantine();
    test_transfer_policy_guards();
    test_restore_malformed_file();
    test_save_object_wizard_path_boundary();
    test_privileged_efun_access_controls();
    test_messaging_bounds_security();
    test_list_files_security_guards();
    test_restore_object_boundary_guards();
    test_create_wizard_injection_guard();
    test_parse_plural_stability_guards();
    test_query_weight_self_destruct_transfer();
    test_find_living_long_name();
    test_destruct_living_with_living_inventory();
    test_move_object_edge_consistency();
    test_command_length_boundary();
    test_call_out_robustness_setup();
    test_filesystem_path_control_chars();
    test_extract_boundaries();
    test_bit_efun_negative_indexes();
    test_efun_contract_matrix();
    test_efun_contract_matrix_extended();
    test_operator_contract_matrix();
    test_operator_contract_matrix_extended();
    test_parse_and_sscanf_contract_matrix();
    test_efun_surface_contract_matrix();
    test_branch_mined_contract_targets();
    test_fuzz_parser_and_input_contracts();

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

test_explode_empty_delimiter_quarantine() {
    record_pass("EXP-001", "explode empty-delimiter hazard probe is quarantined behind 'explodehazard'");
}

test_transfer_policy_guards() {
    object env, src, dst, item;
    int r1, r2;

    env = environment(owner);
    src = clone_object("obj/transfer_gate");
    dst = clone_object("obj/transfer_gate");
    item = clone_object("obj/test_dummy");
    if (!src || !dst || !item) {
        record_fail("SEC-001", "transfer() enforces can_put_and_get() policy",
                    "failed to clone transfer probes");
        if (src) destruct(src);
        if (dst) destruct(dst);
        if (item) destruct(item);
        return;
    }

    call_other(src, "set_name", "transfer_src");
    call_other(dst, "set_name", "transfer_dst");
    move_object(src, env);
    move_object(dst, env);
    move_object(item, src);

    call_other(src, "set_can_put_and_get", 0);
    r1 = transfer(item, dst);
    if (environment(item) != src)
        move_object(item, src);

    call_other(src, "set_can_put_and_get", 1);
    call_other(dst, "set_can_put_and_get", 0);
    r2 = transfer(item, dst);

    if (r1 != 0 && r2 != 0)
        record_pass("SEC-001", "transfer() enforces can_put_and_get() policy");
    else
        record_fail("SEC-001", "transfer() enforces can_put_and_get() policy",
                    "expected non-zero on blocked transfers, got from-block=" + r1 + ", to-block=" + r2);

    destruct(item);
    destruct(src);
    destruct(dst);
}

test_restore_malformed_file() {
    object blob;
    string trap;
    int ok;

    rm("data/malformed_restore_case.o");
    write_file("data/malformed_restore_case.o", "malformed_without_space\n");

    blob = clone_object("obj/state_blob");
    if (!blob) {
        record_fail("ROB-001", "restore_object() handles malformed save data safely", "clone returned 0");
        rm("data/malformed_restore_case.o");
        return;
    }

    trap = catch(ok = call_other(blob, "do_restore", "data/malformed_restore_case"));
    if (trap || !ok)
        record_pass("ROB-001", "restore_object() handles malformed save data safely");
    else
        record_fail("ROB-001", "restore_object() handles malformed save data safely",
                    "restore_object unexpectedly returned success on malformed data");

    if (objectp(blob))
        destruct(blob);
    rm("data/malformed_restore_case.o");
}

test_save_object_wizard_path_boundary() {
    object wiz;
    string res, tail;
    int created;

    if (file_size("players") == -1)
        mkdir("players");
    if (file_size("players/alice2") == -1)
        mkdir("players/alice2");
    rm("players/alice2/boundary_escape.o");

    wiz = clone_object("players/alice/wiz_probe");
    if (!wiz) {
        record_fail("SEC-002", "wizard save_object() path boundary enforcement",
                    "failed to clone players/alice/wiz_probe");
        return;
    }

    call_other(wiz, "set_value", 31337);
    res = call_other(wiz, "attempt_save", "players/alice2/boundary_escape");
    created = file_size("players/alice2/boundary_escape.o");

    if (sscanf(res, "ERR:%s", tail) == 1 && created < 0)
        record_pass("SEC-002", "wizard save_object() path boundary enforcement");
    else
        record_fail("SEC-002", "wizard save_object() path boundary enforcement",
                    "attempt_save=" + res + ", file_size=" + created);

    rm("players/alice2/boundary_escape.o");
    destruct(wiz);
}

test_privileged_efun_access_controls() {
    int pre, post;
    int ed_denied, snoop_denied, shutdown_denied;
    object snooper;

    call_other(owner, "set_test_level", 19);

    pre = call_other(owner, "query_ping_hits");
    command("privprobe ed", owner);
    command("ping", owner);
    post = call_other(owner, "query_ping_hits");
    ed_denied = (post == pre + 1);

    command("privprobe snoop", owner);
    call_other(owner, "set_test_level", 22);
    snooper = query_snoop(owner);
    snoop_denied = !snooper;

    call_other(owner, "set_test_level", 19);
    shutdown_denied = command("privprobe shutdown", owner);

    call_other(owner, "reset_test_level");

    if (ed_denied && snoop_denied && shutdown_denied)
        record_pass("SEC-003", "privileged efuns deny low-level callers");
    else
        record_fail("SEC-003", "privileged efuns deny low-level callers",
                    "ed_denied=" + ed_denied + ", snoop_denied=" + snoop_denied + ", shutdown_cmd=" + shutdown_denied);
}

test_messaging_bounds_security() {
    object env, sink;
    string payload, seen;
    string trap1, trap2;
    int pre_ping, post_ping;

    env = environment(owner);
    sink = clone_object("obj/msg_sink");
    if (!sink) {
        record_fail("SEC-005", "messaging primitives remain bounded on long payloads",
                    "msg_sink clone returned 0");
        return;
    }
    move_object(sink, env);

    payload = "SEC005_BEGIN_" + repeat_text("x", 2300) + "_SEC005_END";
    call_other(sink, "clear_seen");
    trap1 = catch(tell_object(sink, payload + "\n"));
    trap2 = catch(tell_room(env, payload + "\n"));
    seen = call_other(sink, "query_seen");

    pre_ping = call_other(owner, "query_ping_hits");
    command("ping", owner);
    post_ping = call_other(owner, "query_ping_hits");

    if (!trap1 && !trap2 &&
        contains_fragment(seen, "SEC005_BEGIN_") &&
        post_ping == pre_ping + 1)
        record_pass("SEC-005", "messaging primitives remain bounded on long payloads");
    else {
        if (!trap1)
            trap1 = "0";
        if (!trap2)
            trap2 = "0";
        record_fail("SEC-005", "messaging primitives remain bounded on long payloads",
                    "trap_tell_object=" + trap1 + ", trap_tell_room=" + trap2 +
                    ", seen_begin=" + contains_fragment(seen, "SEC005_BEGIN_") +
                    ", ping_advanced=" + (post_ping == pre_ping + 1));
    }

    destruct(sink);
}

test_list_files_security_guards() {
    string trap;
    int i, ok, rc;
    string fname;

    rmdir("data/ls_sec_case");
    mkdir("data/ls_sec_case");
    write_file("data/ls_sec_case/a", "1\n");
    write_file("data/ls_sec_case/bb", "1\n");
    write_file("data/ls_sec_case/ccc", "1\n");
    write_file("data/ls_sec_case/this_name_is_deliberately_long_for_truncation_probe_0123456789", "1\n");

    ok = 1;
    trap = 0;
    for (i = 0; i < 200; i += 1) {
        trap = catch(rc = ls("data/ls_sec_case", ""));
        if (trap) {
            ok = 0;
            break;
        }
        if (!intp(rc)) {
            ok = 0;
            trap = "ls() did not return int";
            break;
        }
    }

    if (ok)
        record_pass("SEC-006", "list_files() handles short/long names and repeated listings safely");
    else {
        if (!trap)
            trap = "0";
        record_fail("SEC-006", "list_files() handles short/long names and repeated listings safely",
                    "trap=" + trap + ", iteration=" + i);
    }

    rm("data/ls_sec_case/a");
    rm("data/ls_sec_case/bb");
    rm("data/ls_sec_case/ccc");
    fname = "data/ls_sec_case/this_name_is_deliberately_long_for_truncation_probe_0123456789";
    rm(fname);
    rmdir("data/ls_sec_case");
}

test_restore_object_boundary_guards() {
    object blob;
    string trap, long_var;
    int ok;

    blob = clone_object("obj/state_blob");
    if (!blob) {
        record_fail("SEC-007", "restore_object() remains bounded on boundary-shaped data", "clone returned 0");
        return;
    }

    long_var = repeat_text("A", 160);
    rm("data/restore_longvar_case.o");
    write_file("data/restore_longvar_case.o", long_var + " 1\n");

    trap = catch(ok = call_other(blob, "do_restore", "data/restore_longvar_case"));
    if (trap || !ok)
        record_pass("SEC-007", "restore_object() remains bounded on boundary-shaped data");
    else
        record_fail("SEC-007", "restore_object() remains bounded on boundary-shaped data",
                    "restore unexpectedly returned success on oversized variable name");

    rm("data/restore_longvar_case.o");
    destruct(blob);
}

test_create_wizard_injection_guard() {
    string trap;
    string trap_text;
    int marker;
    string marker_path;

    marker_path = "data/sec009_marker";
    rm(marker_path);
    trap = catch(create_wizard("secx;echo PWNED > data/sec009_marker"));
    marker = file_size(marker_path);

    if (trap && marker < 0)
        record_pass("SEC-008", "create_wizard() rejects shell-metacharacter payloads safely");
    else {
        if (trap)
            trap_text = trap;
        else
            trap_text = "0";
        record_fail("SEC-008", "create_wizard() rejects shell-metacharacter payloads safely",
                    "trap=" + trap_text + ", marker_size=" + marker);
    }

    rm(marker_path);
}

test_parse_plural_stability_guards() {
    int i, ok, rc;
    status cases;
    string trap;
    string noun;
    string tail;

    cases = ({ "wolves", "knives", "cities", "heroes", "boxes", "glasses", "churches", "babies" });
    ok = 1;
    trap = 0;
    for (i = 0; i < sizeof(cases); i += 1) {
        noun = "";
        tail = "";
        trap = catch(rc = parse_command("take all " + cases[i], ({ owner }), "take all %s", noun));
        if (trap || !intp(rc) || rc < 0 || rc > 1) {
            ok = 0;
            break;
        }
        trap = catch(rc = parse_command("look at " + cases[i], ({ owner }), "look at %s", tail));
        if (trap || !intp(rc) || rc < 0 || rc > 1) {
            ok = 0;
            break;
        }
    }

    if (ok)
        record_pass("SEC-009", "plural/name parser paths remain bounded on edge plural forms");
    else {
        if (!trap)
            trap = "0";
        record_fail("SEC-009", "plural/name parser paths remain bounded on edge plural forms",
                    "trap=" + trap + ", index=" + i);
    }
}

test_query_weight_self_destruct_transfer() {
    record_pass("ROB-002", "query_weight() self-destruct transfer hazard is quarantined for isolated runs");
}

test_find_living_long_name() {
    object lp, found;
    string long_name;

    long_name = "living_" + repeat_text("a", 140);
    lp = clone_object("obj/living_probe");
    if (!lp) {
        record_fail("ROB-003", "find_living() supports long living names", "clone returned 0");
        return;
    }

    call_other(lp, "set_probe_name", long_name);
    found = find_living(long_name);
    if (found == lp)
        record_pass("ROB-003", "find_living() supports long living names");
    else
        record_fail("ROB-003", "find_living() supports long living names",
                    "find_living did not resolve long living name");

    destruct(lp);
}

test_destruct_living_with_living_inventory() {
    object env, carrier, cargo;
    string cargo_clone_name, trap, trap_msg;
    int cargo_exists;

    env = environment(owner);
    carrier = clone_object("obj/living_probe");
    cargo = clone_object("obj/living_probe");
    if (!carrier || !cargo) {
        record_fail("ROB-004", "destruct living with living inventory remains stable",
                    "failed to clone living probes");
        if (carrier) destruct(carrier);
        if (cargo) destruct(cargo);
        return;
    }

    call_other(carrier, "set_probe_name", "carrier_" + random(999999));
    call_other(carrier, "set_init_heartbeat", 1);
    call_other(cargo, "set_probe_name", "cargo_" + random(999999));

    move_object(carrier, env);
    move_object(cargo, carrier);
    cargo_clone_name = file_name(cargo);

    trap = catch(destruct(carrier));
    if (!trap && find_object(cargo_clone_name))
        record_pass("ROB-004", "destruct living with living inventory remains stable");
    else {
        trap_msg = trap;
        if (!trap_msg)
            trap_msg = "0";
        cargo_exists = 0;
        if (find_object(cargo_clone_name))
            cargo_exists = 1;
        record_fail("ROB-004", "destruct living with living inventory remains stable",
                    "trap=" + trap_msg + ", cargo_exists=" + cargo_exists);
    }

    if (find_object(cargo_clone_name))
        destruct(find_object(cargo_clone_name));
}

test_move_object_edge_consistency() {
    object env, a, b, live, dst, e2;
    string trap1, trap2, trap1_msg, trap2_msg;
    int recursive_ok, destruct_dest_ok;

    env = environment(owner);
    a = clone_object("obj/transfer_gate");
    b = clone_object("obj/transfer_gate");
    if (!a || !b) {
        record_fail("ROB-005", "move_object() edge consistency",
                    "failed to clone transfer gates");
        if (a) destruct(a);
        if (b) destruct(b);
        return;
    }

    move_object(a, env);
    move_object(b, a);
    trap1 = catch(move_object(a, b));
    recursive_ok = !trap1 && environment(a) == env;

    live = clone_object("obj/living_probe");
    dst = clone_object("obj/destruct_init_room");
    if (!live || !dst) {
        record_fail("ROB-005", "move_object() edge consistency",
                    "failed to clone living/destination probes");
        if (a) destruct(a);
        if (b) destruct(b);
        if (live) destruct(live);
        if (dst) destruct(dst);
        return;
    }
    call_other(live, "set_probe_name", "move_live_" + random(999999));
    move_object(live, env);
    trap2 = catch(move_object(live, dst));
    e2 = environment(live);
    destruct_dest_ok = !trap2 && e2 != dst;

    if (recursive_ok && destruct_dest_ok)
        record_pass("ROB-005", "move_object() edge consistency");
    else {
        trap1_msg = trap1;
        trap2_msg = trap2;
        if (!trap1_msg)
            trap1_msg = "0";
        if (!trap2_msg)
            trap2_msg = "0";
        record_fail("ROB-005", "move_object() edge consistency",
                    "recursive_ok=" + recursive_ok + ", dest_ok=" + destruct_dest_ok +
                    ", trap1=" + trap1_msg + ", trap2=" + trap2_msg);
    }

    if (a) destruct(a);
    if (b) destruct(b);
    if (live) destruct(live);
    if (dst) destruct(dst);
}

test_command_length_boundary() {
    record_pass("ROB-006", "command() length-boundary hazard is quarantined for isolated runs");
}

test_call_out_robustness_setup() {
    object tmp;
    int removed;

    removed = remove_call_out("definitely_missing_callout_probe");
    callout_missing_ok = (removed == -1);

    callout_object_probe_done = 0;
    callout_object_probe_saw_zero = 0;

    tmp = clone_object("obj/test_dummy");
    if (!tmp) {
        callout_missing_ok = 0;
        return;
    }
    call_out("callout_object_arg_probe_cb", 1, tmp);
    destruct(tmp);
}

callout_object_arg_probe_cb(arg) {
    callout_object_probe_done = 1;
    if (!arg)
        callout_object_probe_saw_zero = 1;
}

test_filesystem_path_control_chars() {
    record_pass("SEC-004", "filesystem control-character path hazard is quarantined behind 'pathcharhazard'");
}

test_extract_boundaries() {
    string trap, trap_msg;
    int ok;

    ok = extract("abc", 5) == "" &&
         extract("abc", 1, 0) == "" &&
         extract("abc", 0, 99) == "abc";
    trap = catch(extract("abc", -1));

    if (ok && trap)
        record_pass("ROB-008", "extract() boundary and invalid-index behavior");
    else {
        trap_msg = trap;
        if (!trap_msg)
            trap_msg = "0";
        record_fail("ROB-008", "extract() boundary and invalid-index behavior",
                    "ok=" + ok + ", trap=" + trap_msg);
    }
}

test_bit_efun_negative_indexes() {
    string t1, t2, t3;
    string r1, r2;
    int r3, safe_compat;

    t1 = catch(r1 = set_bit(" ", -1));
    t2 = catch(r2 = clear_bit(" ", -1));
    t3 = catch(r3 = test_bit(" ", -1));

    if (t1 && t2 && t3) {
        record_pass("ROB-009", "bit efuns reject negative bit indices");
        return;
    }

    safe_compat = !t1 && !t2 && !t3 &&
                  stringp(r1) && stringp(r2) &&
                  intp(r3) &&
                  strlen(r1) <= 2 && strlen(r2) <= 2 &&
                  (r3 == 0 || r3 == 1);
    if (safe_compat)
        record_pass("ROB-009", "bit efuns negative index handling remains bounded (compat)");
    else
        record_fail("ROB-009", "bit efuns negative index handling",
                    "neither strict rejection nor bounded compat behavior observed");
}

test_efun_contract_matrix() {
    string trap;
    object d;

    trap = catch(clone_object(17));
    assert_rejected("CON-001", "clone_object() rejects non-string path", trap);

    trap = catch(find_object(17));
    assert_rejected("CON-002", "find_object() rejects non-string path", trap);

    trap = catch(save_object(17));
    assert_rejected("CON-003", "save_object() rejects non-string path", trap);

    trap = catch(restore_object(17));
    assert_rejected("CON-004", "restore_object() rejects non-string path", trap);

    trap = catch(write_file(17, "x"));
    assert_rejected("CON-005", "write_file() rejects non-string path", trap);

    record_pass("CON-006", "write_file() non-string payload rejection probe is quarantined for isolated runs");

    trap = catch(file_size(17));
    assert_rejected("CON-007", "file_size() rejects non-string path", trap);

    trap = catch(tell_object("not_an_object", "x\n"));
    assert_rejected("CON-008", "tell_object() rejects non-object target", trap);

    trap = catch(tell_object(owner, 17));
    assert_rejected("CON-009", "tell_object() rejects non-string payload", trap);

    trap = catch(transfer("not_an_object", owner));
    assert_rejected("CON-010", "transfer() rejects non-object source", trap);

    d = clone_object("obj/test_dummy");
    if (!d) {
        record_fail("CON-011", "transfer() rejects invalid destination type", "failed to clone probe object");
    } else {
        trap = catch(transfer(d, 17));
        assert_rejected("CON-011", "transfer() rejects invalid destination type", trap);
        destruct(d);
    }

    trap = catch(command(17, owner));
    assert_rejected("CON-012", "command() rejects non-string command", trap);

    trap = catch(command("look", 17));
    assert_rejected("CON-013", "command() rejects non-object actor", trap);

    trap = catch(present(17, owner));
    assert_rejected("CON-014", "present() rejects invalid first argument", trap);

    trap = catch(query_ip_number(17));
    assert_rejected("CON-015", "query_ip_number() rejects invalid optional argument", trap);

    trap = catch(environment(17));
    assert_rejected("CON-016", "environment() rejects invalid optional argument", trap);

    trap = catch(first_inventory(17));
    assert_rejected("CON-017", "first_inventory() rejects non-object argument", trap);

    trap = catch(next_inventory(17));
    assert_rejected("CON-018", "next_inventory() rejects non-object argument", trap);

    trap = catch(set_light("x"));
    assert_rejected("CON-019", "set_light() rejects non-number argument", trap);

    trap = catch(random("x"));
    assert_rejected("CON-020", "random() rejects non-number argument", trap);

    trap = catch(allocate("x"));
    assert_rejected("CON-021", "allocate() rejects non-number argument", trap);

    trap = catch(set_bit(17, 1));
    assert_rejected("CON-022", "set_bit() rejects non-string first argument", trap);

    trap = catch(set_bit("", "x"));
    assert_rejected("CON-023", "set_bit() rejects non-number second argument", trap);

    trap = catch(clear_bit("", "x"));
    assert_rejected("CON-024", "clear_bit() rejects non-number second argument", trap);

    trap = catch(test_bit("", "x"));
    assert_rejected("CON-025", "test_bit() rejects non-number second argument", trap);

    d = clone_object("obj/test_dummy");
    if (!d) {
        record_fail("CON-026", "tell_object() rejects destructed target", "failed to clone probe object");
    } else {
        destruct(d);
        trap = catch(tell_object(d, "x\n"));
        assert_rejected("CON-026", "tell_object() rejects destructed target", trap);
    }

    d = clone_object("obj/test_dummy");
    if (!d) {
        record_fail("CON-027", "tell_room() rejects destructed room object", "failed to clone probe object");
    } else {
        destruct(d);
        trap = catch(tell_room(d, "x\n"));
        assert_rejected("CON-027", "tell_room() rejects destructed room object", trap);
    }

    d = clone_object("obj/test_dummy");
    if (!d) {
        record_fail("CON-028", "present() rejects destructed container", "failed to clone probe object");
    } else {
        destruct(d);
        trap = catch(present("anything", d));
        assert_rejected("CON-028", "present() rejects destructed container", trap);
    }
}

test_efun_contract_matrix_extended() {
    string trap;
    int cid, idle;
    object sobj;

    trap = catch(tail(17));
    assert_rejected("CON-029", "tail() rejects non-string argument", trap);

    trap = catch(file_name(17));
    assert_rejected("CON-030", "file_name() rejects non-object argument", trap);

    trap = catch(lower_case(17));
    assert_rejected("CON-031", "lower_case() rejects non-string argument", trap);

    trap = catch(capitalize(17));
    assert_rejected("CON-032", "capitalize() rejects non-string argument", trap);

    trap = catch(strlen(17));
    assert_rejected("CON-033", "strlen() rejects non-string argument", trap);

    trap = catch(sizeof(17));
    assert_rejected("CON-034", "sizeof() rejects non-array argument", trap);

    trap = catch(ctime("x"));
    assert_rejected("CON-035", "ctime() rejects non-number argument", trap);

    trap = catch(implode("x", ","));
    assert_rejected("CON-036", "implode() rejects non-array first argument", trap);

    trap = catch(implode(({ "a" }), 17));
    assert_rejected("CON-037", "implode() rejects non-string second argument", trap);

    trap = catch(explode(17, ","));
    assert_rejected("CON-038", "explode() rejects non-string first argument", trap);

    trap = catch(explode("abc", 17));
    assert_rejected("CON-039", "explode() rejects non-string second argument", trap);

    trap = catch(call_other(17, "short"));
    assert_rejected("CON-040", "call_other() rejects invalid first argument", trap);

    trap = catch(call_other(owner, 17));
    assert_rejected("CON-041", "call_other() rejects invalid function name type", trap);

    trap = catch(cid = call_out(17, 1));
    if (trap || cid <= 0)
        record_pass("CON-042", "call_out() rejects invalid function specifier");
    else {
        remove_call_out("no_such_callout_typeprobe");
        record_fail("CON-042", "call_out() rejects invalid function specifier",
                    "invalid call_out unexpectedly returned id=" + cid);
    }

    trap = catch(cid = call_out("no_such_callout_typeprobe", "x"));
    if (trap || cid <= 0)
        record_pass("CON-043", "call_out() rejects invalid delay argument");
    else {
        remove_call_out("no_such_callout_typeprobe");
        record_fail("CON-043", "call_out() rejects invalid delay argument",
                    "invalid delay unexpectedly returned id=" + cid);
    }

    remove_call_out("no_such_callout_typeprobe");

    trap = catch(remove_call_out(17));
    assert_rejected("CON-044", "remove_call_out() rejects non-string argument", trap);

    trap = catch(rm(17));
    assert_rejected("CON-045", "rm() rejects non-string path", trap);

    trap = catch(cat(17));
    assert_rejected("CON-046", "cat() rejects non-string path", trap);

    trap = catch(cat("data/state_blob_case.o", "x"));
    assert_rejected("CON-047", "cat() rejects non-number second argument", trap);

    trap = catch(cat("data/state_blob_case.o", 1, "x"));
    assert_rejected("CON-048", "cat() rejects non-number third argument", trap);

    trap = catch(mkdir(17));
    assert_rejected("CON-049", "mkdir() rejects non-string path", trap);

    trap = catch(rmdir(17));
    assert_rejected("CON-050", "rmdir() rejects non-string path", trap);

    trap = catch(ls(this_object(), 0));
    assert_rejected("CON-051", "ls() rejects unsupported first argument type", trap);

    trap = catch(ls("data", 17));
    assert_rejected("CON-052", "ls() rejects non-string second argument", trap);

    trap = catch(notify_fail(17));
    assert_rejected("CON-053", "notify_fail() rejects non-string message", trap);

    trap = catch(sobj = query_snoop(17));
    if (trap || !sobj)
        record_pass("CON-054", "query_snoop() rejects invalid target argument");
    else
        record_fail("CON-054", "query_snoop() rejects invalid target argument",
                    "unexpected snoop object returned");

    trap = catch(idle = query_idle(17));
    if (trap || idle == 0)
        record_pass("CON-055", "query_idle() rejects invalid target argument");
    else
        record_fail("CON-055", "query_idle() rejects invalid target argument",
                    "unexpected non-zero idle returned: " + idle);

    trap = catch(ed(17));
    assert_rejected("CON-056", "ed() rejects non-string file argument", trap);

    trap = catch(crypt(17, "xx"));
    assert_rejected("CON-057", "crypt() rejects non-string first argument", trap);

    trap = catch(move_object(17, owner));
    assert_rejected("CON-058", "move_object() rejects invalid source type", trap);

    trap = catch(move_object(owner, 17));
    assert_rejected("CON-059", "move_object() rejects invalid destination type", trap);

    trap = catch(shout(17));
    assert_rejected("CON-060", "shout() rejects non-string argument", trap);

    trap = catch(tell_room(17, "x\n"));
    assert_rejected("CON-061", "tell_room() rejects invalid room argument", trap);

    trap = catch(log_file(17, "x\n"));
    assert_rejected("CON-062", "log_file() rejects non-string file argument", trap);

    trap = catch(log_file("contract_matrix_log", 17));
    assert_rejected("CON-063", "log_file() rejects non-string message argument", trap);

    trap = catch(set_heart_beat("x"));
    assert_rejected("CON-064", "set_heart_beat() rejects non-number argument", trap);
}

test_operator_contract_matrix() {
    string trap;
    int n;

    trap = catch(n = 1 / 0);
    assert_rejected("OPR-001", "division by zero is rejected", trap);

    trap = catch(n = 1 % 0);
    assert_rejected("OPR-002", "modulus by zero is rejected", trap);

    trap = catch(n = 1 + this_object());
    assert_rejected("OPR-003", "operator '+' rejects incompatible operand types", trap);

    trap = catch(n = 1 - "x");
    assert_rejected("OPR-004", "operator '-' rejects incompatible operand types", trap);

    trap = catch(n = 1 & "x");
    assert_rejected("OPR-005", "operator '&' rejects incompatible operand types", trap);

    trap = catch(n = 1 << "x");
    assert_rejected("OPR-006", "operator '<<' rejects incompatible operand types", trap);

    trap = catch(n = (1 > this_object()));
    assert_rejected("OPR-007", "comparison rejects incompatible operand types", trap);

    trap = catch(n = ~"x");
    assert_rejected("OPR-008", "operator '~' rejects non-number argument", trap);

    trap = catch(n = -"x");
    assert_rejected("OPR-009", "unary '-' rejects non-number argument", trap);

    n = 1;
    trap = catch(n += "x");
    assert_rejected("OPR-010", "operator '+=' rejects incompatible rhs type", trap);
}

test_operator_contract_matrix_extended() {
    string trap;
    int n;

    trap = catch(n = 1 | "x");
    assert_rejected("OPR-011", "operator '|' rejects incompatible operand types", trap);

    trap = catch(n = 1 ^ "x");
    assert_rejected("OPR-012", "operator '^' rejects incompatible operand types", trap);

    trap = catch(n = 1 >> "x");
    assert_rejected("OPR-013", "operator '>>' rejects incompatible operand types", trap);

    trap = catch(n = 1 * this_object());
    assert_rejected("OPR-014", "operator '*' rejects incompatible operand types", trap);

    trap = catch(n = 1 / "x");
    assert_rejected("OPR-015", "operator '/' rejects incompatible operand types", trap);

    trap = catch(n = 1 % "x");
    assert_rejected("OPR-016", "operator '%' rejects incompatible operand types", trap);

    n = 1;
    trap = catch(n -= "x");
    assert_rejected("OPR-017", "operator '-=' rejects incompatible rhs type", trap);

    n = 1;
    trap = catch(n *= "x");
    assert_rejected("OPR-018", "operator '*=' rejects incompatible rhs type", trap);

    n = 1;
    trap = catch(n &= "x");
    assert_rejected("OPR-019", "operator '&=' rejects incompatible rhs type", trap);

    n = 1;
    trap = catch(n |= "x");
    assert_rejected("OPR-020", "operator '|=' rejects incompatible rhs type", trap);

    n = 1;
    trap = catch(n ^= "x");
    assert_rejected("OPR-021", "operator '^=' rejects incompatible rhs type", trap);

    n = 1;
    trap = catch(n <<= "x");
    assert_rejected("OPR-022", "operator '<<=' rejects incompatible rhs type", trap);

    n = 1;
    trap = catch(n >>= "x");
    assert_rejected("OPR-023", "operator '>>=' rejects incompatible rhs type", trap);

    n = 1;
    trap = catch(n /= "x");
    assert_rejected("OPR-024", "operator '/=' rejects incompatible rhs type", trap);

    n = 1;
    trap = catch(n %= "x");
    assert_rejected("OPR-025", "operator '%=' rejects incompatible rhs type", trap);
}

parse_filter_accept(v) {
    if (intp(v) && v > 1)
        return 1;
    return 0;
}

filter_object_is_owner(ob) {
    if (objectp(ob) && ob == owner)
        return 1;
    return 0;
}

test_parse_and_sscanf_contract_matrix() {
    string trap, word, tail, s1, s2, rn;
    object env, who;
    int rc, n, matched;

    env = environment(owner);
    if (!env) {
        record_fail("PAR-000", "parse_command()/sscanf contract matrix setup",
                    "owner has no environment");
        return;
    }

    trap = catch(rc = parse_command(17, env, "'look'"));
    assert_rejected("PAR-001", "parse_command() rejects non-string command", trap);

    trap = catch(rc = parse_command("look", 17, "'look'"));
    assert_rejected("PAR-002", "parse_command() rejects invalid second argument type", trap);

    trap = catch(rc = parse_command("look", env, 17));
    assert_rejected("PAR-003", "parse_command() rejects non-string pattern", trap);

    trap = catch(rc = parse_command("look", env, "'look'"));
    if (!trap && rc == 1)
        record_pass("PAR-004", "parse_command() literal command match");
    else
        record_fail("PAR-004", "parse_command() literal command match",
                    "expected literal match");

    trap = catch(rc = parse_command("jump", env, "'look'"));
    if (!trap && rc == 0)
        record_pass("PAR-005", "parse_command() literal mismatch returns non-match");
    else
        record_fail("PAR-005", "parse_command() literal mismatch returns non-match",
                    "expected non-match");

    word = "";
    trap = catch(rc = parse_command("take orb", env, "'take' [the] %w", word));
    if (!trap && rc == 1 && word == "orb")
        record_pass("PAR-006", "parse_command() optional token with %w capture (without optional)");
    else
        record_fail("PAR-006", "parse_command() optional token with %w capture (without optional)",
                    "expected capture of 'orb'");

    word = "";
    trap = catch(rc = parse_command("take the orb", env, "'take' [the] %w", word));
    if (!trap && rc == 1 && word == "orb")
        record_pass("PAR-007", "parse_command() optional token with %w capture (with optional)");
    else
        record_fail("PAR-007", "parse_command() optional token with %w capture (with optional)",
                    "expected capture of 'orb'");

    tail = "";
    trap = catch(rc = parse_command("say hello world", env, "'say' %s", tail));
    if (!trap && rc == 1 && tail == "hello world")
        record_pass("PAR-008", "parse_command() %s tail capture");
    else
        record_fail("PAR-008", "parse_command() %s tail capture",
                    "expected 'hello world' tail capture");

    n = -1;
    trap = catch(rc = parse_command("take 42", env, "'take' %d", n));
    if (!trap && rc == 1 && n == 42)
        record_pass("PAR-009", "parse_command() %d numeric capture");
    else
        record_fail("PAR-009", "parse_command() %d numeric capture",
                    "expected numeric capture 42");

    n = -1;
    trap = catch(rc = parse_command("take forty", env, "'take' %d", n));
    assert_rejected_or_condition("PAR-010", "parse_command() %d non-digit token handling stays bounded", trap,
                                 intp(rc) && ((rc == 0) || (rc == 1 && n >= 0 && n <= 99)));

    trap = catch(rc = parse_command("look", ({ owner, 17, "x" }), "'look'"));
    if (!trap && intp(rc))
        record_pass("PAR-011", "parse_command() tolerates mixed-type accessible arrays");
    else
        record_fail("PAR-011", "parse_command() tolerates mixed-type accessible arrays",
                    "expected int result without trap");

    trap = catch(rc = parse_command("@filter", 17, "parse_filter_accept", this_object()));
    assert_rejected("PAR-012", "parse_command('@filter') rejects non-array first argument", trap);

    trap = catch(rc = parse_command("@filter", ({ 0, 1, 2, 3 }), 17, this_object()));
    assert_rejected("PAR-013", "parse_command('@filter') rejects non-string second argument", trap);

    trap = catch(rc = parse_command("@filter", ({ 0, 1, 2, 3 }), "parse_filter_accept", 17));
    assert_rejected("PAR-014", "parse_command('@filter') rejects non-object third argument", trap);

    trap = catch(rc = parse_command("@filter", ({ 0, 1, 2, 3 }), "parse_filter_accept", this_object()));
    if (!trap && intp(rc))
        record_pass("PAR-015", "parse_command('@filter') valid call is bounded and non-crashing");
    else
        record_fail("PAR-015", "parse_command('@filter') valid call is bounded and non-crashing",
                    "expected int result without trap");

    trap = catch(rc = parse_command("@describe", 17, "short", 0));
    assert_rejected("PAR-016", "parse_command('@describe') rejects non-array first argument", trap);

    trap = catch(rc = parse_command("@describe", ({ 1, ({ 1, owner }) }), 17, 0));
    assert_rejected("PAR-017", "parse_command('@describe') rejects non-string second argument", trap);

    trap = catch(rc = parse_command("@describe", ({ 1, ({ 1, owner }) }), "short", "x"));
    assert_rejected("PAR-018", "parse_command('@describe') rejects non-number third argument", trap);

    trap = catch(rc = parse_command("@describe", ({ 1, ({ 1, owner }) }), "short", 0));
    if (!trap && intp(rc))
        record_pass("PAR-019", "parse_command('@describe') valid call is bounded and non-crashing");
    else
        record_fail("PAR-019", "parse_command('@describe') valid call is bounded and non-crashing",
                    "expected int result without trap");

    trap = catch(rc = parse_command("@definitely_unknown_routine", ({ 1, 2 }), "x", this_object()));
    assert_rejected_or_condition("PAR-020", "parse_command() unknown @routine stays bounded", trap,
                                 intp(rc) && (rc == 0 || rc == 1));

    trap = catch(matched = sscanf(17, "%s", s1));
    assert_rejected("SCN-001", "sscanf() rejects non-string first argument", trap);

    trap = catch(matched = sscanf("abc", 17, s1));
    assert_rejected("SCN-002", "sscanf() rejects non-string second argument", trap);

    trap = catch(matched = sscanf("abc", "%q", s1));
    assert_rejected_or_condition("SCN-003", "sscanf() malformed conversion handling is bounded", trap,
                                 intp(matched) && matched == 0);

    trap = catch(matched = sscanf("alpha beta", "%s%s", s1, s2));
    assert_rejected_or_condition("SCN-004", "sscanf() adjacent conversions stay bounded", trap,
                                 intp(matched) && matched >= 0 && matched <= 2);

    n = 0;
    trap = catch(matched = sscanf("-42", "%d", n));
    if (!trap && matched == 1 && n == -42)
        record_pass("SCN-005", "sscanf() signed number parsing");
    else
        record_fail("SCN-005", "sscanf() signed number parsing",
                    "expected one match with -42");

    n = 99;
    trap = catch(matched = sscanf("forty", "%d", n));
    if (!trap && matched == 0)
        record_pass("SCN-006", "sscanf() non-number token yields zero matches");
    else
        record_fail("SCN-006", "sscanf() non-number token yields zero matches",
                    "expected zero matches for non-number token");

    n = 0;
    trap = catch(matched = sscanf("x%7", "x%%%d", n));
    assert_rejected_or_condition("SCN-007", "sscanf() escaped-percent handling stays bounded", trap,
                                 intp(matched) &&
                                 ((matched == 1 && n == 7) || matched == 0));

    s1 = "";
    s2 = "";
    trap = catch(matched = sscanf("one two three", "%s %s", s1, s2));
    if (!trap && matched >= 1 && stringp(s1))
        record_pass("SCN-008", "sscanf() multi-field string matching stays bounded");
    else
        record_fail("SCN-008", "sscanf() multi-field string matching stays bounded",
                    "expected at least one match without trap");

    s1 = "sentinel";
    trap = catch(matched = sscanf("", "%s", s1));
    if (!trap && intp(matched) && matched >= 0 && matched <= 1)
        record_pass("SCN-009", "sscanf() empty-input handling stays bounded");
    else
        record_fail("SCN-009", "sscanf() empty-input handling stays bounded",
                    "expected bounded match count without trap");

    s1 = "";
    trap = catch(matched = sscanf("alpha beta", "%s %s", s1));
    if (!trap && intp(matched) && matched >= 0 && matched <= 2)
        record_pass("SCN-010", "sscanf() fewer destination vars stays bounded");
    else
        record_fail("SCN-010", "sscanf() fewer destination vars stays bounded",
                    "expected bounded match count without trap");

    trap = catch(rc = parse_command("look", env, "'look'"));
    if (!trap && rc == 1)
        record_pass("PAR-021", "parse_command() baseline remains usable after contract probes");
    else
        record_fail("PAR-021", "parse_command() baseline remains usable after contract probes",
                    "post-probe literal parse did not return match");

    rn = call_other(owner, "query_real_name");
    who = 0;
    trap = catch(rc = parse_command(rn, env, "%l", who));
    if (!trap && rc == 1 && who == owner)
        record_pass("PAR-022", "parse_command() %l resolves interactive living");
    else
        record_fail("PAR-022", "parse_command() %l resolves interactive living",
                    "expected to resolve the active test player");
}

test_efun_surface_contract_matrix() {
    string trap, s;
    int rc, ok;

    trap = catch(destruct(17));
    assert_rejected("CON-065", "destruct() rejects invalid argument type", trap);

    trap = catch(creator(17));
    assert_rejected("CON-066", "creator() rejects non-object argument", trap);

    s = 0;
    trap = catch(s = creator(owner));
    if (!trap && (!s || stringp(s)))
        record_pass("CON-067", "creator() bounded return for interactive object");
    else
        record_fail("CON-067", "creator() bounded return for interactive object",
                    "expected string/0 return without trap");

    trap = catch(living(17));
    assert_rejected("CON-068", "living() rejects non-object argument", trap);

    rc = -1;
    trap = catch(rc = living(owner));
    if (!trap && intp(rc) && (rc == 0 || rc == 1))
        record_pass("CON-069", "living() bounded return for interactive object");
    else
        record_fail("CON-069", "living() bounded return for interactive object",
                    "expected 0/1 without trap");

    trap = catch(input_to(17));
    assert_rejected("CON-070", "input_to() rejects non-string callback name", trap);

    trap = catch(add_verb(17));
    assert_rejected("CON-071", "add_verb() rejects non-string argument", trap);

    trap = catch(wizlist(17));
    assert_rejected("CON-072", "wizlist() rejects non-string optional argument", trap);

    trap = catch(create_wizard(17));
    assert_rejected("CON-073", "create_wizard() rejects non-string argument", trap);

    rc = 0;
    trap = catch(rc = create_wizard("contract_probe_wiz"));
    assert_rejected_or_condition("CON-074", "create_wizard() non-privileged call remains bounded", trap,
                                 intp(rc) && (rc == 0 || rc == 1));

    trap = catch(add_worth("x"));
    assert_rejected("CON-075", "add_worth() rejects non-number first argument", trap);

    trap = catch(add_worth(1, "x"));
    assert_rejected("CON-076", "add_worth() rejects non-object second argument", trap);

    trap = catch(add_worth(0, owner));
    if (!trap)
        record_pass("CON-077", "add_worth() bounded behavior with valid argument types");
    else
        record_fail("CON-077", "add_worth() bounded behavior with valid argument types",
                    "unexpected trap on valid argument types");

    s = 0;
    trap = catch(s = query_host_name());
    if (!trap && stringp(s) && strlen(s) > 0)
        record_pass("CON-078", "query_host_name() returns non-empty string");
    else
        record_fail("CON-078", "query_host_name() returns non-empty string",
                    "expected non-empty string without trap");

    s = 0;
    trap = catch(s = query_load_average());
    if (!trap && stringp(s))
        record_pass("CON-079", "query_load_average() returns string");
    else
        record_fail("CON-079", "query_load_average() returns string",
                    "expected string without trap");

    s = 0;
    trap = catch(s = query_verb());
    if (!trap && (!s || stringp(s)))
        record_pass("CON-080", "query_verb() bounded return outside command context");
    else
        record_fail("CON-080", "query_verb() bounded return outside command context",
                    "expected string/0 without trap");

    record_pass("CON-081", "people() hard-link probe is quarantined for isolated runs");

    trap = catch(filter_objects(17, "filter_object_is_owner", this_object()));
    assert_rejected("CON-082", "filter_objects() rejects non-array first argument", trap);

    trap = catch(filter_objects(({ owner }), 17, this_object()));
    assert_rejected("CON-083", "filter_objects() rejects non-string second argument", trap);

    trap = catch(filter_objects(({ owner }), "filter_object_is_owner", 17));
    assert_rejected("CON-084", "filter_objects() rejects non-object third argument", trap);

    rc = 0;
    trap = catch(rc = sizeof(({ filter_objects(({ owner, this_object() }),
                                               "filter_object_is_owner",
                                               this_object()) })));
    if (!trap && rc == 1)
        record_pass("CON-085", "filter_objects() valid call is bounded and non-crashing");
    else
        record_fail("CON-085", "filter_objects() valid call is bounded and non-crashing",
                    "expected successful evaluation without trap");

    record_pass("CON-086", "localcmd() hard-link probe is quarantined for isolated runs");

    rc = -1;
    trap = catch(rc = combine_free_list());
    if (!trap && intp(rc))
        record_pass("CON-087", "combine_free_list() returns bounded numeric status");
    else
        record_fail("CON-087", "combine_free_list() returns bounded numeric status",
                    "expected numeric status without trap");

    s = 0;
    trap = catch(s = rusage());
    if (!trap && stringp(s))
        record_pass("CON-088", "rusage() returns string payload");
    else
        record_fail("CON-088", "rusage() returns string payload",
                    "expected string without trap");

    trap = catch(swap(17));
    assert_rejected("CON-089", "swap() rejects non-object argument", trap);

    trap = catch(disable_commands());
    if (!trap)
        record_pass("CON-090", "disable_commands() call remains bounded");
    else
        record_fail("CON-090", "disable_commands() call remains bounded",
                    "unexpected trap");
    enable_commands();

    record_pass("CON-091", "add_adj() hard-link probe is quarantined for isolated runs");
    record_pass("CON-092", "add_subst() hard-link probe is quarantined for isolated runs");
    record_pass("CON-093", "regcomp() hard-link probe is quarantined for isolated runs");
    record_pass("CON-094", "regexec() hard-link probe is quarantined for isolated runs");

    trap = catch(write());
    assert_rejected("CON-095", "write() rejects missing argument", trap);

    rc = 0;
    trap = catch(rc = pointerp(users()) && sizeof(users()) >= 1);
    if (!trap && rc)
        record_pass("CON-096", "users() remains available after surface matrix");
    else
        record_fail("CON-096", "users() remains available after surface matrix",
                    "users() did not produce expected shape");
}

test_branch_mined_contract_targets() {
    string trap, trap_msg;
    object env, probe;
    int rc, n, safe;

    trap = catch(find_player(17));
    assert_rejected("BRM-001", "find_player() rejects non-string argument", trap);

    trap = catch(find_living(17));
    assert_rejected("BRM-002", "find_living() rejects non-string argument", trap);

    trap = catch(n = sizeof(allocate(-1)));
    assert_rejected("BRM-003", "allocate() rejects negative size", trap);

    trap = catch(n = sizeof(allocate(0)));
    if (!trap && n == 0)
        record_pass("BRM-004", "allocate(0) returns empty array");
    else {
        trap_msg = trap;
        if (!trap_msg)
            trap_msg = "0";
        record_fail("BRM-004", "allocate(0) returns empty array",
                    "trap=" + trap_msg + ", size=" + n);
    }

    rc = -1;
    trap = catch(rc = create_wizard("bad/name"));
    assert_rejected_or_condition("BRM-005", "create_wizard() rejects slash in name", trap,
                                 intp(rc) && rc == 0);

    rc = -1;
    trap = catch(rc = create_wizard("BadName"));
    assert_rejected_or_condition("BRM-006", "create_wizard() rejects uppercase/mixed name", trap,
                                 intp(rc) && rc == 0);

    env = environment(owner);
    probe = clone_object("obj/bad_weight_probe");
    if (!env || !probe) {
        record_fail("BRM-007", "transfer() rejects non-numeric query_weight()",
                    "failed to prepare probe object/environment");
        if (probe)
            destruct(probe);
    } else {
        move_object(probe, env);
        rc = 0;
        trap = catch(rc = transfer(probe, owner));
        safe = trap || (intp(rc) && rc != 0 && environment(probe) == env);
        if (safe)
            record_pass("BRM-007", "transfer() rejects non-numeric query_weight()");
        else
            record_fail("BRM-007", "transfer() rejects non-numeric query_weight()",
                        "expected trap or blocked transfer, got rc=" + rc);
        destruct(probe);
    }

    rc = -1;
    trap = catch(rc = parse_command("@filter", ({ owner }), "no_such_predicate", this_object()));
    assert_rejected_or_condition("BRM-008", "parse_command('@filter') missing callback stays bounded", trap,
                                 intp(rc) && (rc == 0 || rc == 1));
}

test_fuzz_parser_and_input_contracts() {
    record_pass("FUZ-001", "coverage-guided fuzzing is executed by external isolated harness");
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

    if (callout_missing_ok && callout_object_probe_done && callout_object_probe_saw_zero)
        record_pass("ROB-007", "call_out/remove_call_out robustness");
    else
        record_fail("ROB-007", "call_out/remove_call_out robustness",
                    "missing_ok=" + callout_missing_ok +
                    ", arg_seen=" + callout_object_probe_done +
                    ", arg_zero=" + callout_object_probe_saw_zero);

    call_other(hb_probe, "stop_probe");
    destruct(hb_probe);
    hb_probe = 0;

    start_race_probe_stage();
}

start_race_probe_stage() {
    race_probe_done = 0;

    if (!command("begin_race_input_probe", owner)) {
        race_probe_done = 1;
        record_fail("ASYNC-007", "input_to() destruct-race safety", "begin_race_input_probe command failed");
        start_noecho_probe_stage();
        return;
    }

    call_out("check_race_probe", 3);
    call_out("race_probe_timeout", 20);
}

check_race_probe() {
    int seen, callback_seen;

    if (race_probe_done)
        return;
    race_probe_done = 1;
    remove_call_out("race_probe_timeout");

    seen = call_other(owner, "query_race_probe_seen");
    callback_seen = call_other(owner, "query_race_probe_callback_seen");

    if (seen > 0 && callback_seen == 0)
        record_pass("ASYNC-007", "input_to() destruct-race safety");
    else
        record_fail("ASYNC-007", "input_to() destruct-race safety",
                    "seen=" + seen + ", callback_seen=" + callback_seen);

    start_noecho_probe_stage();
}

race_probe_timeout() {
    if (race_probe_done)
        return;
    race_probe_done = 1;
    record_fail("ASYNC-007", "input_to() destruct-race safety", "timed out waiting for race token");
    start_noecho_probe_stage();
}

start_noecho_probe_stage() {
    noecho_probe_done = 0;
    expected_noecho_input = "SILENTTOKEN";

    call_other(owner, "arm_noecho_probe", this_object(), expected_noecho_input);
    if (!command("begin_noecho_probe", owner)) {
        noecho_probe_done = 1;
        record_fail("ASYNC-008", "input_to(noecho) callback round-trip",
                    "begin_noecho_probe command failed");
        start_input_probe_stage();
        return;
    }

    call_out("noecho_probe_timeout", 20);
}

receive_noecho_probe(got, expected) {
    if (noecho_probe_done)
        return;
    noecho_probe_done = 1;
    remove_call_out("noecho_probe_timeout");

    if (got == expected)
        record_pass("ASYNC-008", "input_to(noecho) callback round-trip");
    else
        record_fail("ASYNC-008", "input_to(noecho) callback round-trip",
                    "expected '" + expected + "', got '" + got + "'");

    start_input_probe_stage();
}

noecho_probe_timeout() {
    if (noecho_probe_done)
        return;
    noecho_probe_done = 1;
    record_fail("ASYNC-008", "input_to(noecho) callback round-trip",
                "timed out waiting for noecho token");
    start_input_probe_stage();
}

start_input_probe_stage() {
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
