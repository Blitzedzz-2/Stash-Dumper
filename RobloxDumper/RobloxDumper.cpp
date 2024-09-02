#include <iostream>
#include <fstream>
#include <windows.h>
#include <shlobj.h>
#include <shobjidl_core.h>
#include <string>

#pragma comment(lib, "shell32.lib")

void SetConsoleColor(WORD color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

void PrintWithColor(const std::wstring& message, WORD color) {
    SetConsoleColor(color);
    std::wcout << L"[-] " << message << std::endl;
    SetConsoleColor(7);
}

int main() {
    SetConsoleColor(9);
    std::wcout << L"  ______  ________   ______    ______   __    __ \n"
        " /      \\|        \\ /      \\  /      \\ |  \\  |  \\\n"
        "|  $$$$$$\\\\$$$$$$$$|  $$$$$$\\|  $$$$$$\\| $$  | $$\n"
        "| $$___\\$$  | $$   | $$__| $$| $$___\\$$| $$__| $$\n"
        " \\$$    \\   | $$   | $$    $$ \\$$    \\ | $$    $$\n"
        " _\\$$$$$$\\  | $$   | $$$$$$$$ _\\$$$$$$\\| $$$$$$$$\n"
        "|  \\__| $$  | $$   | $$  | $$|  \\__| $$| $$  | $$\n"
        " \\$$    $$  | $$   | $$  | $$ \\$$    $$| $$  | $$\n"
        "  \\$$$$$$    \\$$    \\$$   \\$$  \\$$$$$$  \\$$   \\$$\n"
        "                                                 \n"
        "                                                 \n";
    SetConsoleColor(7);

    wchar_t RobloxPlayerShortcut[MAX_PATH];
    ExpandEnvironmentStrings(L"%userprofile%\\AppData\\Roaming\\Microsoft\\Windows\\Start Menu\\Programs\\Roblox\\Roblox Player.lnk", RobloxPlayerShortcut, MAX_PATH);

    wchar_t RobloxStudioShortcut[MAX_PATH];
    ExpandEnvironmentStrings(L"%userprofile%\\AppData\\Roaming\\Microsoft\\Windows\\Start Menu\\Programs\\Roblox\\Roblox Studio.lnk", RobloxStudioShortcut, MAX_PATH);

    IShellLink* pShellLink = nullptr;
    IPersistFile* pPersistFile = nullptr;
    wchar_t RobloxPlayerPath[MAX_PATH] = { 0 };
    wchar_t RobloxStudioPath[MAX_PATH] = { 0 };

    CoInitialize(nullptr);
    CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER, IID_IShellLink, reinterpret_cast<void**>(&pShellLink));
    pShellLink->QueryInterface(IID_IPersistFile, reinterpret_cast<void**>(&pPersistFile));
    pPersistFile->Load(RobloxPlayerShortcut, STGM_READ);
    pShellLink->GetPath(RobloxPlayerPath, MAX_PATH, nullptr, SLGP_UNCPRIORITY);
    pPersistFile->Release();
    pShellLink->Release();

    CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER, IID_IShellLink, reinterpret_cast<void**>(&pShellLink));
    pShellLink->QueryInterface(IID_IPersistFile, reinterpret_cast<void**>(&pPersistFile));
    pPersistFile->Load(RobloxStudioShortcut, STGM_READ);
    pShellLink->GetPath(RobloxStudioPath, MAX_PATH, nullptr, SLGP_UNCPRIORITY);
    pPersistFile->Release();
    pShellLink->Release();
    CoUninitialize();

    if (wcslen(RobloxPlayerPath) == 0) {
        PrintWithColor(L"Failed to retrieve the target path from Roblox Player shortcut.", 12);
        return 1;
    }

    if (wcslen(RobloxStudioPath) == 0) {
        PrintWithColor(L"Failed to retrieve the target path from Roblox Studio shortcut.", 12);
        return 1;
    }

    PrintWithColor(L"Found ROBLOX PLAYER at " + std::wstring(RobloxPlayerPath), 10);
    PrintWithColor(L"Found ROBLOX STUDIO at " + std::wstring(RobloxStudioPath), 10);

    BROWSEINFO bi = { 0 };
    bi.lpszTitle = L"Select the destination folder";
    LPITEMIDLIST pidl = SHBrowseForFolder(&bi);

    if (pidl == 0) {
        PrintWithColor(L"No destination folder selected.", 12);
        return 1;
    }

    wchar_t destinationFolder[MAX_PATH];
    if (!SHGetPathFromIDList(pidl, destinationFolder)) {
        PrintWithColor(L"Failed to get the path from ID list.", 12);
        return 1;
    }

    IMalloc* imalloc = nullptr;
    SHGetMalloc(&imalloc);
    imalloc->Free(pidl);
    imalloc->Release();

    std::wstring RobloxPlayerDump = std::wstring(destinationFolder) + L"\\RobloxPlayerBeta_Dump.exe";
    std::wstring RobloxStudioDump = std::wstring(destinationFolder) + L"\\RobloxStudioBeta_Dump.exe";

    PrintWithColor(L"Dumping ROBLOX PLAYER. Please wait", 14);
    std::ifstream srcPlayer(RobloxPlayerPath, std::ios::binary);
    std::ofstream destPlayer(RobloxPlayerDump, std::ios::binary);
    destPlayer << srcPlayer.rdbuf();
    srcPlayer.close();
    destPlayer.close();

    PrintWithColor(L"Dumping ROBLOX STUDIO. Please wait", 14);
    std::ifstream srcStudio(RobloxStudioPath, std::ios::binary);
    std::ofstream destStudio(RobloxStudioDump, std::ios::binary);
    destStudio << srcStudio.rdbuf();
    srcStudio.close();
    destStudio.close();

    PrintWithColor(L"Dumped ROBLOX PLAYER to " + RobloxPlayerDump, 10);
    PrintWithColor(L"Dumped ROBLOX STUDIO to " + RobloxStudioDump, 10);
    PrintWithColor(L"Made by blitzedzz", 11);

    system("pause >nul");
    return 0;
}
