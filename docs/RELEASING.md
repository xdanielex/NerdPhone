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

Title: **NerdPhone v0.2.4-midstate-test — Experimental Android Bitcoin solo miner**

> Selected midstate-optimized release build, with configurable C++ workers, Stratum V1, Compose dashboard and offline CPU benchmark. On one UMIDIGI Power 3 / Android 10 with eight workers, supplied pool counters imply approximately 3.95 MH/s over nearly 22 minutes, with screen-off operation reported. All shares were reported accepted at successive checks; no instrumented power or internal-temperature measurement was made. Performance and background behavior vary by device. This is educational lottery mining, not a wallet or income promise.
>
> Same distribution signature as 0.2.2/0.2.3, versionCode 10. Existing distribution installs can update; debug installs require uninstall/reinstall. The original lower-version APK cannot normally be installed over this build without uninstalling. See MIDSTATE-EXPERIMENT.md.

The name remains `0.2.4-midstate-test` to match the selected, physically tested APK; do not silently rename/rebuild it and claim binary identity. The paid customer package still needs refreshing to this exact APK, its source/tag URL and final installation/support documentation before sale. Existing 0.2.3 customer ZIPs are historical, not the selected optimized product.
