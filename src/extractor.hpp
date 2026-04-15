////////////////////////////////////////////////////////////////////
//                                                                //
//    Copyright (c) 2026 Jonathan Käcker. All rights reserved.    //
//                                                                //
////////////////////////////////////////////////////////////////////

#pragma once

#include <string>

namespace Extractor
{
    bool extractTools();        // Call at startup

    std::string ytDlpPath();    // Returns path to extracted yt-dlp.exe
    std::string ffmpegPath();   // Returns path to extracted ffmpeg.exe
    std::string tempDir();      // Returns the temp directory containing the tools

    void cleanup();             // Call at exit
}