# SegaSonic The Hedgehog — Native Android Port

Native C11 / OpenGL ES 3.0 & 2.0 / SDL2 Android port of *SegaSonic The Hedgehog* (Sega System 32), supporting **all 32-bit and 64-bit Android phones, tablets, handhelds, and TVs** from **Android 5.0 (API 21)** through **Android 15+ (API 35+, 16 KB page sizes)**.

Just like the desktop builds, **no ROM or copyrighted game data is included**. On first launch, the built-in Android Launcher verifies your MAME `sonic.zip` (Rev. C, 18 ROM files) by CRC32, extracts `segasonic.assets` into app-private storage, deletes the temporary zip copy, and never needs `sonic.zip` again.

---

## Key Features

### 1. Native Android Launcher (`LauncherActivity` — English & Russian UI)
- **One-Tap `sonic.zip` Import**: Uses Android's Storage Access Framework (`ACTION_OPEN_DOCUMENT`) — zero invasive storage permissions required. Verifies all 18 ROM CRC32s via `libsegasonic_launcher.so` (`sonic_data`) and extracts `segasonic.assets` asynchronously with live progress.
- **Dual Edition Support** (isolated `:game` process for 100% clean C static/BSS state on every launch):
  - **Console Edition (`libsegasonic_console.so`)**: 16:9 widescreen stages, home title menu, Time Attack & split records, Stage Select, English captions, Free Play / Continues, Pause menu.
  - **Original Arcade Edition (`libsegasonic_arcade.so`)**: Frame-exact 1993 Sega System 32 arcade cabinet experience (Coin / Start / Service / Test).
- **Unified Settings Sync**: Changes made in the Launcher or in the In-Game Quick Menu stay synchronized across `android_port.ini`, `touch_controls.ini`, `segasonic.ini`, and `sonic.ini` while preserving custom per-button coordinates and controller bindings.

### 2. Arcade-Grade Touch Controls & Live HUD Layout Editor (`touch_controls.c`)
- **Three Movement Input Modes**:
  1. **Arcade Trackball (3D Bearing Inertia)**: True velocity-coupled trackball physics with configurable sensitivity, bearing momentum (`80%–97%`), automatic per-frame delta clamping (`±88` counts/frame so System 32 signed deltas never wrap/reverse), spin-stop brake when planting a thumb, and optional subtle rolling haptics on amplitude-controlled motors.
  2. **Virtual Analog Stick**: Smooth proportional vector stick with configurable deadzone and optional floating-origin mode.
  3. **Classic 8-Way D-Pad**: Crisp directional zones with diagonal lock-in.
- **Multi-Touch Action & Cabinet Buttons**: Jump / Spin (`A`), Rapid-Jump Turbo (`TURBO`), Start / Pause (`START`), Coin / Back (`COIN` / `BACK`), Operator Service (`SVC`), and Top-Center Quick Menu Pill (`•••`).
- **Interactive Live HUD Layout Editor** (accessible directly from the Launcher or in-game Quick Menu):
  - **Drag** any control anywhere on screen (with optional grid snap).
  - **Two-Finger Pinch-to-Scale** any selected control (`0.50x` to `2.25x`) or fine-tune with `-` / `+` inspector buttons.
  - **Per-Widget Opacity & Visibility**: Adjust individual opacity or hide unused buttons (e.g., `TURBO` or `SVC`).
  - **4 Built-In Layout Presets**: *Arcade Default*, *Compact Phone*, *Wide Tablet*, and *Left-Handed* (swaps movement pad to the right and action buttons to the left), plus character color themes (*Sonic Cobalt Blue*, *Mighty Crimson Red*, *Ray Golden Yellow*).
  - **Bluetooth / USB Gamepad Auto-Hide**: Automatically fades out the touch overlay when physical controller input is detected and restores it instantly on screen touch.

### 3. Hardware ETC2 Texture Compression & CPU/GPU Optimizations
- **Khronos-Exact ETC2 & EAC Codec (`etc2.c`)**:
  - Encodes the `512×512` procedural vector-antialiased UI & HUD sprite atlas into **`GL_COMPRESSED_RGBA8_ETC2_EAC` (`0x9278`)** on OpenGL ES 3.0+ GPUs — cutting UI texture VRAM and memory bandwidth by **4× (`1024 KB → 256 KB`, >41.8 dB PSNR)** and caching the compressed `.etc2` payload (`etc2_cache/ui_atlas_v2.etc2`) with CRC32 validation for instant `< 1 ms` startup on subsequent runs.
  - Optional **Real-Time Dirty-Strip `GL_COMPRESSED_RGB8_ETC2` (`0x9274`) Frame Streaming** mode (4 bpp, **8× smaller** than `RGBA8888`), re-encoding only modified `4×4` blocks in `~0.15 ms/frame` with ARM NEON SIMD acceleration.
  - **Default Lossless PBO Mode**:System 32's hardware DAC outputs 15-bit `RGB555`. Converting only dirty scanlines via ARM NEON into `RGB565` inside double-buffered OpenGL ES 3.0 `GL_PIXEL_UNPACK_BUFFER` (async DMA PBOs) is **100% mathematically lossless** at half the bandwidth of 32-bit RGBA.
  - **Automatic OpenGL ES 2.0 Fallback**: Legacy 32-bit Android phones without OpenGL ES 3.0 / ETC2 hardware automatically fall back to uncompressed `GL_RGB565` + `GL_RGBA` textures while linking strictly against `libGLESv2.so` (`dlsym`-loading GLES 3.0 PBO entry points at runtime) so the APK loads on every Android 5.0+ device.
- **37% Faster System 32 Software Video & Fixed-Point Audio**:
  - Unpacked 4bpp tile cache, specialized non-zoomed/opaque scanline blitters, cached text-layer cells, and precomputed `mix()` color LUTs in `runtime/src/video.c` accelerate software System 32 frame rendering by **37.1%** while preserving bit-identical frame and sound hashes.
  - Zero-FPU `Q16.16` fixed-point stereo resampler mixes YM3438 FM + Dual RF5C68 PCM at `48 kHz` (`512`-sample `~10.6 ms` low-latency buffer).

### 4. Universal 32-Bit & 64-Bit Compatibility
- **ABIs**: `armeabi-v7a` (32-bit ARM), `arm64-v8a` (64-bit ARM), `x86` (32-bit Intel/emulator), `x86_64` (64-bit Intel/Chromebook/emulator).
- **32-Bit ARMv7 Unaligned Access Hardening**: Decompiled V60 code performs unaligned 16/32-bit RAM/ROM reads and writes (`src/ram.h`). On `armeabi-v7a`, `aligned(1)` access typedefs in `runtime/include/*/support.h` combined with `-mllvm -arm-assume-misaligned-load-store -fno-slp-vectorize` prevent `LDRD`/`STRD`/`LDM`/`STM` generation, eliminating `SIGBUS (BUS_ADRALN)` faults on strict 32-bit ARMv7 kernels without altering a single line of `src/`.
- **16 KB Page-Size Ready**: All native libraries (`libSDL2.so`, `libsegasonic_launcher.so`, `libsegasonic_console.so`, `libsegasonic_arcade.so`) link with `-Wl,-z,max-page-size=16384` and `ANDROID_SUPPORT_FLEXIBLE_PAGE_SIZES=ON` for Android 15+ devices.

---

## Building the APK

### Requirements
- **JDK 17+**
- **Android SDK** (`platforms;android-35`, `build-tools;35.0.0`)
- **Android NDK** `27.2.12479018` and **CMake** `3.22.1+`
- Internet access on first configure (CMake `FetchContent` downloads official SDL2 `release-2.32.10` automatically, or place a local SDL2 checkout at `ports/android/sdl2`).

### Command-Line Build
```bash
cd ports/android
./gradlew assembleRelease
```

The signed universal APK (containing all four 32-bit and 64-bit ABIs) is written to:
```
ports/android/app/build/outputs/apk/release/app-release.apk
```

To install directly onto a connected phone via `adb`:
```bash
adb install -r ports/android/app/build/outputs/apk/release/app-release.apk
```

---

## Краткая инструкция (RU)

1. Установите `segasonic-android-universal.apk` (работает на любых **32-битных и 64-битных** телефонах с Android 5.0 и новее).
2. При первом запуске нажмите **«Выбрать и импортировать sonic.zip»** и укажите архив MAME `sonic.zip` (Rev. C). Лаунчер проверит контрольные суммы CRC32 всех 18 микросхем ПЗУ и создаст локальный пакет `segasonic.assets`.
3. Выберите **Console Edition (16:9)** для широкоэкранной игры без монет с меню, Time Attack и выбором уровней, либо **Оригинальную Аркаду (System 32)**.
4. Для настройки расположения, размера, прозрачности кнопок и физики трекбола нажмите **«Настроить сенсорное управление (Редактор)»** в лаунчере или кнопку **`•••`** в верхней части экрана прямо во время игры.
