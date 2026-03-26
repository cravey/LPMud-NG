# SSL/TLS Build and Runtime Guide

This is the canonical SSL/TLS reference for the `lpmud/` driver.

---

## 1) Summary of the SSL Build Process and Dependencies

SSL support is optional and controlled by two gates:

- build-time: compile with `USE_SSL=1`
- runtime: enable with `MUD_SSL=1`

If runtime requests SSL on a non-SSL build, startup fails fast with:

- `MUD_SSL=1 requested, but parse was built without USE_SSL=1.`

### Build Wiring in `Makefile`

- adds `-DUSE_SSL` when `USE_SSL=1`
- links `-lssl -lcrypto`
- uses `OPENSSL_ROOT` include/lib paths when set
- also auto-detects common OpenSSL installation paths

### SSL Build Dependencies

- OpenSSL development headers/libraries
- standard driver build dependencies from [README.md](README.md)

### SSL Build Commands

```bash
make clean
make parse USE_SSL=1
```

If OpenSSL is outside default compiler/linker search paths:

```bash
make parse USE_SSL=1 OPENSSL_ROOT=/path/to/openssl
```

---

## 2) Usage of an SSL Build

### Run SSL-Enabled `parse`

```bash
MUD_SSL=1 \
MUD_SSL_CERT_FILE=/abs/path/server.crt \
MUD_SSL_KEY_FILE=/abs/path/server.key \
MUD_LIB=./mudlib \
MUD_BIND_ADDR=127.0.0.1 \
./parse
```

### Optional Client-Certificate Verification

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

### `MUD_SSL` Truthiness

- false when unset, empty, `0`, `false`, `no`, or `off` (case-insensitive)
- any other non-empty value enables SSL mode

### Useful Verification Paths

- non-SSL regression: `make mudlibtest` or `./tests/run_test_mudlib_suite.sh`
- SSL integration: `./tests/run_test_mudlib_ssl_suite.sh`

---

## 3) Certificate and Key Requirements (Creation and Use)

### Required Formats

- server certificate: PEM (`MUD_SSL_CERT_FILE`)
- server private key: PEM (`MUD_SSL_KEY_FILE`)
- optional CA bundle: PEM (`MUD_SSL_CA_FILE`)
- certificate and key must match

### Quick Self-Signed Cert Generation (Local/Dev)

```bash
mkdir -p tests/tls
openssl req -x509 -newkey rsa:2048 -nodes \
  -keyout tests/tls/server.key \
  -out tests/tls/server.crt \
  -days 1 \
  -subj "/CN=localhost" \
  -addext "subjectAltName=DNS:localhost,IP:127.0.0.1"
```

Set restrictive permissions on the private key:

```bash
chmod 600 tests/tls/server.key
```

### Use the Generated Cert/Key

```bash
MUD_SSL=1 \
MUD_SSL_CERT_FILE=tests/tls/server.crt \
MUD_SSL_KEY_FILE=tests/tls/server.key \
MUD_LIB=./mudlib \
MUD_BIND_ADDR=127.0.0.1 \
./parse
```

### Verify Certificate/Key Material

Inspect certificate subject, validity window, and SAN:

```bash
openssl x509 -in tests/tls/server.crt -noout -subject -issuer -dates -ext subjectAltName
```

Validate private key structure:

```bash
openssl pkey -in tests/tls/server.key -noout -check
```

Check that certificate and key are a pair:

```bash
openssl x509 -in tests/tls/server.crt -noout -modulus | openssl md5
openssl rsa -in tests/tls/server.key -noout -modulus | openssl md5
```

The two digests above should match.

### Operational Recommendations

- use CA-issued certs for production
- restrict private key file permissions
- rotate certificates before expiry and restart `parse`

---

## 4) Additional SSL Build/Run/Maintenance Notes (Including Clients)

### Compatibility Notes

- server uses OpenSSL TLS server mode
- minimum TLS version is TLS 1.2 when supported by available OpenSSL headers
- clients must start directly with TLS (no STARTTLS handshake mode)

### Known Compatible Clients

- `openssl s_client` (used by the SSL test harness)
- custom TLS-capable line-oriented clients
- plain `nc`/telnet are for non-SSL mode only

Example client probe against localhost:

```bash
openssl s_client -connect 127.0.0.1:2000 -servername localhost -CAfile tests/tls/server.crt -quiet
```

### Rollout and Maintenance Approach

1. build with `USE_SSL=1`
2. validate non-SSL behavior
3. run SSL suite
4. enable `MUD_SSL=1` in deployment

Rollback:

- set `MUD_SSL` false or unset it, then restart

### Common SSL Failures

- missing OpenSSL headers/libraries at build time
- invalid cert/key paths
- cert/key mismatch
- `MUD_SSL_VERIFY_CLIENT=1` without `MUD_SSL_CA_FILE`
