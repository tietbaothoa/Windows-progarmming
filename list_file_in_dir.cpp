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
    hfind = FindFirstFileA(searchPath.c_str(), &data);  // tim file dau tien trong path tra ve handle hfind
    if (hfind != INVALID_HANDLE_VALUE) {  //tim thay file
        //in ra cac truong 
        cout << left << setw(25) << "Create Time" << setw(25) << "Modify Time"
             << setw(25) << "File Attributes" << setw(20) << "File Size" << setw(40) << " File Name" << endl;    
        do {
            char CreateTime[20], ModifyTime[20];
            // chuyen doi cau truc FileTime sang SystemTime
            // thoi gian tạo file
            if (FileTimeToSystemTime(&data.ftCreationTime, &systemTime)) {    
                sprintf(CreateTime, "%02d/%02d/%04d %02d:%02d:%02d", systemTime.wDay, systemTime.wMonth,
                        systemTime.wYear, systemTime.wHour, systemTime.wMinute, systemTime.wSecond);
                cout << left << setw(25) << CreateTime;
            }
            //lan cuoi chinh sua file
            if (FileTimeToSystemTime(&data.ftLastWriteTime, &systemTime)) {
                sprintf(ModifyTime, "%02d/%02d/%04d %02d:%02d:%02d", systemTime.wDay, systemTime.wMonth,
                        systemTime.wYear, systemTime.wHour, systemTime.wMinute, systemTime.wSecond);
                cout << setw(25) << ModifyTime;
            }
            //xac dinh co phai thu muc khong
            if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                cout << setw(45) << "<DIR>" << setw(40) << data.cFileName << endl;
                // neu la thu muc && option "/s" thuc hien de quy tiep tuc in ra cac file trong dir
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
        // duyet cac file tiep theo trong path
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
