#include "csnapshot.h"
#include "cpuusage.h"
#include <Psapi.h>
#include <Shlobj.h>

CSnapShot::CSnapShot(): m_pModuleHandleArr(nullptr) {

}

bool CSnapShot::ProcessRoundRobin(){
    m_Process_vec.clear();

    HANDLE hsnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hsnap == INVALID_HANDLE_VALUE) {
        return false;
    }

    PROCESSENTRY32 lEntry;
    lEntry.dwSize = sizeof(lEntry);

    if (!Process32First(hsnap, &lEntry)) {
        CloseHandle(hsnap);
        return false;
    }

    do {
        m_Process_vec.push_back(lEntry);
    } while (Process32Next(hsnap, &lEntry));

    CloseHandle(hsnap);

    return true;
}

bool CSnapShot::ThreadRoundRobin() {
    m_Thread_vec.clear();

    HANDLE hsnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (hsnap == INVALID_HANDLE_VALUE) {
        return false;
    }

    THREADENTRY32 lEntry;
    lEntry.dwSize = sizeof(lEntry);

    if (!Thread32First(hsnap, &lEntry)) {
        printf("Thread32First Failed\n");
        CloseHandle(hsnap);
        return false;
    }

    do {
        m_Thread_vec.push_back(lEntry);
//        if (lEntry.th32OwnerProcessID == dwPid) {
//            m_Thread_vec.push_back(lEntry);
//        }
    } while (Thread32Next(hsnap, &lEntry));

    CloseHandle(hsnap);

    return true;
}

bool CSnapShot::ModuleRoundRobin(DWORD dwPid) {
#if 0
    DWORD dwBuffSize = 0;
    HANDLE hProcess = GetCurrentProcess();

    WCHAR szApplication[MAX_PATH] = { 0 };
    DWORD cchLength = _countof(szApplication);
    QueryFullProcessImageName(GetCurrentProcess(), 0,
        szApplication, &cchLength);

    SHELLEXECUTEINFO sei = { sizeof(SHELLEXECUTEINFO) };
    sei.lpVerb = L"runas"; // 提升权限需要用到这个runas
    sei.lpFile = szApplication;
    sei.lpParameters = NULL;
    sei.nShow = SW_SHOWNORMAL;

    // 必须使用ShellExecuteEx
    if (!IsAdmin()) {
        if (ShellExecuteEx(&sei)) {
            ExitProcess(0);
        }
    }

    m_hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dwPid);
    //m_hProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION, TRUE, dwPid);
    if (!m_hProcess)
        return false;

    EnumProcessModulesEx(m_hProcess, NULL, 0, &dwBuffSize, LIST_MODULES_ALL);

    if (m_pModuleHandleArr) {
        delete[] m_pModuleHandleArr;
        m_dwBuffSize = 0;
    }

    m_pModuleHandleArr = (HMODULE*)new char [dwBuffSize];
    EnumProcessModulesEx(m_hProcess, m_pModuleHandleArr, dwBuffSize, &dwBuffSize, LIST_MODULES_ALL);
    m_dwBuffSize = dwBuffSize;

    return true;
#else
    m_Module_vec.clear();

    HANDLE hsnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPid);
    if (hsnap == INVALID_HANDLE_VALUE) {
        return false;
    }

    MODULEENTRY32 lEntry;
    lEntry.dwSize = sizeof(lEntry);

    if (!Module32First(hsnap, &lEntry)) {
        CloseHandle(hsnap);
        return false;
    }

    do {
        m_Module_vec.push_back(lEntry);
    } while (Module32Next(hsnap, &lEntry));

    CloseHandle(hsnap);

    return true;
#endif
}

bool CSnapShot::IsAdmin() {
    HANDLE hToken = NULL;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
        return FALSE;

    TOKEN_ELEVATION_TYPE ElevationType = TokenElevationTypeDefault;
    BOOL bIsAdmin = false;
    DWORD dwSize = 0;

    if (GetTokenInformation(hToken, TokenElevationType, &ElevationType,
        sizeof(TOKEN_ELEVATION_TYPE), &dwSize)) {
        BYTE adminSID[SECURITY_MAX_SID_SIZE];

        dwSize = sizeof(adminSID);
        CreateWellKnownSid(WinBuiltinAdministratorsSid, NULL, &adminSID, &dwSize);

        if (ElevationType == TokenElevationTypeLimited) {
            HANDLE hUnfilteredToken = NULL;

            GetTokenInformation(hToken, TokenLinkedToken, (PVOID)&hUnfilteredToken,
                sizeof(HANDLE), &dwSize);

            if (!CheckTokenMembership(hUnfilteredToken, &adminSID, &bIsAdmin))
                return false;

            CloseHandle(hUnfilteredToken);
        } else {
            bIsAdmin = IsUserAnAdmin();
        }

        CloseHandle(hToken);
    }

    bool bFullToken = false;

    switch (ElevationType) {
    case TokenElevationTypeDefault:
    case TokenElevationTypeFull:
        if (IsUserAnAdmin())
            bFullToken = true;
        else
            bFullToken = false;
        break;
    case TokenElevationTypeLimited:
        bFullToken = false;
        break;
    default:
        break;
    }

    return bFullToken;
}
