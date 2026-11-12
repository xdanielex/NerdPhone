# Distribution signing

Release APKs are signed with a private RSA-3072 distribution identity, not the Android Debug certificate. The current build, **0.2.4-midstate-test** (versionCode 10), uses this identity. This is direct APK distribution, not Play App Signing, and it does not imply store approval or a security audit.

## Build public source without secrets

```sh
./gradlew --no-daemon :app:assembleRelease
```

Output: `app/build/outputs/apk/release/app-release-unsigned.apk`. The public Gradle configuration deliberately contains no private signing configuration. CI continues to verify a debug build and does not publish binary artifacts.

## Sign privately

Use JDK 17 and Android Build Tools 35.0.0. Obtain your saved keystore and recovery information from your private backup, not from this repository.

```sh
export ANDROID_HOME="/absolute/path/to/android-sdk"
export NERDPHONE_KEYSTORE="/absolute/private/path/nerdphone-release.p12"
export NERDPHONE_KEY_ALIAS="nerdphone-release"
read -r -s -p "Keystore password: " NERDPHONE_STORE_PASSWORD
printf '\n'
export NERDPHONE_STORE_PASSWORD
# PKCS12 backup uses the same password for store and key.
bash tools/sign_release.sh \
  app/build/outputs/apk/release/app-release-unsigned.apk \
  /absolute/output/path/NerdPhone-v0.2.4-midstate-test.apk
unset NERDPHONE_STORE_PASSWORD NERDPHONE_KEY_PASSWORD
```

Do not enable shell tracing or log passwords. The script aligns, signs and verifies the APK; it does not upload anything. `zipalign -P 16` verifies archive layout, not that every bundled ELF supports every device page size. Runtime device testing is still required.

## First transition from debug builds

The distribution certificate differs from the old Android Debug certificate. A distribution APK therefore cannot update the installed debug APK in place. First stop mining, record your public pool/settings, uninstall the debug app and install the distribution APK. Uninstallation removes private app preferences, counters/candidate records stored locally and notification configuration. Pool-side history is independent.

Future direct APK updates must use this same distribution identity, package `it.nerdphone`, and an appropriately increasing versionCode. Changing the key later is not a routine version bump. Keep at least two encrypted offline backups of the keystore and recovery credentials. Do not rely on a chat workspace as the only backup.

## What may be public

The APK, its checksum, public signing certificate/fingerprint, source tag, license and customer guide may be distributed. A fingerprint verifies identity/integrity; it does not prove the app safe or endorsed by Android/pool operators.

## What must stay private

Keystore/private key, store/key passwords, recovery files and private backup archives. Never include them in a source ZIP, Git, a release attachment, the paid download, a support ticket or a chat message. The source `.gitignore` is only one precaution; always inspect exports manually.

A password-protected PKCS12 file plus its recovery password in the same ZIP means that the ZIP as a whole is sensitive. Keep that backup in encrypted storage. An ordinary ZIP container is not itself encrypted.
