#!/usr/bin/env python3
"""Isolated command-surface fuzzer for the LPmud test mudlib.

This harness is intentionally process-isolated from the default `runtests`
suite. It focuses on parser/input surfaces through randomized interactive
commands and reports disconnects/crash-like behavior.
"""

from __future__ import annotations

import argparse
import json
import random
import socket
import string
import sys
import time
from pathlib import Path


BASE_PAYLOADS = [
    "",
    " ",
    "x",
    "xx",
    "east",
    "west",
    "say",
    "look",
    "probeenv",
    "a" * 8,
    "a" * 64,
    "a" * 256,
    "a" * 999,
    "a" * 1000,
    "%s",
    "%d",
    "%%",
    "%s%s%s",
    "'quoted'",
    "\"double-quoted\"",
    "[the] %w",
    "@filter",
    "@describe",
    "../path",
    "..\\path",
    "tab\tchar",
    "nul\x00char",
    "ctrl\x7fchar",
    "or || and &&",
    "bit << >> ^ | &",
]


TEMPLATES = [
    "{payload}",
    "say {payload}",
    "look {payload}",
    "look at {payload}",
    "probeenv {payload}",
    "who {payload}",
    "score {payload}",
]


ERROR_MARKERS = [
    "[FAIL]",
    "Error in loading object",
    "Runtime error",
    "Fatal",
    "segmentation fault",
]


def decode_chunk(data: bytes) -> str:
    return data.decode("latin1", errors="ignore")


def send_line(sock: socket.socket, line: str) -> None:
    clean = line.replace("\r", " ").replace("\n", " ")
    sock.sendall((clean + "\n").encode("latin1", errors="ignore"))


def recv_until(sock: socket.socket, needle: str, timeout_s: float) -> tuple[str, bool, bool]:
    deadline = time.time() + timeout_s
    buf = ""
    while time.time() < deadline:
        sock.settimeout(max(0.05, deadline - time.time()))
        try:
            chunk = sock.recv(4096)
        except socket.timeout:
            continue
        if not chunk:
            return buf, False, True
        buf += decode_chunk(chunk)
        if needle in buf:
            return buf, True, False
    return buf, False, False


def recv_brief(sock: socket.socket, duration_s: float = 0.6, idle_s: float = 0.08) -> tuple[str, bool]:
    end = time.time() + duration_s
    buf = ""
    saw_data_at = time.time()
    while time.time() < end:
        sock.settimeout(min(idle_s, max(0.01, end - time.time())))
        try:
            chunk = sock.recv(4096)
        except socket.timeout:
            if time.time() - saw_data_at >= idle_s:
                break
            continue
        if not chunk:
            return buf, True
        saw_data_at = time.time()
        buf += decode_chunk(chunk)
        if "> " in buf:
            break
    return buf, False


def mutate_payload(rng: random.Random, base: str) -> str:
    op = rng.choice(["insert", "delete", "duplicate", "flip", "none"])
    if op == "none" or base == "":
        return base
    if op == "insert":
        pos = rng.randrange(0, len(base) + 1)
        ch = rng.choice(string.ascii_letters + string.digits + " %'\"[](){}<>|&^+-=*/\t")
        return base[:pos] + ch + base[pos:]
    if op == "delete" and len(base) > 0:
        pos = rng.randrange(0, len(base))
        return base[:pos] + base[pos + 1 :]
    if op == "duplicate" and len(base) > 0:
        i = rng.randrange(0, len(base))
        j = rng.randrange(i, len(base))
        return base + base[i : j + 1]
    if op == "flip" and len(base) > 0:
        pos = rng.randrange(0, len(base))
        ch = base[pos]
        if "a" <= ch <= "z":
            ch = ch.upper()
        elif "A" <= ch <= "Z":
            ch = ch.lower()
        else:
            ch = rng.choice("!@#$%^&*()_+[]{}")
        return base[:pos] + ch + base[pos + 1 :]
    return base


def random_payload(rng: random.Random, max_len: int) -> str:
    if rng.random() < 0.7:
        base = rng.choice(BASE_PAYLOADS)
    else:
        n = rng.randint(0, max_len)
        alphabet = string.ascii_letters + string.digits + " %'\"[](){}<>|&^+-=*/\t"
        base = "".join(rng.choice(alphabet) for _ in range(n))
    if rng.random() < 0.8:
        base = mutate_payload(rng, base)
    return base[:max_len]


def run(args: argparse.Namespace) -> int:
    rng = random.Random(args.seed)
    transcript_lines: list[str] = []
    anomalies: list[str] = []
    fail_lines: list[str] = []
    iterations_completed = 0
    disconnected = False

    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.settimeout(args.connect_timeout)
    try:
        sock.connect((args.host, args.port))
    except Exception as exc:
        payload = {
            "status": "connect_failed",
            "error": str(exc),
            "host": args.host,
            "port": args.port,
        }
        Path(args.output_json).write_text(json.dumps(payload, indent=2) + "\n")
        Path(args.transcript).write_text("\n".join(transcript_lines) + "\n")
        return 1

    banner, ok, closed = recv_until(sock, "Login name", args.phase_timeout)
    transcript_lines.append("<<< " + banner)
    if closed or not ok:
        payload = {
            "status": "login_prompt_missing",
            "closed": closed,
            "received": banner,
        }
        Path(args.output_json).write_text(json.dumps(payload, indent=2) + "\n")
        Path(args.transcript).write_text("\n".join(transcript_lines) + "\n")
        return 1

    send_line(sock, args.user)
    transcript_lines.append(">>> " + args.user)

    post_login, ok, closed = recv_until(sock, "> ", args.phase_timeout)
    transcript_lines.append("<<< " + post_login)
    if closed or not ok:
        payload = {
            "status": "post_login_prompt_missing",
            "closed": closed,
            "received": post_login,
        }
        Path(args.output_json).write_text(json.dumps(payload, indent=2) + "\n")
        Path(args.transcript).write_text("\n".join(transcript_lines) + "\n")
        return 1

    for warm in ("help", "look", "probeenv"):
        send_line(sock, warm)
        transcript_lines.append(">>> " + warm)
        out, closed = recv_brief(sock, duration_s=0.8)
        transcript_lines.append("<<< " + out)
        if closed:
            disconnected = True
            break

    if not disconnected:
        for i in range(args.iterations):
            payload = random_payload(rng, args.max_payload_len)
            template = rng.choice(TEMPLATES)
            cmd = template.format(payload=payload)
            if cmd == "":
                cmd = " "

            send_line(sock, cmd)
            transcript_lines.append(f">>> #{i+1} {cmd}")

            out, closed = recv_brief(sock, duration_s=args.command_timeout)
            transcript_lines.append("<<< " + out)
            iterations_completed += 1

            for line in out.splitlines():
                for marker in ERROR_MARKERS:
                    if marker in line:
                        if line not in anomalies:
                            anomalies.append(line)
                        if line.startswith("[FAIL]") and line not in fail_lines:
                            fail_lines.append(line)

            if closed:
                disconnected = True
                break

    if not disconnected:
        try:
            send_line(sock, "quit")
            transcript_lines.append(">>> quit")
            out, _ = recv_brief(sock, duration_s=0.8)
            transcript_lines.append("<<< " + out)
        except Exception:
            disconnected = True

    try:
        sock.close()
    except Exception:
        pass

    report = {
        "status": "ok" if not disconnected else "disconnect",
        "seed": args.seed,
        "host": args.host,
        "port": args.port,
        "user": args.user,
        "iterations_requested": args.iterations,
        "iterations_completed": iterations_completed,
        "disconnected": disconnected,
        "crash_suspected": disconnected and iterations_completed < args.iterations,
        "anomaly_count": len(anomalies),
        "anomalies": anomalies[:200],
        "fail_line_count": len(fail_lines),
        "fail_lines": fail_lines[:200],
    }
    Path(args.output_json).write_text(json.dumps(report, indent=2) + "\n")
    Path(args.transcript).write_text("\n".join(transcript_lines) + "\n")
    return 2 if report["crash_suspected"] else 0


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--host", default="127.0.0.1")
    parser.add_argument("--port", type=int, default=2000)
    parser.add_argument("--user", default="fuzzuser")
    parser.add_argument("--seed", type=int, default=1337)
    parser.add_argument("--iterations", type=int, default=250)
    parser.add_argument("--max-payload-len", type=int, default=2048)
    parser.add_argument("--connect-timeout", type=float, default=5.0)
    parser.add_argument("--phase-timeout", type=float, default=20.0)
    parser.add_argument("--command-timeout", type=float, default=0.9)
    parser.add_argument("--output-json", required=True)
    parser.add_argument("--transcript", required=True)
    return parser.parse_args()


if __name__ == "__main__":
    sys.exit(run(parse_args()))
