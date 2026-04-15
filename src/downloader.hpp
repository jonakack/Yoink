////////////////////////////////////////////////////////////////////
//                                                                //
//    Copyright (c) 2026 Jonathan Käcker. All rights reserved.    //
//                                                                //
////////////////////////////////////////////////////////////////////

#pragma once

#include <string>
#include <functional>

class Downloader
{
public:
    bool run(const std::string &url, 
             bool isMp3,
             const std::string &quality,
             const std::string &outputFolder,
             const std::string &filename,
             std::function<void(float, float, float, const std::string&)> progress);

private:
    std::string command; // The command that gets sent to yt-dlp.exe
    bool buildCommand(const std::string &url, 
                      bool isMp3,
                      const std::string &quality,
                      const std::string &outputFolder,
                      const std::string &filename);
}; 