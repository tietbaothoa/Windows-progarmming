#include <iostream>
#include<Windows.h>
#include<tlhelp32.h>
#include <winbase.h>
#include <psapi.h>
#include <iomanip>
//#include <processthreadsapi.h>
using namespace std;
#define faster ios_base::sync_with_stdio(0); cin.tie(0); cout.tie(0)

void FindParentProcessName(DWORD PID){
    HANDLE hSnapshot;
    PROCESSENTRY32 data;
    // tao snapshot cac process dang chay tai thoi diem hien tai
    hSnapshot = CreateToolhelp32Snapshot(0x2, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        cout << "Get handle of snapshot's handle fail when find parent process name";
        return;
    }
    data.dwSize = sizeof(PROCESSENTRY32);
    //Tim process dau tien trong snapshot
    if (Process32First(hSnapshot, &data)) {
        do{
            //neu pid cua process hien tai == process cua process cha -> in ra ten process cha va tra ve
            if(data.th32ProcessID == PID){
                cout << data.szExeFile << endl;
                return;
            }
        // tim process tiep theo trong snapshot
        }while(Process32Next(hSnapshot, &data));
    }
    // neu trong snapshot khong tim thay process nao
    cout << "NOT FOUND" << endl;
}

int main() {
    HANDLE hSnapshot;
    PROCESSENTRY32 data;
    hSnapshot = CreateToolhelp32Snapshot(0x2, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        cout << "Get handle of snapshot's handle fail";
        return 0;
    }
    data.dwSize = sizeof(PROCESSENTRY32);
    // tao snapshot cac process dang chay tai thoi diem hien tai
    if (Process32First(hSnapshot, &data)) {
        cout << left << setw(10) << "PID" << setw(30) << "Process Name" << setw(40) << "PPID" << setw(40) << "Parent Process Name" << endl;
        // tim process tiep theo trong snapshot
        while (Process32Next(hSnapshot, &data)) {
            // in ra PID, ten file, parent PID
            cout << left << setw(10) << data.th32ProcessID << setw(30) << data.szExeFile << setw(40) << data.th32ParentProcessID << setw(40);
            // goi ham tim ten process cha bang cach so sanh pid cua process cha với pid cua cac process ghi lai trong snapshot
            FindParentProcessName(data.th32ParentProcessID);
    }
    CloseHandle(hSnapshot);
    return 0;
}
