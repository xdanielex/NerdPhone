# Experimental midstate build — 0.2.4-midstate-test

This is a device-test build, not a validated commercial release.

## Scope

A dedicated 80-byte header hasher caches the SHA-256 state after the first 64 bytes. Each nonce requires the remaining first-hash compression and one second-hash compression. Padding and intermediate data use fixed-size stack arrays, avoiding generic SHA vector allocations in the nonce loop. The midstate is rebuilt for every newly constructed header/extranonce2. Generic sha/sha2 remain unchanged for coinbase/merkle computation and as a test reference. Offline CPU testing uses the optimized path too.

Pool protocol, targets, nonce encoding, worker partitioning, queueing, service behavior and CPU policy are unchanged. No ARM SHA instructions or thermal throttles were added.

## Verification

20,480 randomized/boundary cases compare the optimized digest byte-for-byte against the existing generic sha2 implementation. Tests include zero nonce, UINT32_MAX, leading-zero nonce and invalid header length. Existing known-answer/core tests and independent Python mock pool share validation also run, including multi-worker duplicate checking and difficulty negotiation.

Host speed measurements are diagnostic only and cannot predict phone performance. A first tester has since reported successful screen-off operation, accepted shares and approximately 3.93 MH/s of share-equivalent work across 17 minutes 57 seconds. The build is retained as the preferred testing reference; longer-duration and broader-device validation remain pending. See the field report for exact observations and limitations.

## Upgrade and rollback

- Experimental APK has versionCode 10 and the same distribution signing identity as releases 0.2.2/0.2.3. It can update those distribution installs without an uninstall, subject to normal Android installation behavior.
- Debug-signed installs still require uninstalling first, losing local settings/records.
- Original 0.2.3 APK (versionCode 9) and a pre-experiment source archive are retained separately.
- Android normally blocks installing a lower versionCode over a newer install. Returning to the original 0.2.3 APK therefore normally requires uninstalling the experiment first, losing local settings/records. Record configuration and any important local candidate data before changing builds.
- A future fallback build could retain the original engine with a higher versionCode and the same distribution signature, avoiding that downgrade constraint; it has not been built as part of this experiment.

## Suggested comparison

Use the same device, worker count, screen state, power and network conditions. Compare several offline benchmark runs on old/new builds, then at least 30–60 minutes of pool operation. Record submitted/accepted/rejected counts and reasons, plus duration. Pool estimates alone fluctuate with luck. Do not treat short peaks as sustained throughput or infer power efficiency without measuring energy.
