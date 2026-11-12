# Maintainer release checklist

## Repository setup

- Confirm the repository name/owner, author identity for commits and the intended license attribution.
- Set up a private security-reporting channel (for example GitHub private vulnerability reporting) and update SECURITY.md accordingly.
- Choose an update-signing policy before distributing widely. Do not use a shared debug signing key as production trust.
- Authenticate through a connected account, local credential helper or a narrowly scoped deploy mechanism. Never send a private key or token in chat, commit it, or paste it into a workflow file.

## Validation

- Run `bash tests/run_host_tests.sh` and a clean Android debug build.
- Verify the APK signature and record SHA-256. Ensure APK/source/version correspond.
- Confirm installation/update and accepted shares on the test device; record exact model and Android version.
- Test screen-off operation, Stop, reconnect and a longer multi-worker session.
- Clearly label untested paths; complete Android block-candidate alert testing is still pending.
- Inspect the staged diff for credentials, payout addresses, generated files and exaggerated claims.

## Publish

1. Commit reviewed source under the real maintainer's chosen identity.
2. Push to the confirmed remote using the approved authentication mechanism.
3. Create a `v0.2.4-midstate-test` tag only after confirming the intended version/tag history; do not overwrite an existing published tag.
4. Publish source tags and **experimental pre-release notes** only, not a stable/profitability claim. Do not attach APK/AAB packages to the public repository release.
5. Prepare the optional paid, ready-to-install package separately: sign it with the maintainer's private distribution key, verify it, and provide its checksum and matching public source tag. Explain signing/update compatibility. Never commit the signing key.
6. Publish the paid download only after confirming platform policies, compatibility claims, update/support terms and applicable refund/tax obligations. The proposed price is €5; do not add a purchase link until the real page exists.
7. Add real, redacted screenshots and the actual purchase link to the README. Keep free self-build instructions complete and explain that payment is optional. The MIT license continues to permit redistribution under its terms.
8. Check the CI contains no binary-upload or release-publishing step. It should test, compile and remove its APK/AAB output directories, not distribute a ready-to-install package.

A rebuild may have a different APK checksum/signature from the supplied test artifact. Do not label an independently built package as the already tested binary without checking it. CI cleanup does not prevent anyone from building or redistributing the open-source code.

## Suggested release text

Title: **NerdPhone v0.2.4-midstate-test — Android Bitcoin solo miner (educational)**

> Configurable C++ workers, Stratum V1, Compose dashboard and an offline CPU benchmark, running on a repurposed Android phone.
>
> On one UMIDIGI Power 3 (Android 10) with eight workers and the screen off, the pool estimate stayed near **4 MH/s** for about **twelve hours**. The captured session reported **56,952 accepted** shares of **56,999 submitted**; the **47** rejections were all `Stale`. Battery use was about **10% per hour**, and the phone stayed cold to the touch with no cooling accessory. Internal temperature and wall power were not measured.
>
> This is educational lottery mining: there is no expected return, no yield and no investment advice. On a solo pool the block subsidy (3.125 BTC plus fees) goes entirely to whoever finds the block.
>
> Distribution-signed, versionCode 10, not debuggable. Because the distribution certificate differs from the debug certificate, a debug install must be uninstalled before installing this APK, and uninstalling loses saved settings.

The version name remains `0.2.4-midstate-test` to match the APK that was actually tested; do not silently rename or rebuild it and then claim binary identity. The paid customer package must be built from this exact APK, with its source/tag URL, checksum and final installation and support documentation, before the storefront goes live.
