# Preserve all SDL2 JNI entry points and callbacks
-keep class org.libsdl.app.** { *; }
-keep class io.github.segasonic.port.LauncherNative { *; }
-keep class io.github.segasonic.port.GameActivity { *; }
-dontwarn org.libsdl.app.**
