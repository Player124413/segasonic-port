package io.github.segasonic.port;

import android.content.Context;
import android.content.Intent;
import android.os.Build;
import android.os.Bundle;
import android.os.SystemClock;
import android.os.VibrationEffect;
import android.os.Vibrator;
import android.os.VibratorManager;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;

import org.libsdl.app.SDLActivity;

/**
 * Isolated (:game process) SDL2 + OpenGL ES 3.0/2.0 host activity.
 * Dynamically loads either libsegasonic_console.so or libsegasonic_arcade.so
 * and terminates its isolated process cleanly on exit so C static/BSS state
 * is 100% fresh on every launch.
 */
public class GameActivity extends SDLActivity {
    public static final String EXTRA_EDITION = "edition";
    public static final String EXTRA_TOUCH_EDITOR_ONLY = "touch_editor_only";
    public static final String EDITION_CONSOLE = "console";
    public static final String EDITION_ARCADE = "arcade";

    private static volatile boolean sArcadeEdition = false;
    private static volatile boolean sTouchEditorOnly = false;
    private static volatile Vibrator sVibrator = null;
    private static volatile long sLastTickMs = 0L;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        // Must read Intent extras BEFORE super.onCreate(), because SDLActivity.onCreate()
        // immediately invokes loadLibraries() -> getLibraries()!
        Intent intent = getIntent();
        if (intent != null) {
            String ed = intent.getStringExtra(EXTRA_EDITION);
            sArcadeEdition = EDITION_ARCADE.equalsIgnoreCase(ed);
            sTouchEditorOnly = intent.getBooleanExtra(EXTRA_TOUCH_EDITOR_ONLY, false);
        } else {
            sArcadeEdition = false;
            sTouchEditorOnly = false;
        }

        initVibrator();
        configureFullscreenWindow();
        super.onCreate(savedInstanceState);
        applyImmersiveSticky();
    }

    @Override
    protected String[] getLibraries() {
        return new String[]{
            "SDL2",
            sArcadeEdition ? "segasonic_arcade" : "segasonic_console"
        };
    }

    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
        super.onWindowFocusChanged(hasFocus);
        if (hasFocus) {
            applyImmersiveSticky();
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        // Cleanly exit the isolated :game process so subsequent Console/Arcade launches
        // from LauncherActivity start with zero leftover C globals.
        System.exit(0);
    }

    private void initVibrator() {
        try {
            if (Build.VERSION.SDK_INT >= 31) {
                VibratorManager vm = (VibratorManager) getSystemService(Context.VIBRATOR_MANAGER_SERVICE);
                if (vm != null) {
                    sVibrator = vm.getDefaultVibrator();
                }
            } else {
                @SuppressWarnings("deprecation")
                Vibrator vib = (Vibrator) getSystemService(Context.VIBRATOR_SERVICE);
                sVibrator = vib;
            }
        } catch (Throwable ignored) {
            sVibrator = null;
        }
    }

    private void configureFullscreenWindow() {
        Window window = getWindow();
        if (window == null) {
            return;
        }
        window.addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        if (Build.VERSION.SDK_INT >= 28) {
            WindowManager.LayoutParams lp = window.getAttributes();
            lp.layoutInDisplayCutoutMode =
                WindowManager.LayoutParams.LAYOUT_IN_DISPLAY_CUTOUT_MODE_SHORT_EDGES;
            window.setAttributes(lp);
        }
    }

    @SuppressWarnings("deprecation")
    private void applyImmersiveSticky() {
        Window window = getWindow();
        if (window == null) {
            return;
        }
        View decor = window.getDecorView();
        if (decor == null) {
            return;
        }
        decor.setSystemUiVisibility(
            View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY
                | View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                | View.SYSTEM_UI_FLAG_FULLSCREEN
        );
    }

    /** Called from C (android_main.c) via JNI when touch controls emit haptic events. */
    public static boolean isLaunchTouchEditorOnly() {
        return sTouchEditorOnly;
    }

    /** Called from C (android_main.c) via JNI: 1=press, 2=UI, 3=release, 4=trackball bearing tick. */
    @SuppressWarnings("deprecation")
    public static void vibratePulse(int kind) {
        Vibrator vib = sVibrator;
        if (vib == null || !vib.hasVibrator()) {
            return;
        }

        long now = SystemClock.uptimeMillis();
        if (kind == 4) {
            // Subtle rolling tick: only fire on API 26+ amplitude-controlled motors, max ~25 Hz
            if (Build.VERSION.SDK_INT < 26 || !vib.hasAmplitudeControl() || (now - sLastTickMs) < 38L) {
                return;
            }
            sLastTickMs = now;
        }

        try {
            if (Build.VERSION.SDK_INT >= 26) {
                long ms;
                int amp;
                switch (kind) {
                    case 1:  ms = 11L; amp = 145; break;
                    case 2:  ms = 8L;  amp = 115; break;
                    case 3:  ms = 5L;  amp = 65;  break;
                    case 4:  ms = 4L;  amp = 42;  break;
                    default: return;
                }
                if (!vib.hasAmplitudeControl()) {
                    amp = VibrationEffect.DEFAULT_AMPLITUDE;
                }
                vib.vibrate(VibrationEffect.createOneShot(ms, amp));
            } else if (kind == 1 || kind == 2) {
                vib.vibrate(kind == 1 ? 10L : 7L);
            }
        } catch (Throwable ignored) {
        }
    }
}
