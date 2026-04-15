////////////////////////////////////////////////////////////////////
//                                                                //
//    Copyright (c) 2026 Jonathan Käcker. All rights reserved.    //
//                                                                //
////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <regex>

#include "downloader.hpp"
#include "extractor.hpp"

// ----------------------------------------------------------------
//  Spawns the assembled yt-dlp command as a child process, pipes its
//  stdout and stderr back to the console in real time, then waits for
//  the process to exit. Returns true if yt-dlp exits with code 0,
//  false otherwise.
// ----------------------------------------------------------------
bool Downloader::run(const std::string &url, 
                     bool isMp3,
                     const std::string &quality,
                     const std::string &outputFolder,
                     const std::string &filename,
                     std::function<void(float, float, float, const std::string&)> progress)
{
    buildCommand(url, isMp3, quality, outputFolder, filename);

    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(sa);
    sa.lpSecurityDescriptor = nullptr;
    sa.bInheritHandle = true;

    HANDLE readEnd, writeEnd;
    CreatePipe(&readEnd, &writeEnd, &sa, 0);
    SetHandleInformation(readEnd, HANDLE_FLAG_INHERIT, 0);

    STARTUPINFOA si = {};
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdOutput = writeEnd; // yt-dlp's stdout goes into pipe
    si.hStdError = writeEnd;  // yt-dlp's stderr goes into pipe

    PROCESS_INFORMATION pi = {};
    CreateProcessA(
        nullptr,
        command.data(),
        nullptr,
        nullptr,
        TRUE,
        CREATE_NO_WINDOW,
        nullptr,
        nullptr,
        &si,
        &pi);

    CloseHandle(writeEnd);

    char buffer[4096];
    unsigned long bytesRead;

    while (ReadFile(readEnd, buffer, sizeof(buffer) - 1, &bytesRead, nullptr) && bytesRead > 0)
    {
        buffer[bytesRead] = '\0';

        std::string chunk(buffer, bytesRead);
        std::regex re(R"(\[download\]\s+([\d.]+)%\s+of\s+~?([\d.]+)(\S+)\s+at\s+(\S+))");
        
        float percent = -1.0f;
        float totalMiB = 0.0f;
        float downloadedMiB = 0.0f;
        std::string speed;
        auto it  = std::sregex_iterator(chunk.begin(), chunk.end(), re);
        auto end = std::sregex_iterator();
        for (; it != end; ++it)
        {
            percent = std::stof((*it)[1].str());
            float size = std::stof((*it)[2].str());
            std::string unit = (*it)[3].str();
            speed = (*it)[4].str();

            if (unit == "GiB") size *= 1024.0f;
            else if (unit == "KiB") size /= 1024.0f;
            totalMiB = size;
            downloadedMiB = totalMiB * percent / 100.0f;
        }

        if (percent >= 0.0f)
            progress(percent, downloadedMiB, totalMiB, speed);
    }

    // Cleanup
    CloseHandle(readEnd);
    WaitForSingleObject(pi.hProcess, INFINITE); // Wait for yt-dlp to fully finish

    DWORD exitCode;
    GetExitCodeProcess(pi.hProcess, &exitCode);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return exitCode == 0;
}

// ----------------------------------------------------------------
//  Builds the yt-dlp command string from the given parameters.
//  The resulting command is stored in this->command.
// ----------------------------------------------------------------
bool Downloader::buildCommand(const std::string &url, 
                              bool isMp3,
                              const std::string &quality,
                              const std::string &outputFolder,
                              const std::string &filename)
{
    std::string cmd = Extractor::ytDlpPath()
                    + " --ffmpeg-location " + Extractor::tempDir();

    // Map UI quality labels to yt-dlp values
    std::string ytQuality;
    bool bestQuality = (quality == "Highest quality available");

    if (quality == "4K")              ytQuality = "2160";
    else if (quality == "1440p")      ytQuality = "1440";
    else if (quality == "1080p")      ytQuality = "1080";
    else if (quality == "720p")       ytQuality = "720";
    else if (quality == "480p")       ytQuality = "480";
    else if (quality == "360p")       ytQuality = "360";
    else if (quality == "240p")       ytQuality = "240";
    else if (quality == "144p")       ytQuality = "144";
    else if (quality == "320kbp/s")   ytQuality = "0";
    else if (quality == "256kbp/s")   ytQuality = "2";
    else if (quality == "128kbp/s")   ytQuality = "5";
    else if (quality == "96kbp/s")    ytQuality = "9";

    if (isMp3)
    {
        cmd += " -x --audio-format mp3";
        if (bestQuality)
            cmd += " --audio-quality 0";
        else if (!ytQuality.empty())
            cmd += " --audio-quality " + ytQuality;
    }
    else
    {
        if (bestQuality)
            cmd += " -S \"vcodec:h264,acodec:m4a\" --merge-output-format mp4";
        else if (!ytQuality.empty())
            cmd += " -S \"vcodec:h264,acodec:m4a\" -f \"bestvideo[height<=" + ytQuality + "]+bestaudio/best[height<=" + ytQuality + "]\" --merge-output-format mp4";
    }

    // Output path
    std::string outputName = filename.empty() ? "%(title)s" : filename;
    cmd += " -o \"" + outputFolder + "\\" + outputName + ".%(ext)s\"";

    cmd += " " + url;

    this->command = cmd;
    return true;
}