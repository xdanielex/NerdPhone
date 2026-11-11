# 0.2.2 release verification

- Package: `it.nerdphone`; versionName `0.2.2`; versionCode `8`.
- Minimum SDK 26; target SDK 35. ARM32, ARM64 and x86_64 native libraries included.
- Release build completed, with vital lint passing. Release is not debuggable; no minification was introduced.
- Fresh distribution certificate: `CN=NerdPhone Distribution`, RSA 3072.
- Public certificate SHA-256: `43bb0e9fd881b7bbe8f0739a03774e51319a94992e72efbc3e6dbaec69be2962`.
- Exact signed APK SHA-256: `d252f016b76f9e9c2a394c598777e5b14a708a72cecd7f3012e3d60e20b339b2`.
- APK signature schemes v2 and v3, ZIP integrity and archive alignment verified.
- Offline host suite passed after fixing an end-of-test mock socket-close race; protocol/hash assertions remain enforced.
- This certificate differs from the prior Debug certificate: uninstall debug before migrating, losing local preferences.
- No private key/password is in the public repository or customer package. The private backup is an independent maintainer-only deliverable.
- Actual installation, share acceptance, screen-off behavior and candidate alerts of this exact release binary still require physical-device testing before sale. Earlier field results refer to development builds.
- Archive alignment does not certify all bundled ELF page sizes or universal modern-device compatibility.
- Not uploaded or published to a storefront; no payment, licensing server or DRM added.
