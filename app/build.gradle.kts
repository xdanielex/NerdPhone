plugins { id("com.android.application"); id("org.jetbrains.kotlin.android"); id("org.jetbrains.kotlin.plugin.compose") }
android {
 namespace = "it.nerdphone"
 compileSdk = 35
 ndkVersion = "27.0.12077973"
 defaultConfig {
  applicationId = "it.nerdphone"
  minSdk = 26
  targetSdk = 35
  versionCode = 10
  versionName = "0.2.4-midstate-test"
  ndk { abiFilters += listOf("arm64-v8a", "armeabi-v7a", "x86_64") }
  externalNativeBuild { cmake { cppFlags += "-std=c++17" } }
 }
 externalNativeBuild { cmake { path = file("src/main/cpp/CMakeLists.txt"); version = "3.22.1" } }
 packaging { jniLibs { useLegacyPackaging = true } }
 buildFeatures { compose = true }
 compileOptions { sourceCompatibility = JavaVersion.VERSION_17; targetCompatibility = JavaVersion.VERSION_17 }
 kotlinOptions { jvmTarget = "17" }
 // Distribution signing is performed separately with tools/sign_release.sh.
 // No private signing material is loaded by the public build or CI.
 buildTypes { release { isMinifyEnabled = false; isDebuggable = false } }
}
dependencies {
 implementation(platform("androidx.compose:compose-bom:2024.12.01"))
 implementation("androidx.activity:activity-compose:1.9.3")
 implementation("androidx.compose.material3:material3")
 implementation("androidx.compose.ui:ui")
 implementation("androidx.compose.foundation:foundation")
 implementation("androidx.core:core-ktx:1.15.0")
}
