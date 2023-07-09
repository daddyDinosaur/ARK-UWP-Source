#include "Logger.h"
#include <Shlobj.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Storage.h>

bool Enabled = true;

void Logger::Log(const char* format, ...)
{
    SYSTEMTIME rawtime;
    GetSystemTime(&rawtime);
    char buf[MAX_PATH];
    auto size = GetTimeFormatA(LOCALE_CUSTOM_DEFAULT, 0, &rawtime, "[HH':'mm':'ss] ", buf, MAX_PATH) - 1;
    size += snprintf(buf + size, sizeof(buf) - size, "[TID: 0x%X] ", GetCurrentThreadId());
    va_list argptr;
    va_start(argptr, format);
    size += vsnprintf(buf + size, sizeof(buf) - size, format, argptr);
    WriteFile(file, buf, size, NULL, NULL);
    va_end(argptr);
}


bool Logger::Remove()
{
    if (!file) return true;
    return CloseHandle(file);
}

bool Logger::Init(std::wstring Param)
{
    // C:\\Users\\Dev\\AppData\\Local\\Packages\\StudioWildcard.4558480580BB9_1w2mm55455e38\\TempState\\Debug.txt

    if (Enabled)
    {
        //RetrieveUWPFolder();
        /*std::wstring zinger = s2ws(RetrieveUWPFolder());*/
        //std::wstring FilePath(L"C:\\Users\\Dev\\AppData\\Local\\Packages\\StudioWildcard.4558480580BB9_1w2mm55455e38\\TempState\\Debug.txt");
        file = CreateFileW(Param.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        return file != INVALID_HANDLE_VALUE;
    }
}













// E:\\WpSystem\\S-1-5-21-4JUdGzvrMFDWrUUwY3toJATSeNwjn54LkCnKBPRzDuhzi5vSepHfUckJNxRL2gjkNrSqtCoRUrEDAgRwsQvVCjZbRyFTLRNyDmT1a1boZVTempState\\Debug.txt
// prodigy420 C:\\Users\\steve\\AppData\\Local\\Packages\\StudioWildcard.4558480580BB9_1w2mm55455e38\\TempState\\Debug.txt
// Shroom E:\\WpSystem\\S-1-5-21-3585895747-4JUdGzvrMFDWrUUwY3toJATSeNwjn54LkCnKBPRzDuhzi5vSepHfUckJNxRL2gjkNrSqtCoRUrEDAgRwsQvVCjZbRyFTLRNyDmT1a1boZVDebug.txt
// System96 C:\\Users\\Blanket\\AppData\\Local\\Packages\\StudioWildcard.4558480580BB9_1w2mm55455e38\\TempState\\Debug.txt
// Drip C:\\Users\\mwinc\\AppData\\Local\\Packages\\StudioWildcard.4558480580BB9_1w2mm55455e38\\TempState\\Debug.txt
// Cal I:\\WpSystem\\S-1-5-21-3577560923-312526556-3490813979-1001\\AppData\\Local\\Packages\\StudioWildcard.4558480580BB9_1w2mm55455e38\\TempState\\Debug.txt
// Cuckoo C:\\Users\\pluto\\AppData\\Local\\Packages\\StudioWildcard.4558480580BB9_1w2mm55455e38\\TempState\\Debug.txt
// Leeroy Jenkins C:\\Users\\Default.DESKTOP-8B35IA0\\AppData\\Local\\Packages\\StudioWildcard.4558480580BB9_1w2mm55455e38\\TempState\\Debug.txt
// Murdom D:\\WpSystem\\S-1-5-21-3111613574-2524581245-2586426736-1000\\AppData\\Local\\Packages\\StudioWildcard.4558480580BB9_1w2mm55455e38\\TempState\\Debug.txt
// Cert D:\\WpSystem\\S-1-5-21-2793685904-3401961663-2657041808-1002\\AppData\\Local\\Packages\\StudioWildcard.4558480580BB9_1w2mm55455e38\\TempState\\Debug.txt
// KSTG C:\\Users\\monke\\AppData\\Local\\Packages\\StudioWildcard.4558480580BB9_1w2mm55455e38\\TempState\\Debug.txt
// Wave Bilky C:\\Users\\Caleb\\AppData\\Local\\Packages\\StudioWildcard.4558480580BB9_1w2mm55455e38\\TempState\\Debug.txt
// Rigz C:\\Users\\Admin\\AppData\\Local\\Packages\\StudioWildcard.4558480580BB9_1w2mm55455e38\\TempState\\Debug.txt
// G5Zer0 D:\\WpSystem\\S-1-5-21-8193316-2593466362-4JUdGzvrMFDWrUUwY3toJATSeNwjn54LkCnKBPRzDuhzi5vSepHfUckJNxRL2gjkNrSqtCoRUrEDAgRwsQvVCjZbRyFTLRNyDmT1a1boZV
// WanHeda D:\\WpSystem\\S-1-5-21-4JUdGzvrMFDWrUUwY3toJATSeNwjn54LkCnKBPRzDuhzi5vSepHfUckJNxRL2gjkNrSqtCoRUrEDAgRwsQvVCjZbRyFTLRNyDmT1a1boZVTempState\\Debug.txt