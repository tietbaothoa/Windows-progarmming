#include <bits/stdc++.h>
#include <Windows.h>
#include <iomanip>
using namespace std;

void ListDir(string path, int loop) {
    string searchPath = path + "\\*";
    WIN32_FIND_DATAA data;
    HANDLE hfind;
    SYSTEMTIME systemTime;

    cout << "Path: " << path << endl;
    hfind = FindFirstFileA(searchPath.c_str(), &data);
    if (hfind != INVALID_HANDLE_VALUE) {
        cout << left << setw(25) << "Create Time" << setw(25) << "Modify Time"
             << setw(25) << "File Attributes" << setw(20) << "File Size" << setw(40) << " File Name" << endl;
        do {
            char CreateTime[20], ModifyTime[20];
            if (FileTimeToSystemTime(&data.ftCreationTime, &systemTime)) {
                sprintf(CreateTime, "%02d/%02d/%04d %02d:%02d:%02d", systemTime.wDay, systemTime.wMonth,
                        systemTime.wYear, systemTime.wHour, systemTime.wMinute, systemTime.wSecond);
                cout << left << setw(25) << CreateTime;
            }
            if (FileTimeToSystemTime(&data.ftLastWriteTime, &systemTime)) {
                sprintf(ModifyTime, "%02d/%02d/%04d %02d:%02d:%02d", systemTime.wDay, systemTime.wMonth,
                        systemTime.wYear, systemTime.wHour, systemTime.wMinute, systemTime.wSecond);
                cout << setw(25) << ModifyTime;
            }

            if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                cout << setw(45) << "<DIR>" << setw(40) << data.cFileName << endl;
                if (loop && strcmp(data.cFileName, ".") != 0 && strcmp(data.cFileName, "..") != 0) {
                    string subPath = path + "\\" + data.cFileName;
                    ListDir(subPath, loop);
                }
            } else {
                cout << setw(25) << "<FILE>";
                LARGE_INTEGER fileSize;
                fileSize.LowPart = data.nFileSizeLow;
                fileSize.HighPart = data.nFileSizeHigh;
                cout << setw(20) << fileSize.QuadPart << setw(40) << data.cFileName << endl;
            }
        } while (FindNextFile(hfind, &data) != 0);
        FindClose(hfind);
    }
}

int main(int argc, char* argv[]) {
    int loop = 0;
    if (argc < 2) {
        cout << "Input: FileExecute.exe <Path> [/s]" << endl;
        return 0;
    }
    string path = argv[1];  
    if (argc == 3 && strcmp(argv[2], "/s") == 0) {
        loop = 1;
    }
    ListDir(path, loop);
    return 0;
}
