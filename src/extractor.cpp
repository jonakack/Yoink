////////////////////////////////////////////////////////////////////
//                                                                //
//    Copyright (c) 2026 Jonathan Käcker. All rights reserved.    //
//                                                                //
////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <fstream>
#include <string>

#include "extractor.hpp"

#define ID_YTDLP  101
#define ID_FFMPEG 102

static std::string g_tempDir;

// ----------------------------------------------------------------
//  Looks up the embedded RCDATA resource with the given ID, loads it
//  into memory, and writes its raw bytes to destPath on disk.
//  Returns true on success, false if the resource cannot be found,
//  loaded, or written.
// ----------------------------------------------------------------
static bool extractResource(int id, const std::string& destPath)
{
    // Find the resource in our own .exe (nullptr = this .exe)
    HRSRC hRes = FindResourceA(nullptr, MAKEINTRESOURCEA(id), MAKEINTRESOURCEA(10));
    if (!hRes) return false;

    // Load it into memory
    HGLOBAL hData = LoadResource(nullptr, hRes);
    if (!hData) return false;

    // Get a pointer to the raw bytes and the size
    void* pData = LockResource(hData);
    DWORD size  = SizeofResource(nullptr, hRes);
    if (!pData || size == 0) return false;

    // Write the bytes to disk
    std::ofstream out(destPath, std::ios::binary);
    if (!out) return false;
    out.write(static_cast<const char*>(pData), size);
    return out.good();
}

// ----------------------------------------------------------------
//  Creates a unique temporary directory and extracts the embedded
//  yt-dlp.exe and ffmpeg.exe resources into it. Must be called once
//  at startup before ytDlpPath() or ffmpegPath() are used.
//  Returns true on success, false if any step fails.
// ----------------------------------------------------------------
bool Extractor::extractTools()
{
    // Get the system temp folder e.g. C:\Users\jonte\AppData\Local\Temp
    char tempPath[MAX_PATH];
    GetTempPathA(MAX_PATH, tempPath);

    // Generate a unique path inside it e.g. C:\...\Temp\yoinkA3F2.tmp
    char uniquePath[MAX_PATH];
    GetTempFileNameA(tempPath, "yoink", 0, uniquePath);

    // GetTempFileNameA creates a file — delete it and make a folder instead
    DeleteFileA(uniquePath);
    CreateDirectoryA(uniquePath, nullptr);

    g_tempDir = uniquePath;

    // Extract both tools into the temp folder
    if (!extractResource(ID_YTDLP,  g_tempDir + "\\yt-dlp.exe"))  return false;
    if (!extractResource(ID_FFMPEG, g_tempDir + "\\ffmpeg.exe"))   return false;

    return true;
}

std::string Extractor::ytDlpPath()
{
    return g_tempDir + "\\yt-dlp.exe";
}

std::string Extractor::ffmpegPath()
{
    return g_tempDir + "\\ffmpeg.exe";
}

std::string Extractor::tempDir()
{
    return g_tempDir;
}

void Extractor::cleanup()
{
    if (g_tempDir.empty()) return;
    DeleteFileA((g_tempDir + "\\yt-dlp.exe").c_str());
    DeleteFileA((g_tempDir + "\\ffmpeg.exe").c_str());
    RemoveDirectoryA(g_tempDir.c_str());
    g_tempDir.clear();
}