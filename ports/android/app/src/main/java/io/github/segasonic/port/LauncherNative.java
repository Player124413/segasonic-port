package io.github.segasonic.port;

/**
 * JNI bridge to libsegasonic_launcher.so for MAME sonic.zip verification,
 * segasonic.assets extraction, and character icon decoding in the Launcher process.
 */
public final class LauncherNative {
    private static boolean sLoaded = false;
    private static Throwable sLoadError = null;

    static {
        try {
            System.loadLibrary("segasonic_launcher");
            sLoaded = true;
        } catch (Throwable t) {
            sLoadError = t;
        }
    }

    private LauncherNative() {}

    public static boolean isLibraryLoaded() {
        return sLoaded;
    }

    public static String getLoadError() {
        return sLoadError != null ? sLoadError.toString() : "";
    }

    public static native boolean nativeHasValidAssets(String assetsPath);

    /** Returns null if source (.zip or folder) is a valid MAME sonic Rev. C set, else error message. */
    public static native String nativeVerifyRomSource(String sourcePath);

    /** Extracts segasonic.assets from sourcePath into destPackPath; returns null on success. */
    public static native String nativeExtractAssets(String sourcePath, String destPackPath);

    /** Extracts 64x64 ARGB8888 Sonic/Mighty/Ray portrait icon from segasonic.assets, or null. */
    public static native int[] nativeExtractIconPixels(String assetsPath);
}
