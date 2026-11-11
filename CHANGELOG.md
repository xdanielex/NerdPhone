# Changelog

## 0.2.4-midstate-test (experimental)
- Add cached first-block SHA-256 midstate and fixed-size padding for 80-byte header mining and offline CPU tests.
- Keep generic hashing and all network/target/worker behavior unchanged.
- Add 20,480 randomized/boundary equivalence comparisons and retain independent pool share validation.
- Selected as the preferred build following the first UMIDIGI Power 3 screen-off trial: approximately 3.95 MH/s share-equivalent work over nearly 22 minutes between supplied snapshots; no rejected shares reported at successive checks. Longer/broader validation remains pending.
- See [experiment and rollback notes](docs/MIDSTATE-EXPERIMENT.md).

## 0.2.3
- Add selected NerdPhone launcher artwork: legacy square/round density assets, adaptive icon and Android 13+ themed monochrome layer.
- Wire icon and roundIcon into the manifest; include public branding PNGs and provenance notes in the source.
- VersionCode 9, same private distribution certificate as 0.2.2. Existing distribution release installs can update in place; old Debug installs still require uninstall/reinstall.
- No mining/network behavior changes. New icon rendering still needs visual testing on physical launchers.

## 0.2.2
- First intended distribution-signed release build (versionCode 8); release debuggability explicitly disabled, no minification/obfuscation added.
- No mining algorithm, thread-allocation or pool-protocol changes from 0.2.1.
- Public release builds remain unsigned until the maintainer runs a separate signing script with a privately held key; no secrets in Gradle or CI.
- New distribution certificate differs from the old Debug certificate, so migration from debug requires uninstall/reinstall and loses local settings.
- Add signing/update/backup instructions; test harness tolerates the expected TCP close race at the end of a timed mock run while preserving protocol/exit-code assertions.
- Physical-device installation/share acceptance of this exact release binary still require retesting before commercial publication.

## 0.2.1
- Critical fix: encode mining.submit nonce as exactly 8 zero-padded hex characters. Short nonce strings could be rejected as No nonce before hash validation.
- The sequential low nonce search on short jobs made this far more frequent than the 1-in-16 rate of a uniform full-range nonce distribution.
- Default pool changed to pool.nerdminers.org:3333; preserve existing saved host settings.
- Add boundary/round-trip formatter tests. Enforce nonce length and hex charset in the independent mock pool; exercise nonzero prevhash, merkle branches and 8-byte extranonce2.
- Update diagnostics documentation and test reports. Real pool acceptance remains to be confirmed on device.

## 0.2.0
- Detect configured/online/app-available logical CPUs on each device using native OS APIs; refresh at activity launch and service startup.
- Configure 1..available mining threads; preserve one dedicated network worker.
- Disjoint strided nonce partitions per extranonce2, no affinity pinning.
- Live per-miner hash rate and offline 3-second synthetic-header benchmark with per-worker results.
- Clamp saved selections to the current device, default to one worker, prevent concurrent benchmark/mining startup.
- Extend tests and CI for multi-worker share validity, duplicate detection, CPU discovery, benchmark execution and partition coverage.

## 0.1.4
- English interface, foreground notification, candidate alerts and app-generated native statuses/errors.
- Display the existing NerdMiner-compatible user agent explicitly.
- Count difficulty suggestion requests and display associated RPC replies/errors without treating acknowledgements as target assignments.
- Add regression tests proving that suggestion success/rejection cannot override difficulty 1 announced by a pool.
- English README, contribution/security notes, Git ignore rules and offline-test/build CI.
- Document primary-source research and short authorized NerdMiner.io probes; neither password x nor experimental d=0.001 obtained an observed 0.001 assignment.

## 0.1.3
- Live Stratum job monitor: received, assigned and started counts; job IDs, working hashes and difficulty; job/message ages and clean_jobs.
- Clear current work on disconnect/stop; retain session totals until restart.
- Extend mock tests to verify job telemetry.

## 0.1.2
- Default pool for new settings: pool.nerdminer.io:3333; preserve existing preferences.
- Request difficulty 0.001 and display the actual assigned value separately.
- Use initial Stratum difficulty 1 until announced otherwise.
- Detailed TCP phase, errno, endpoint and SO_ERROR diagnostics.
- Read available data before handling a pending socket error; enable TCP keepalive and TCP_NODELAY.
- Android 10 socket failure remains device/network-dependent; diagnostics do not prove an OS-specific fix.

## 0.1.1
- Enable legacy compressed/extracted native-library packaging for compatibility testing.

## 0.1.0
- Initial C++17/Compose/JNI miner, two-worker architecture and foreground service.
- SHA-256d and mock Stratum tests; signed Android debug APK.
