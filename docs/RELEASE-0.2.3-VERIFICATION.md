# 0.2.3 icon release verification

- Package it.nerdphone, versionName 0.2.3, versionCode 9, release build not debuggable.
- Signed with exactly the 0.2.2 distribution identity; certificate SHA-256: `43bb0e9fd881b7bbe8f0739a03774e51319a94992e72efbc3e6dbaec69be2962`.
- Signed APK SHA-256: `a315489b22ce7a8c8ccd4c94a74591710ec8724c762da54d4ce8f14ed32425ed`.
- Build, vital lint, signature v2/v3, ZIP integrity and archive alignment checks passed.
- Manifest resolves launcher and round launcher icons, with legacy density resources, adaptive layers and API 33+ monochrome support.
- Same-key upgrade from distribution release 0.2.2 is supported by package/version/signing metadata, but installation and actual launcher rendering still need device testing.
- Older debug-signed builds still require uninstall/reinstall when moving to distribution signing.
- Mining and networking behavior was not modified in this icon release. No new physical-device benchmark is claimed.
- Icon artwork is AI-generated, selected by the maintainer and adapted into Android resources. This does not guarantee trademark clearance or exclusivity.
