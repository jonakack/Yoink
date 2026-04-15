# Yoink

A simple YouTube downloader for Windows. Paste a URL, pick a format and quality, hit download.

Built with C++ and Qt6. Uses [yt-dlp](https://github.com/yt-dlp/yt-dlp) and [ffmpeg](https://ffmpeg.org/) under the hood — both are embedded into the executable, so there's nothing extra to install.

## Features

- Download as MP3 or MP4
- Quality selection (video resolution or audio bitrate)
- Progress bar with download speed and size
- Remembers your last output folder
- Single portable `.exe` — no dependencies, no installer

## Building

**Requirements:**
- CMake 3.20+
- Qt 6 (Widgets, Concurrent)
- MinGW 13

Place `yt-dlp.exe` and `ffmpeg.exe` in the `bin/` folder, then:

```
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . -j
```

To create a standalone deployment, run `windeployqt` on the resulting `Yoink.exe`.

## License

Copyright (c) 2026 Jonathan Käcker. All rights reserved.