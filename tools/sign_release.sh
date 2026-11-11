#!/usr/bin/env bash
# Sign a locally built unsigned release APK without embedding secrets in Gradle/CI.
set -euo pipefail
umask 077
if [[ $# -ne 2 ]]; then
  echo "Usage: $0 UNSIGNED_RELEASE_APK SIGNED_OUTPUT_APK" >&2
  exit 2
fi
: "${ANDROID_HOME:?Set ANDROID_HOME to your Android SDK}"
: "${NERDPHONE_KEYSTORE:?Set the absolute path to your private keystore}"
: "${NERDPHONE_KEY_ALIAS:?Set the private key alias}"
: "${NERDPHONE_STORE_PASSWORD:?Set the keystore password via a secure prompt}"
export NERDPHONE_STORE_PASSWORD
export NERDPHONE_KEY_PASSWORD="${NERDPHONE_KEY_PASSWORD:-$NERDPHONE_STORE_PASSWORD}"
input="$1"
output="$2"
[[ -f "$input" ]] || { echo "Input APK not found" >&2; exit 2; }
[[ -f "$NERDPHONE_KEYSTORE" ]] || { echo "Keystore not found" >&2; exit 2; }
[[ "$input" != "$output" ]] || { echo "Input and output must be different" >&2; exit 2; }
tools="$ANDROID_HOME/build-tools/35.0.0"
[[ -x "$tools/apksigner" && -x "$tools/zipalign" ]] || { echo "Install Android Build Tools 35.0.0" >&2; exit 2; }
tmp="$(mktemp -d)"
trap 'rm -rf "$tmp"' EXIT
"$tools/zipalign" -f -P 16 4 "$input" "$tmp/aligned.apk"
"$tools/apksigner" sign --ks "$NERDPHONE_KEYSTORE" \
  --ks-key-alias "$NERDPHONE_KEY_ALIAS" \
  --ks-pass env:NERDPHONE_STORE_PASSWORD --key-pass env:NERDPHONE_KEY_PASSWORD \
  --out "$output" "$tmp/aligned.apk"
"$tools/apksigner" verify --verbose --print-certs "$output"
"$tools/zipalign" -c -P 16 4 "$output"
echo "Signed APK verified. Store its SHA-256 and public certificate fingerprint with the release."
