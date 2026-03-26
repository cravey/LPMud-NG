# SSL/TLS Build and Runtime Guide

## 1) Summary of the SSL Build Process and Dependencies

SSL support is optional and controlled by two gates:

- build-time: compile with `USE_SSL=1`
- runtime: enable with `MUD_SSL=1`

If runtime requests SSL on a non-SSL build, startup fails fast with:

- `MUD_SSL=1 requested, but parse was built without USE_SSL=1.`

Build wiring in `Makefile`:

- adds `-DUSE_SSL` when `USE_SSL=1`
- links `-lssl -lcrypto`
- uses `OPENSSL_ROOT` include/lib paths when set (and also has auto-detection for common OpenSSL locations)

Dependencies for SSL build:

- OpenSSL development headers/libraries
- normal driver build dependencies from `README.md`

SSL build commands:

```bash
make clean
make parse USE_SSL=1
```

If OpenSSL is not in default compiler/linker search paths:

```bash
make parse USE_SSL=1 OPENSSL_ROOT=/path/to/openssl
```

## 2) Usage of an SSL Build

Run SSL-enabled `parse`:

```bash
MUD_SSL=1 \
MUD_SSL_CERT_FILE=/abs/path/server.crt \
MUD_SSL_KEY_FILE=/abs/path/server.key \
MUD_LIB=./mudlib \
MUD_BIND_ADDR=127.0.0.1 \
./parse
```

Optional client-certificate verification:

```bash
MUD_SSL=1 \
MUD_SSL_CERT_FILE=/abs/path/server.crt \
MUD_SSL_KEY_FILE=/abs/path/server.key \
MUD_SSL_CA_FILE=/abs/path/ca_bundle.pem \
MUD_SSL_VERIFY_CLIENT=1 \
MUD_LIB=./mudlib \
MUD_BIND_ADDR=127.0.0.1 \
./parse
```

`MUD_SSL` truthiness:

- false when unset, empty, `0`, `false`, `no`, or `off` (case-insensitive)
- any other non-empty value enables SSL mode

Useful verification paths:

- non-SSL regression: `make mudlibtest` or `./tests/run_test_mudlib_suite.sh`
- SSL integration: `./tests/run_test_mudlib_ssl_suite.sh`

## 3) Certificate and Key Requirements (Creation and Use)

Required formats:

- server certificate: PEM (`MUD_SSL_CERT_FILE`)
- server private key: PEM (`MUD_SSL_KEY_FILE`)
- optional CA bundle: PEM (`MUD_SSL_CA_FILE`)
- cert and key must match

Quick self-signed cert generation for local/dev:

```bash
mkdir -p tests/tls
openssl req -x509 -newkey rsa:2048 -nodes \
  -keyout tests/tls/server.key \
  -out tests/tls/server.crt \
  -days 1 \
  -subj "/CN=localhost"
```

Use generated cert/key:

```bash
MUD_SSL=1 \
MUD_SSL_CERT_FILE=tests/tls/server.crt \
MUD_SSL_KEY_FILE=tests/tls/server.key \
MUD_LIB=./mudlib \
MUD_BIND_ADDR=127.0.0.1 \
./parse
```

Operational recommendations:

- use CA-issued certs for production
- restrict private key file permissions
- rotate certificates before expiry and restart `parse`

## 4) Additional SSL Build/Run/Maintenance Notes (Including Clients)

Compatibility notes:

- server uses OpenSSL TLS server mode with minimum TLS 1.2 when available in headers
- clients must start directly with TLS (no STARTTLS handshake mode)

Known compatible clients:

- `openssl s_client` (used by the SSL test harness)
- custom TLS-capable line-oriented clients
- plain `nc`/telnet are for non-SSL mode only

Rollout/maintenance approach:

1. build with `USE_SSL=1`
2. validate non-SSL behavior
3. run SSL suite
4. enable `MUD_SSL=1` in deployment

Rollback:

- set `MUD_SSL` false or unset it, then restart

Common SSL failures:

- missing OpenSSL headers/libraries at build time
- invalid cert/key paths
- cert/key mismatch
- `MUD_SSL_VERIFY_CLIENT=1` without `MUD_SSL_CA_FILE`
