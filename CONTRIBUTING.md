# Contributing

Run `bash tests/run_host_tests.sh` and an Android debug build before submitting a change. Include device model, Android version, ABI, pool host/port, app version and sanitized diagnostics when reporting a bug.

Never include seeds, private keys, signing keys or pool passwords in an issue. A Bitcoin address is public but can identify your mining activity: redact it if you do not want it associated with your account.

Keep networking on its dedicated worker, separate from the configurable mining workers. Preserve disjoint nonce searches across workers. Changes to target calculation, endianess, nonce/extranonce progression or job invalidation need independent test vectors. Never equate share acceptance with a confirmed Bitcoin block.

Live pool tests must be explicitly initiated, short, and low-impact. Do not automatically sweep user-agent/password combinations or submit invalid shares to public pools. Prefer the local mock.

The NerdMiner-compatible user agent must be documented as compatibility behavior, not proof of original firmware or operator endorsement. Do not claim that changing a user agent bypasses a pool's difficulty policy.
