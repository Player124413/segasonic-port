package io.github.segasonic.port;

import android.app.Activity;
import android.app.ActivityManager;
import android.app.AlertDialog;
import android.content.Context;
import android.content.Intent;
import android.content.pm.ConfigurationInfo;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.LinearGradient;
import android.graphics.Paint;
import android.graphics.Rect;
import android.graphics.Shader;
import android.graphics.Typeface;
import android.graphics.drawable.GradientDrawable;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.util.TypedValue;
import android.view.Gravity;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.ProgressBar;
import android.widget.ScrollView;
import android.widget.SeekBar;
import android.widget.Spinner;
import android.widget.Switch;
import android.widget.TextView;
import android.widget.Toast;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.nio.charset.StandardCharsets;
import java.util.LinkedHashMap;
import java.util.Locale;
import java.util.Map;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

/**
 * Native SegaSonic Launcher, MAME sonic.zip verifier/extractor, hardware capability
 * inspector, and unified Touch / Graphics / ETC2 / Audio configuration hub.
 */
public class LauncherActivity extends Activity {
    private static final int REQ_OPEN_ROM_ZIP = 1001;

    private static final int COL_BG = 0xFF0A0F1E;
    private static final int COL_CARD = 0xFF131D36;
    private static final int COL_CARD_BORDER = 0xFF263B6A;
    private static final int COL_ACCENT_BLUE = 0xFF1E78FF;
    private static final int COL_ACCENT_GOLD = 0xFFFFD13B;
    private static final int COL_ACCENT_GREEN = 0xFF2BD97C;
    private static final int COL_ACCENT_SLATE = 0xFF223357;
    private static final int COL_TEXT_PRIMARY = 0xFFF3F7FF;
    private static final int COL_TEXT_MUTED = 0xFF9CB2D9;

    private final ExecutorService mIoExecutor = Executors.newSingleThreadExecutor();
    private final Handler mMainHandler = new Handler(Looper.getMainLooper());

    private boolean mSuppressListeners = false;
    private boolean mExtracting = false;

    // Header & ROM widgets
    private ImageView mHeroIconView;
    private TextView mRomStatusText;
    private ProgressBar mRomProgress;
    private Button mImportRomBtn;
    private Button mPlayConsoleBtn;
    private Button mPlayArcadeBtn;
    private Button mEditTouchBtn;

    // Touch controls widgets
    private Spinner mMoveModeSpinner;
    private Spinner mThemeColorSpinner;
    private TextView mSensitivityLabel;
    private SeekBar mSensitivitySeek;
    private TextView mInertiaLabel;
    private SeekBar mInertiaSeek;
    private TextView mOpacityLabel;
    private SeekBar mOpacitySeek;
    private Switch mFloatingOriginSwitch;
    private Switch mHapticsSwitch;
    private Switch mAutoHidePadSwitch;

    // Video / ETC2 / Audio widgets
    private Spinner mFilterSpinner;
    private Spinner mAspectSpinner;
    private Spinner mEtc2Spinner;
    private Switch mWidescreenSwitch;
    private Switch mFreePlaySwitch;
    private TextView mVolumeLabel;
    private SeekBar mVolumeSeek;
    private Switch mLowLatencySwitch;
    private Switch mVsyncSwitch;
    private Switch mShowFpsSwitch;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        buildUi();
        refreshAllState();
    }

    @Override
    protected void onResume() {
        super.onResume();
        refreshAllState();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        mIoExecutor.shutdownNow();
    }

    private File getAssetsPackFile() {
        return new File(getFilesDir(), "segasonic.assets");
    }

    private File getAndroidPortIniFile() {
        return new File(getFilesDir(), "android_port.ini");
    }

    private File getTouchControlsIniFile() {
        return new File(getFilesDir(), "touch_controls.ini");
    }

    private File getConsoleIniFile() {
        return new File(getFilesDir(), "segasonic.ini");
    }

    private File getArcadeIniFile() {
        return new File(getFilesDir(), "sonic.ini");
    }

    private int dp(int v) {
        return Math.round(TypedValue.applyDimension(
            TypedValue.COMPLEX_UNIT_DIP, v, getResources().getDisplayMetrics()));
    }

    private GradientDrawable roundedBg(int fillColor, int strokeColor, int radiusDp, int strokeDp) {
        GradientDrawable gd = new GradientDrawable();
        gd.setColor(fillColor);
        gd.setCornerRadius(dp(radiusDp));
        if (strokeDp > 0) {
            gd.setStroke(dp(strokeDp), strokeColor);
        }
        return gd;
    }

    private String queryHardwareBadgeText() {
        String abi = (Build.SUPPORTED_ABIS != null && Build.SUPPORTED_ABIS.length > 0)
            ? Build.SUPPORTED_ABIS[0] : Build.CPU_ABI;
        boolean is64 = abi != null && (abi.contains("64"));
        int glesVer = 0x00020000;
        try {
            ActivityManager am = (ActivityManager) getSystemService(Context.ACTIVITY_SERVICE);
            if (am != null) {
                ConfigurationInfo ci = am.getDeviceConfigurationInfo();
                if (ci != null && ci.reqGlEsVersion > 0) {
                    glesVer = ci.reqGlEsVersion;
                }
            }
        } catch (Throwable ignored) {
        }
        int major = (glesVer >> 16) & 0xFFFF;
        int minor = glesVer & 0xFFFF;
        String etcCap = (major >= 3) ? "HW ETC2/EAC Ready" : "GLES2 RGB565 Mode";
        return String.format(Locale.US, "%s (%s) • OpenGL ES %d.%d • %s",
            abi, is64 ? "64-bit" : "32-bit", major, minor, etcCap);
    }

    private Bitmap createFallbackTrackballEmblem() {
        int sz = 192;
        Bitmap bmp = Bitmap.createBitmap(sz, sz, Bitmap.Config.ARGB_8888);
        Canvas c = new Canvas(bmp);
        Paint p = new Paint(Paint.ANTI_ALIAS_FLAG);

        p.setColor(0xFF17264A);
        c.drawCircle(sz * 0.5f, sz * 0.5f, sz * 0.46f, p);

        p.setStyle(Paint.Style.STROKE);
        p.setStrokeWidth(8f);
        p.setColor(COL_ACCENT_GOLD);
        c.drawCircle(sz * 0.5f, sz * 0.5f, sz * 0.44f, p);

        p.setStyle(Paint.Style.FILL);
        p.setShader(new LinearGradient(
            sz * 0.25f, sz * 0.20f, sz * 0.78f, sz * 0.85f,
            0xFF56B4FF, 0xFF0B46B8, Shader.TileMode.CLAMP));
        c.drawCircle(sz * 0.5f, sz * 0.5f, sz * 0.34f, p);
        p.setShader(null);

        p.setColor(0x99FFFFFF);
        c.drawCircle(sz * 0.40f, sz * 0.38f, sz * 0.09f, p);
        return bmp;
    }

    private Bitmap scaleNearest(int[] pixels64, int factor) {
        Bitmap src = Bitmap.createBitmap(pixels64, 64, 64, Bitmap.Config.ARGB_8888);
        int outSz = 64 * factor;
        Bitmap dst = Bitmap.createBitmap(outSz, outSz, Bitmap.Config.ARGB_8888);
        Canvas canvas = new Canvas(dst);
        Paint paint = new Paint();
        paint.setFilterBitmap(false);
        paint.setAntiAlias(false);
        canvas.drawBitmap(src, new Rect(0, 0, 64, 64), new Rect(0, 0, outSz, outSz), paint);
        src.recycle();
        return dst;
    }

    private void buildUi() {
        ScrollView scroll = new ScrollView(this);
        scroll.setFillViewport(true);
        scroll.setBackgroundColor(COL_BG);

        LinearLayout outer = new LinearLayout(this);
        outer.setOrientation(LinearLayout.VERTICAL);
        outer.setGravity(Gravity.CENTER_HORIZONTAL);
        int padH = dp(16);
        int padV = dp(16);
        outer.setPadding(padH, padV, padH, dp(28));

        LinearLayout container = new LinearLayout(this);
        container.setOrientation(LinearLayout.VERTICAL);
        int maxWidthPx = dp(680);
        int screenW = getResources().getDisplayMetrics().widthPixels;
        int targetW = (screenW > maxWidthPx + dp(32)) ? maxWidthPx : ViewGroup.LayoutParams.MATCH_PARENT;
        LinearLayout.LayoutParams containerLp = new LinearLayout.LayoutParams(
            targetW, ViewGroup.LayoutParams.WRAP_CONTENT);
        outer.addView(container, containerLp);

        // 1. Hero Header Card
        container.addView(buildHeroCard(), cardLayoutParams());

        // 2. ROM Assets Status & Import Card
        container.addView(buildRomCard(), cardLayoutParams());

        // 3. Launch Edition & Live Touch Editor Card
        container.addView(buildLaunchCard(), cardLayoutParams());

        // 4. Touch Controls & Trackball Physics Card
        container.addView(buildTouchSettingsCard(), cardLayoutParams());

        // 5. Graphics, Hardware ETC2 & Audio Optimization Card
        container.addView(buildVideoAudioCard(), cardLayoutParams());

        scroll.addView(outer);
        setContentView(scroll);
    }

    private LinearLayout.LayoutParams cardLayoutParams() {
        LinearLayout.LayoutParams lp = new LinearLayout.LayoutParams(
            ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.WRAP_CONTENT);
        lp.bottomMargin = dp(14);
        return lp;
    }

    private LinearLayout createCard(String sectionTitle) {
        LinearLayout card = new LinearLayout(this);
        card.setOrientation(LinearLayout.VERTICAL);
        card.setBackground(roundedBg(COL_CARD, COL_CARD_BORDER, 14, 1));
        int p = dp(16);
        card.setPadding(p, p, p, p);

        if (sectionTitle != null) {
            TextView hdr = new TextView(this);
            hdr.setText(sectionTitle);
            hdr.setTextColor(COL_ACCENT_GOLD);
            hdr.setTextSize(TypedValue.COMPLEX_UNIT_SP, 12.5f);
            hdr.setTypeface(Typeface.DEFAULT_BOLD);
            hdr.setLetterSpacing(0.06f);
            LinearLayout.LayoutParams lp = new LinearLayout.LayoutParams(
                ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.WRAP_CONTENT);
            lp.bottomMargin = dp(10);
            card.addView(hdr, lp);
        }
        return card;
    }

    private View buildHeroCard() {
        LinearLayout card = createCard(null);
        LinearLayout row = new LinearLayout(this);
        row.setOrientation(LinearLayout.HORIZONTAL);
        row.setGravity(Gravity.CENTER_VERTICAL);

        mHeroIconView = new ImageView(this);
        mHeroIconView.setScaleType(ImageView.ScaleType.FIT_CENTER);
        mHeroIconView.setImageBitmap(createFallbackTrackballEmblem());
        LinearLayout.LayoutParams iconLp = new LinearLayout.LayoutParams(dp(72), dp(72));
        iconLp.rightMargin = dp(14);
        row.addView(mHeroIconView, iconLp);

        LinearLayout titles = new LinearLayout(this);
        titles.setOrientation(LinearLayout.VERTICAL);

        TextView title = new TextView(this);
        title.setText("SEGASONIC THE HEDGEHOG");
        title.setTextColor(COL_TEXT_PRIMARY);
        title.setTextSize(TypedValue.COMPLEX_UNIT_SP, 19f);
        title.setTypeface(Typeface.DEFAULT_BOLD);
        titles.addView(title);

        TextView sub = new TextView(this);
        sub.setText(R.string.launcher_subtitle);
        sub.setTextColor(COL_ACCENT_GOLD);
        sub.setTextSize(TypedValue.COMPLEX_UNIT_SP, 13f);
        titles.addView(sub);

        TextView hw = new TextView(this);
        hw.setText(queryHardwareBadgeText());
        hw.setTextColor(COL_TEXT_MUTED);
        hw.setTextSize(TypedValue.COMPLEX_UNIT_SP, 11.5f);
        LinearLayout.LayoutParams hwLp = new LinearLayout.LayoutParams(
            ViewGroup.LayoutParams.WRAP_CONTENT, ViewGroup.LayoutParams.WRAP_CONTENT);
        hwLp.topMargin = dp(4);
        titles.addView(hw, hwLp);

        row.addView(titles, new LinearLayout.LayoutParams(
            0, ViewGroup.LayoutParams.WRAP_CONTENT, 1f));
        card.addView(row);
        return card;
    }

    private View buildRomCard() {
        LinearLayout card = createCard(getString(R.string.rom_section_title));

        mRomStatusText = new TextView(this);
        mRomStatusText.setTextColor(COL_TEXT_PRIMARY);
        mRomStatusText.setTextSize(TypedValue.COMPLEX_UNIT_SP, 13.5f);
        LinearLayout.LayoutParams stLp = new LinearLayout.LayoutParams(
            ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.WRAP_CONTENT);
        stLp.bottomMargin = dp(10);
        card.addView(mRomStatusText, stLp);

        mRomProgress = new ProgressBar(this, null, android.R.attr.progressBarStyleHorizontal);
        mRomProgress.setIndeterminate(true);
        mRomProgress.setVisibility(View.GONE);
        LinearLayout.LayoutParams prLp = new LinearLayout.LayoutParams(
            ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.WRAP_CONTENT);
        prLp.bottomMargin = dp(10);
        card.addView(mRomProgress, prLp);

        mImportRomBtn = createStyledButton(getString(R.string.btn_import_rom), COL_ACCENT_SLATE, COL_ACCENT_BLUE);
        mImportRomBtn.setOnClickListener(v -> openRomFilePicker());
        card.addView(mImportRomBtn);

        return card;
    }

    private View buildLaunchCard() {
        LinearLayout card = createCard(getString(R.string.launch_section_title));

        mPlayConsoleBtn = createStyledButton(
            getString(R.string.btn_play_console), COL_ACCENT_BLUE, COL_ACCENT_GOLD);
        mPlayConsoleBtn.setOnClickListener(v -> launchGame(GameActivity.EDITION_CONSOLE, false));
        LinearLayout.LayoutParams lp1 = new LinearLayout.LayoutParams(
            ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.WRAP_CONTENT);
        lp1.bottomMargin = dp(10);
        card.addView(mPlayConsoleBtn, lp1);

        mPlayArcadeBtn = createStyledButton(
            getString(R.string.btn_play_arcade), COL_ACCENT_SLATE, COL_CARD_BORDER);
        mPlayArcadeBtn.setOnClickListener(v -> launchGame(GameActivity.EDITION_ARCADE, false));
        LinearLayout.LayoutParams lp2 = new LinearLayout.LayoutParams(
            ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.WRAP_CONTENT);
        lp2.bottomMargin = dp(10);
        card.addView(mPlayArcadeBtn, lp2);

        mEditTouchBtn = createStyledButton(
            getString(R.string.btn_edit_touch), 0xFF1A3A3A, COL_ACCENT_GREEN);
        mEditTouchBtn.setOnClickListener(v -> launchGame(GameActivity.EDITION_CONSOLE, true));
        card.addView(mEditTouchBtn);

        return card;
    }

    private View buildTouchSettingsCard() {
        LinearLayout card = createCard(getString(R.string.touch_section_title));

        mMoveModeSpinner = addLabeledSpinner(card, getString(R.string.lbl_move_mode), new String[]{
            getString(R.string.mode_trackball),
            getString(R.string.mode_stick),
            getString(R.string.mode_dpad)
        });

        mThemeColorSpinner = addLabeledSpinner(card, getString(R.string.lbl_theme_color), new String[]{
            getString(R.string.color_sonic),
            getString(R.string.color_mighty),
            getString(R.string.color_ray)
        });

        mSensitivityLabel = addSeekLabel(card);
        mSensitivitySeek = addSeekBar(card, 275, progress -> {
            int pct = 25 + progress;
            mSensitivityLabel.setText(getString(R.string.lbl_sensitivity, pct));
            if (!mSuppressListeners) saveAllConfigs();
        });

        mInertiaLabel = addSeekLabel(card);
        mInertiaSeek = addSeekBar(card, 17, progress -> {
            int pct = 80 + progress;
            mInertiaLabel.setText(getString(R.string.lbl_inertia, pct));
            if (!mSuppressListeners) saveAllConfigs();
        });

        mOpacityLabel = addSeekLabel(card);
        mOpacitySeek = addSeekBar(card, 80, progress -> {
            int pct = 20 + progress;
            mOpacityLabel.setText(getString(R.string.lbl_hud_opacity, pct));
            if (!mSuppressListeners) saveAllConfigs();
        });

        mFloatingOriginSwitch = addStyledSwitch(card, getString(R.string.sw_floating_origin));
        mHapticsSwitch = addStyledSwitch(card, getString(R.string.sw_haptics));
        mAutoHidePadSwitch = addStyledSwitch(card, getString(R.string.sw_autohide_pad));

        Button resetBtn = createStyledButton(
            getString(R.string.btn_reset_touch), 0xFF2B1E2E, 0xFF8C3B5A);
        LinearLayout.LayoutParams rlp = new LinearLayout.LayoutParams(
            ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.WRAP_CONTENT);
        rlp.topMargin = dp(8);
        resetBtn.setOnClickListener(v -> {
            File f = getTouchControlsIniFile();
            if (f.exists()) {
                //noinspection ResultOfMethodCallIgnored
                f.delete();
            }
            loadAllConfigsIntoUi();
            Toast.makeText(this, R.string.touch_reset_done, Toast.LENGTH_SHORT).show();
        });
        card.addView(resetBtn, rlp);

        return card;
    }

    private View buildVideoAudioCard() {
        LinearLayout card = createCard(getString(R.string.video_section_title));

        mFilterSpinner = addLabeledSpinner(card, getString(R.string.lbl_shader_filter), new String[]{
            getString(R.string.filter_sharp_aa),
            getString(R.string.filter_nearest),
            getString(R.string.filter_smooth),
            getString(R.string.filter_crt)
        });

        mAspectSpinner = addLabeledSpinner(card, getString(R.string.lbl_aspect), new String[]{
            getString(R.string.aspect_auto),
            getString(R.string.aspect_4_3),
            getString(R.string.aspect_fill)
        });

        mEtc2Spinner = addLabeledSpinner(card, getString(R.string.lbl_etc2_mode), new String[]{
            getString(R.string.etc2_auto),
            getString(R.string.etc2_stream),
            getString(R.string.etc2_off)
        });

        mWidescreenSwitch = addStyledSwitch(card, getString(R.string.sw_widescreen));
        mFreePlaySwitch = addStyledSwitch(card, getString(R.string.sw_freeplay));

        mVolumeLabel = addSeekLabel(card);
        mVolumeSeek = addSeekBar(card, 100, progress -> {
            mVolumeLabel.setText(getString(R.string.lbl_volume, progress));
            if (!mSuppressListeners) saveAllConfigs();
        });

        mLowLatencySwitch = addStyledSwitch(card, getString(R.string.sw_low_latency));
        mVsyncSwitch = addStyledSwitch(card, getString(R.string.sw_vsync));
        mShowFpsSwitch = addStyledSwitch(card, getString(R.string.sw_show_fps));

        return card;
    }

    private Button createStyledButton(String label, int fillCol, int borderCol) {
        Button b = new Button(this);
        b.setAllCaps(false);
        b.setText(label);
        b.setTextColor(COL_TEXT_PRIMARY);
        b.setTextSize(TypedValue.COMPLEX_UNIT_SP, 14.5f);
        b.setTypeface(Typeface.DEFAULT_BOLD);
        b.setBackground(roundedBg(fillCol, borderCol, 10, 1));
        b.setPadding(dp(14), dp(12), dp(14), dp(12));
        return b;
    }

    private Spinner addLabeledSpinner(LinearLayout parent, String label, String[] items) {
        TextView tv = new TextView(this);
        tv.setText(label);
        tv.setTextColor(COL_TEXT_MUTED);
        tv.setTextSize(TypedValue.COMPLEX_UNIT_SP, 12.5f);
        LinearLayout.LayoutParams tlp = new LinearLayout.LayoutParams(
            ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.WRAP_CONTENT);
        tlp.bottomMargin = dp(4);
        parent.addView(tv, tlp);

        Spinner sp = new Spinner(this);
        sp.setBackground(roundedBg(0xFF0E172C, COL_CARD_BORDER, 8, 1));
        sp.setPadding(dp(10), dp(6), dp(10), dp(6));

        ArrayAdapter<String> adapter = new ArrayAdapter<String>(
            this, android.R.layout.simple_spinner_item, items) {
            @Override
            public View getView(int position, View convertView, ViewGroup parentGroup) {
                View v = super.getView(position, convertView, parentGroup);
                if (v instanceof TextView) {
                    ((TextView) v).setTextColor(COL_TEXT_PRIMARY);
                    ((TextView) v).setTextSize(TypedValue.COMPLEX_UNIT_SP, 13.5f);
                }
                return v;
            }

            @Override
            public View getDropDownView(int position, View convertView, ViewGroup parentGroup) {
                View v = super.getDropDownView(position, convertView, parentGroup);
                v.setBackgroundColor(0xFF152242);
                v.setPadding(dp(12), dp(10), dp(12), dp(10));
                if (v instanceof TextView) {
                    ((TextView) v).setTextColor(COL_TEXT_PRIMARY);
                    ((TextView) v).setTextSize(TypedValue.COMPLEX_UNIT_SP, 13.5f);
                }
                return v;
            }
        };
        sp.setAdapter(adapter);
        sp.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> parentView, View selectedItemView, int pos, long id) {
                if (!mSuppressListeners) {
                    saveAllConfigs();
                }
            }

            @Override
            public void onNothingSelected(AdapterView<?> parentView) {}
        });

        LinearLayout.LayoutParams slp = new LinearLayout.LayoutParams(
            ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.WRAP_CONTENT);
        slp.bottomMargin = dp(12);
        parent.addView(sp, slp);
        return sp;
    }

    private interface SeekCallback {
        void onProgress(int progress);
    }

    private TextView addSeekLabel(LinearLayout parent) {
        TextView tv = new TextView(this);
        tv.setTextColor(COL_TEXT_PRIMARY);
        tv.setTextSize(TypedValue.COMPLEX_UNIT_SP, 13f);
        parent.addView(tv);
        return tv;
    }

    private SeekBar addSeekBar(LinearLayout parent, int max, final SeekCallback cb) {
        SeekBar sb = new SeekBar(this);
        sb.setMax(max);
        sb.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                cb.onProgress(progress);
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {}

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
                if (!mSuppressListeners) {
                    saveAllConfigs();
                }
            }
        });
        LinearLayout.LayoutParams lp = new LinearLayout.LayoutParams(
            ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.WRAP_CONTENT);
        lp.bottomMargin = dp(10);
        parent.addView(sb, lp);
        return sb;
    }

    private Switch addStyledSwitch(LinearLayout parent, String text) {
        Switch sw = new Switch(this);
        sw.setText(text);
        sw.setTextColor(COL_TEXT_PRIMARY);
        sw.setTextSize(TypedValue.COMPLEX_UNIT_SP, 13.5f);
        sw.setPadding(0, dp(6), 0, dp(6));
        sw.setOnCheckedChangeListener((buttonView, isChecked) -> {
            if (!mSuppressListeners) {
                saveAllConfigs();
            }
        });
        LinearLayout.LayoutParams lp = new LinearLayout.LayoutParams(
            ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.WRAP_CONTENT);
        lp.bottomMargin = dp(4);
        parent.addView(sw, lp);
        return sw;
    }

    private void refreshAllState() {
        loadAllConfigsIntoUi();
        updateAssetsStatusUi();
    }

    private void updateAssetsStatusUi() {
        File pack = getAssetsPackFile();
        boolean valid = LauncherNative.isLibraryLoaded()
            && LauncherNative.nativeHasValidAssets(pack.getAbsolutePath());

        if (valid) {
            double mb = pack.length() / (1024.0 * 1024.0);
            mRomStatusText.setText(getString(R.string.rom_status_ready,
                String.format(Locale.US, "%.2f", mb)));
            mRomStatusText.setTextColor(COL_ACCENT_GREEN);
            mImportRomBtn.setText(R.string.btn_reimport_rom);

            int[] iconPx = LauncherNative.nativeExtractIconPixels(pack.getAbsolutePath());
            if (iconPx != null && iconPx.length == 64 * 64) {
                mHeroIconView.setImageBitmap(scaleNearest(iconPx, 3));
            }
        } else {
            mRomStatusText.setText(R.string.rom_status_missing);
            mRomStatusText.setTextColor(COL_ACCENT_GOLD);
            mImportRomBtn.setText(R.string.btn_import_rom);
        }

        boolean canLaunch = valid && !mExtracting;
        mPlayConsoleBtn.setEnabled(canLaunch);
        mPlayConsoleBtn.setAlpha(canLaunch ? 1.0f : 0.45f);
        mPlayArcadeBtn.setEnabled(canLaunch);
        mPlayArcadeBtn.setAlpha(canLaunch ? 1.0f : 0.45f);
        mEditTouchBtn.setEnabled(canLaunch);
        mEditTouchBtn.setAlpha(canLaunch ? 1.0f : 0.45f);
        mImportRomBtn.setEnabled(!mExtracting);
    }

    private void openRomFilePicker() {
        if (mExtracting) {
            return;
        }
        try {
            Intent intent = new Intent(Intent.ACTION_OPEN_DOCUMENT);
            intent.addCategory(Intent.CATEGORY_OPENABLE);
            intent.setType("*/*");
            intent.putExtra(Intent.EXTRA_MIME_TYPES, new String[]{
                "application/zip",
                "application/x-zip-compressed",
                "application/octet-stream"
            });
            startActivityForResult(intent, REQ_OPEN_ROM_ZIP);
        } catch (Throwable t) {
            Intent fallback = new Intent(Intent.ACTION_GET_CONTENT);
            fallback.addCategory(Intent.CATEGORY_OPENABLE);
            fallback.setType("*/*");
            startActivityForResult(fallback, REQ_OPEN_ROM_ZIP);
        }
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (requestCode == REQ_OPEN_ROM_ZIP && resultCode == RESULT_OK && data != null) {
            Uri uri = data.getData();
            if (uri != null) {
                importRomFromUri(uri);
            }
        }
    }

    private void importRomFromUri(final Uri uri) {
        if (!LauncherNative.isLibraryLoaded()) {
            showErrorDialog("Native launcher library error: " + LauncherNative.getLoadError());
            return;
        }

        mExtracting = true;
        mRomProgress.setVisibility(View.VISIBLE);
        mRomStatusText.setText(R.string.rom_extracting);
        mRomStatusText.setTextColor(COL_ACCENT_GOLD);
        updateAssetsStatusUi();

        final File tempZip = new File(getCacheDir(), "import_sonic_rom.zip");
        final File destPack = getAssetsPackFile();
        final File tempPack = new File(getFilesDir(), "segasonic.assets.tmp");

        mIoExecutor.execute(() -> {
            String errMsg = null;
            try {
                try (InputStream in = getContentResolver().openInputStream(uri);
                     FileOutputStream out = new FileOutputStream(tempZip)) {
                    if (in == null) {
                        throw new IllegalStateException("Cannot open selected file stream.");
                    }
                    byte[] buf = new byte[65536];
                    int n;
                    while ((n = in.read(buf)) > 0) {
                        out.write(buf, 0, n);
                    }
                }

                errMsg = LauncherNative.nativeVerifyRomSource(tempZip.getAbsolutePath());
                if (errMsg == null) {
                    errMsg = LauncherNative.nativeExtractAssets(
                        tempZip.getAbsolutePath(), tempPack.getAbsolutePath());
                    if (errMsg == null) {
                        if (destPack.exists()) {
                            //noinspection ResultOfMethodCallIgnored
                            destPack.delete();
                        }
                        if (!tempPack.renameTo(destPack)) {
                            errMsg = "Failed to finalize segasonic.assets in app storage.";
                        }
                    }
                }
            } catch (Throwable t) {
                errMsg = t.getMessage() != null ? t.getMessage() : t.toString();
            } finally {
                // Never retain user's raw sonic.zip copy in cache after extraction
                if (tempZip.exists()) {
                    //noinspection ResultOfMethodCallIgnored
                    tempZip.delete();
                }
                if (tempPack.exists()) {
                    //noinspection ResultOfMethodCallIgnored
                    tempPack.delete();
                }
            }

            final String finalErr = errMsg;
            mMainHandler.post(() -> {
                mExtracting = false;
                mRomProgress.setVisibility(View.GONE);
                updateAssetsStatusUi();
                if (finalErr == null) {
                    Toast.makeText(LauncherActivity.this,
                        R.string.rom_import_ok, Toast.LENGTH_LONG).show();
                } else {
                    showErrorDialog(getString(R.string.rom_import_failed, finalErr));
                }
            });
        });
    }

    private void showErrorDialog(String message) {
        if (isFinishing()) {
            return;
        }
        new AlertDialog.Builder(this)
            .setTitle(R.string.app_name)
            .setMessage(message)
            .setPositiveButton(android.R.string.ok, null)
            .show();
    }

    private void launchGame(String edition, boolean touchEditorOnly) {
        saveAllConfigs();
        File pack = getAssetsPackFile();
        if (!LauncherNative.isLibraryLoaded()
                || !LauncherNative.nativeHasValidAssets(pack.getAbsolutePath())) {
            openRomFilePicker();
            return;
        }

        Intent intent = new Intent(this, GameActivity.class);
        intent.putExtra(GameActivity.EXTRA_EDITION, edition);
        intent.putExtra(GameActivity.EXTRA_TOUCH_EDITOR_ONLY, touchEditorOnly);
        startActivity(intent);
    }

    // ---- INI readers/mergers (preserve custom button coords, sections & keybinds!) ----

    private static LinkedHashMap<String, String> readFlatIniMap(File file) {
        LinkedHashMap<String, String> map = new LinkedHashMap<>();
        if (!file.exists()) {
            return map;
        }
        try (BufferedReader br = new BufferedReader(
                new InputStreamReader(new FileInputStream(file), StandardCharsets.UTF_8))) {
            String line;
            while ((line = br.readLine()) != null) {
                String trimmed = line.trim();
                if (trimmed.isEmpty() || trimmed.startsWith("#") || trimmed.startsWith(";")) {
                    continue;
                }
                int eq = trimmed.indexOf('=');
                if (eq > 0) {
                    String k = trimmed.substring(0, eq).trim();
                    String v = trimmed.substring(eq + 1).trim();
                    map.put(k, v);
                }
            }
        } catch (Throwable ignored) {
        }
        return map;
    }

    private static void writeFlatIniMap(File file, String headerComment, Map<String, String> map) {
        File tmp = new File(file.getParentFile(), file.getName() + ".tmp");
        try (BufferedWriter bw = new BufferedWriter(
                new OutputStreamWriter(new FileOutputStream(tmp), StandardCharsets.UTF_8))) {
            if (headerComment != null) {
                bw.write("# " + headerComment + "\n");
            }
            for (Map.Entry<String, String> e : map.entrySet()) {
                bw.write(e.getKey() + " = " + e.getValue() + "\n");
            }
        } catch (Throwable ignored) {
            return;
        }
        if (file.exists()) {
            //noinspection ResultOfMethodCallIgnored
            file.delete();
        }
        //noinspection ResultOfMethodCallIgnored
        tmp.renameTo(file);
    }

    /** Section-aware INI map: keys stored as "section.key" matching ports/sdl2/settings.c. */
    private static LinkedHashMap<String, LinkedHashMap<String, String>> readSectionedIni(File file) {
        LinkedHashMap<String, LinkedHashMap<String, String>> sections = new LinkedHashMap<>();
        if (!file.exists()) {
            return sections;
        }
        String curSection = "";
        try (BufferedReader br = new BufferedReader(
                new InputStreamReader(new FileInputStream(file), StandardCharsets.UTF_8))) {
            String line;
            while ((line = br.readLine()) != null) {
                String trimmed = line.trim();
                if (trimmed.isEmpty() || trimmed.startsWith("#") || trimmed.startsWith(";")) {
                    continue;
                }
                if (trimmed.startsWith("[") && trimmed.endsWith("]")) {
                    curSection = trimmed.substring(1, trimmed.length() - 1).trim();
                    if (!sections.containsKey(curSection)) {
                        sections.put(curSection, new LinkedHashMap<>());
                    }
                    continue;
                }
                int eq = trimmed.indexOf('=');
                if (eq > 0 && !curSection.isEmpty()) {
                    String k = trimmed.substring(0, eq).trim();
                    String valRaw = trimmed.substring(eq + 1);
                    int semi = valRaw.indexOf(';');
                    int hash = valRaw.indexOf('#');
                    int cut = -1;
                    if (semi >= 0 && hash >= 0) cut = Math.min(semi, hash);
                    else if (semi >= 0) cut = semi;
                    else if (hash >= 0) cut = hash;
                    String v = (cut >= 0 ? valRaw.substring(0, cut) : valRaw).trim();
                    LinkedHashMap<String, String> secMap = sections.get(curSection);
                    if (secMap == null) {
                        secMap = new LinkedHashMap<>();
                        sections.put(curSection, secMap);
                    }
                    secMap.put(k, v);
                }
            }
        } catch (Throwable ignored) {
        }
        return sections;
    }

    private static void putSectionValue(
            LinkedHashMap<String, LinkedHashMap<String, String>> sections,
            String section, String key, String value) {
        LinkedHashMap<String, String> sec = sections.get(section);
        if (sec == null) {
            sec = new LinkedHashMap<>();
            sections.put(section, sec);
        }
        sec.put(key, value);
    }

    private static String getSectionValue(
            LinkedHashMap<String, LinkedHashMap<String, String>> sections,
            String section, String key) {
        LinkedHashMap<String, String> sec = sections.get(section);
        return sec != null ? sec.get(key) : null;
    }

    private static void writeSectionedIni(
            File file, LinkedHashMap<String, LinkedHashMap<String, String>> sections) {
        File tmp = new File(file.getParentFile(), file.getName() + ".tmp");
        try (BufferedWriter bw = new BufferedWriter(
                new OutputStreamWriter(new FileOutputStream(tmp), StandardCharsets.UTF_8))) {
            boolean first = true;
            for (Map.Entry<String, LinkedHashMap<String, String>> sec : sections.entrySet()) {
                if (!first) {
                    bw.write("\n");
                }
                first = false;
                bw.write("[" + sec.getKey() + "]\n");
                for (Map.Entry<String, String> kv : sec.getValue().entrySet()) {
                    bw.write(kv.getKey() + " = " + kv.getValue() + "\n");
                }
            }
        } catch (Throwable ignored) {
            return;
        }
        if (file.exists()) {
            //noinspection ResultOfMethodCallIgnored
            file.delete();
        }
        //noinspection ResultOfMethodCallIgnored
        tmp.renameTo(file);
    }

    private static int parseIntSafe(String s, int fallback, int min, int max) {
        if (s == null) return fallback;
        try {
            int v = Integer.parseInt(s.trim());
            return Math.max(min, Math.min(max, v));
        } catch (Throwable t) {
            return fallback;
        }
    }

    private static float parseFloatSafe(String s, float fallback, float min, float max) {
        if (s == null) return fallback;
        try {
            float v = Float.parseFloat(s.trim().replace(',', '.'));
            return Math.max(min, Math.min(max, v));
        } catch (Throwable t) {
            return fallback;
        }
    }

    private static boolean parseYesNo(String s, boolean fallback) {
        if (s == null) return fallback;
        String t = s.trim();
        if ("yes".equalsIgnoreCase(t) || "1".equals(t) || "true".equalsIgnoreCase(t)) return true;
        if ("no".equalsIgnoreCase(t) || "0".equals(t) || "false".equalsIgnoreCase(t)) return false;
        return fallback;
    }

    private void loadAllConfigsIntoUi() {
        mSuppressListeners = true;
        try {
            Map<String, String> portMap = readFlatIniMap(getAndroidPortIniFile());
            Map<String, String> touchMap = readFlatIniMap(getTouchControlsIniFile());
            LinkedHashMap<String, LinkedHashMap<String, String>> consoleSec =
                readSectionedIni(getConsoleIniFile());

            // Touch settings (matching exact keys in touch_controls.c!)
            int moveMode = parseIntSafe(touchMap.get("move_mode"), 0, 0, 2);
            int colorIdx = parseIntSafe(touchMap.get("player_color"), 0, 0, 2);
            float sens = parseFloatSafe(touchMap.get("trackball_sensitivity"), 1.15f, 0.25f, 3.0f);
            float friction = parseFloatSafe(touchMap.get("trackball_friction"), 0.91f, 0.80f, 0.97f);
            float opacity = parseFloatSafe(touchMap.get("master_opacity"), 0.88f, 0.20f, 1.0f);
            int floating = parseIntSafe(touchMap.get("floating_origin"), 0, 0, 1);
            int haptics = parseIntSafe(touchMap.get("haptics_enabled"), 1, 0, 1);
            int autoHide = parseIntSafe(touchMap.get("auto_hide_gamepad"), 1, 0, 1);

            mMoveModeSpinner.setSelection(moveMode);
            mThemeColorSpinner.setSelection(colorIdx);

            int sensPct = Math.max(25, Math.min(300, Math.round(sens * 100f)));
            mSensitivitySeek.setProgress(sensPct - 25);
            mSensitivityLabel.setText(getString(R.string.lbl_sensitivity, sensPct));

            int inertiaPct = Math.max(80, Math.min(97, Math.round(friction * 100f)));
            mInertiaSeek.setProgress(inertiaPct - 80);
            mInertiaLabel.setText(getString(R.string.lbl_inertia, inertiaPct));

            int opacityPct = Math.max(20, Math.min(100, Math.round(opacity * 100f)));
            mOpacitySeek.setProgress(opacityPct - 20);
            mOpacityLabel.setText(getString(R.string.lbl_hud_opacity, opacityPct));

            mFloatingOriginSwitch.setChecked(floating != 0);
            mHapticsSwitch.setChecked(haptics != 0);
            mAutoHidePadSwitch.setChecked(autoHide != 0);

            // Port / Video / ETC2 / Audio settings (matching android_main.c)
            int filter = parseIntSafe(portMap.get("filter"), 0, 0, 3);
            int aspect = parseIntSafe(portMap.get("aspect"), 0, 0, 2);
            int etc2 = parseIntSafe(portMap.get("etc2_mode"), 1, 0, 2);
            int lowLat = parseIntSafe(portMap.get("low_latency_audio"), 1, 0, 1);
            int vsync = parseIntSafe(portMap.get("vsync"), 1, 0, 1);
            int showFps = parseIntSafe(portMap.get("show_fps"), 0, 0, 1);

            mFilterSpinner.setSelection(filter);
            mAspectSpinner.setSelection(aspect);
            // Map C enum (1=Auto, 2=FullStream, 0=Off) <-> Spinner index (0=Auto, 1=Stream, 2=Off)
            int etc2SpinnerIdx = (etc2 == 1) ? 0 : (etc2 == 2 ? 1 : 2);
            mEtc2Spinner.setSelection(etc2SpinnerIdx);

            // Console settings (matching ports/sdl2/settings.c sections & formats)
            boolean widescreen = parseYesNo(getSectionValue(consoleSec, "video", "wide"), true);
            float continuesVal = parseFloatSafe(
                getSectionValue(consoleSec, "game", "continues"), -1.0f, -1.0f, 9.0f);
            float volumeUnit = parseFloatSafe(
                getSectionValue(consoleSec, "sound", "volume"), 1.0f, 0.0f, 2.0f);
            int volumePct = Math.max(0, Math.min(100, Math.round(volumeUnit * 100f)));

            mWidescreenSwitch.setChecked(widescreen);
            mFreePlaySwitch.setChecked(continuesVal < 0.0f);
            mVolumeSeek.setProgress(volumePct);
            mVolumeLabel.setText(getString(R.string.lbl_volume, volumePct));
            mLowLatencySwitch.setChecked(lowLat != 0);
            mVsyncSwitch.setChecked(vsync != 0);
            mShowFpsSwitch.setChecked(showFps != 0);
        } finally {
            mSuppressListeners = false;
        }
    }

    private void saveAllConfigs() {
        // 1. android_port.ini
        LinkedHashMap<String, String> portMap = readFlatIniMap(getAndroidPortIniFile());
        portMap.put("filter", String.valueOf(mFilterSpinner.getSelectedItemPosition()));
        portMap.put("aspect", String.valueOf(mAspectSpinner.getSelectedItemPosition()));
        int spEtc2 = mEtc2Spinner.getSelectedItemPosition();
        int cEtc2 = (spEtc2 == 0) ? 1 : (spEtc2 == 1 ? 2 : 0);
        portMap.put("etc2_mode", String.valueOf(cEtc2));
        portMap.put("show_fps", mShowFpsSwitch.isChecked() ? "1" : "0");
        portMap.put("low_latency_audio", mLowLatencySwitch.isChecked() ? "1" : "0");
        portMap.put("vsync", mVsyncSwitch.isChecked() ? "1" : "0");
        writeFlatIniMap(getAndroidPortIniFile(), "SegaSonic Android Port Settings", portMap);

        // 2. touch_controls.ini (preserves widget.nx/ny/scale/opacity/visible from HUD editor!)
        LinkedHashMap<String, String> touchMap = readFlatIniMap(getTouchControlsIniFile());
        float sens = (25 + mSensitivitySeek.getProgress()) / 100.0f;
        float friction = (80 + mInertiaSeek.getProgress()) / 100.0f;
        float opacity = (20 + mOpacitySeek.getProgress()) / 100.0f;
        touchMap.put("move_mode", String.valueOf(mMoveModeSpinner.getSelectedItemPosition()));
        touchMap.put("player_color", String.valueOf(mThemeColorSpinner.getSelectedItemPosition()));
        touchMap.put("trackball_sensitivity", String.format(Locale.US, "%.3f", sens));
        touchMap.put("trackball_friction", String.format(Locale.US, "%.3f", friction));
        touchMap.put("master_opacity", String.format(Locale.US, "%.3f", opacity));
        touchMap.put("floating_origin", mFloatingOriginSwitch.isChecked() ? "1" : "0");
        touchMap.put("haptics_enabled", mHapticsSwitch.isChecked() ? "1" : "0");
        touchMap.put("auto_hide_gamepad", mAutoHidePadSwitch.isChecked() ? "1" : "0");
        writeFlatIniMap(getTouchControlsIniFile(), "SegaSonic The Hedgehog Android Touch Controls", touchMap);

        // 3. segasonic.ini (Console Edition) & sonic.ini (Arcade Edition)
        String volStr = String.format(Locale.US, "%.2f", mVolumeSeek.getProgress() / 100.0f);
        String crtStr = (mFilterSpinner.getSelectedItemPosition() == 3) ? "yes" : "no";

        LinkedHashMap<String, LinkedHashMap<String, String>> consoleSec =
            readSectionedIni(getConsoleIniFile());
        putSectionValue(consoleSec, "sound", "volume", volStr);
        putSectionValue(consoleSec, "video", "crt", crtStr);
        putSectionValue(consoleSec, "video", "wide", mWidescreenSwitch.isChecked() ? "yes" : "no");
        putSectionValue(consoleSec, "game", "continues", mFreePlaySwitch.isChecked() ? "-1" : "3");
        writeSectionedIni(getConsoleIniFile(), consoleSec);

        LinkedHashMap<String, LinkedHashMap<String, String>> arcadeSec =
            readSectionedIni(getArcadeIniFile());
        putSectionValue(arcadeSec, "sound", "volume", volStr);
        putSectionValue(arcadeSec, "video", "crt", crtStr);
        writeSectionedIni(getArcadeIniFile(), arcadeSec);
    }
}
