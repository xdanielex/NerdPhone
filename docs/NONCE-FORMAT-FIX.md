# 0.2.1: strict-pool "No nonce" rejection

## Report

The tester connected to `pool.nerdminers.org:3333`, received difficulty 0.001 and reported every submission rejected with `error: No nonce`, `result: false`. The response `id` correlates the RPC request; it is not the submitted nonce.

## Application bug

Our formatter used `std::hex << nonce`, which strips leading zeroes. The relevant five-element Stratum submission still contained a nonce, but it could be shorter than eight hexadecimal digits. A strict parser can reject that as "No nonce" without evaluating its proof of work.

The pinned upstream NerdMiner reference also uses an unpadded formatter. An upstream report describes the same validation error and cites CKPool's check for `strlen(nonce) < 8`:

https://github.com/BitMaker-hub/NerdMiner_v2/issues/822

We are not claiming to have inspected the live pool's private deployment. The local formatting defect is established and matches the reported error.

## Why almost every share can fail here

A 32-bit uniformly distributed nonce has a leading zero hex digit 1/16 of the time. Our miner does not sample uniformly from the whole space: every new job starts at low nonces, partitioned between workers. Short jobs and a low share target keep the discovered nonces far below `0x10000000`, the first integer with eight nonzero-leading hexadecimal digits. Thus the bug can affect every share in such sessions, rather than only 6.25%.

## Fix

Use an eight-character, zero-padded, lowercase numeric hexadecimal encoding:

- 0 -> `00000000`
- 1 -> `00000001`
- 0x12ab -> `000012ab`
- 0xffffffff -> `ffffffff`

This changes only the submitted text representation. It does not change the numeric nonce, the header's little-endian nonce bytes, the SHA-256d calculation or the assigned target.

## Test gap and new coverage

The former mock reconstructed the header with `int(nonce, 16)` and therefore accepted short strings. This validated hashing but missed wire-format compatibility.

- C++ regression vectors verify width, padding and numeric round trips, including boundaries.
- The mock now rejects any nonce not exactly 8 lowercase hex digits before independently validating the hash.
- The mock now uses a nonzero previous-block hash, nonempty merkle branches and an 8-byte extranonce2 by default; 4-byte extranonce2 remains separately testable.
- Duplicate checking and multi-worker validation remain enabled.

These are offline tests. After installing the fix, confirm that submitted shares become accepted on the public pool. A different error such as Above target, Stale or Duplicate requires separate investigation; this release does not claim to have ruled those out on the user's device.
