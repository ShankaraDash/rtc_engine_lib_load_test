

apply plugin: 'com.android.library'

    plugins {
    id("com.android.library")
    id("org.jetbrains.kotlin.android")
}

android {
    compileSdkVersion(28)
    ndkVersion = rootProject.extra.get("tmg.ndk.version") as String

    defaultConfig {
        minSdkVersion(16)
        targetSdkVersion(28)
        versionCode = 1
        versionName = project.version.toString()

        externalNativeBuild {
            ndkBuild {
                arguments.add("NDK_APPLICATION_MK:=src/main/cpp/Application.mk")
            }
        }

        ndk {
            abiFilters("armeabi-v7a", "x86", "arm64-v8a", "x86_64")
        }
    }

    buildTypes {
        getByName("release") {
            isMinifyEnabled = false
            proguardFiles(getDefaultProguardFile("proguard-android.txt"))
            consumerProguardFiles("proguard-rules.pro")
        }
    }

    externalNativeBuild {
        ndkBuild {
            setPath("src/main/cpp/Android.mk")
        }
    }

    lintOptions {
        isAbortOnError = false
    }

    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_1_8
        targetCompatibility = JavaVersion.VERSION_1_8
    }
}

dependencies {
    implementation("org.jetbrains.kotlin:kotlin-stdlib-jdk7")
}

apply(from = rootProject.file("gradle/publishing.gradle"))
apply(from = rootProject.file("gradle/javadoc.gradle"))
