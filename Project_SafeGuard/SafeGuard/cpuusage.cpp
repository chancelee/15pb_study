#include "cpuusage.h"
#include <strsafe.h>

CpuUsage::CpuUsage(DWORD dwProcessID)
    : m_nCpuUsage(0),
    m_dwLastRun(0),
    m_lRunCount(0),
    m_dwProcessID(dwProcessID),
    m_ullPrevProcNonIdleTime(0),
    m_ullPrevSysNonIdleTime(0) {
    HANDLE hProcess = GetCurrentProcess();
    SetPrivilege(hProcess, SE_DEBUG_NAME, TRUE);

    m_hProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION, TRUE, m_dwProcessID);
    if (m_hProcess == 0) {
        //ErrorMsg((LPTSTR)TEXT("OpenProcess"));
    }

    ZeroMemory(&m_ftPrevSysKernel, sizeof(FILETIME));
    ZeroMemory(&m_ftPrevSysUser, sizeof(FILETIME));

    ZeroMemory(&m_ftPrevProcKernel, sizeof(FILETIME));
    ZeroMemory(&m_ftPrevProcUser, sizeof(FILETIME));
}

ULONGLONG CpuUsage::SubtractTimes(const FILETIME &ftA, const FILETIME &ftB) {
    LARGE_INTEGER a, b;

    a.LowPart = ftA.dwLowDateTime;
    a.HighPart = ftA.dwHighDateTime;

    b.LowPart = ftB.dwLowDateTime;
    b.HighPart = ftB.dwHighDateTime;

    return a.QuadPart - b.QuadPart;
}

ULONGLONG CpuUsage::AddTimes(const FILETIME &ftA, const FILETIME &ftB) {
    LARGE_INTEGER a, b;

    a.LowPart = ftA.dwLowDateTime;
    a.HighPart = ftA.dwHighDateTime;

    b.LowPart = ftB.dwLowDateTime;
    b.HighPart = ftB.dwHighDateTime;

    return a.QuadPart + b.QuadPart;
}

bool CpuUsage::EnoughTimePassed() {
    const int minElapsedMS = 250;//milliseconds

    ULONGLONG dwCurrentTickCount = GetTickCount();
    return (dwCurrentTickCount - m_dwLastRun) > minElapsedMS;
}

ULONGLONG CpuUsage::GetSystemNonIdleTimes() {
    FILETIME ftSysIdle, ftSysKernel, ftSysUser;

    if (!GetSystemTimes(&ftSysIdle, &ftSysKernel, &ftSysUser)) {
        ErrorMsg((LPTSTR)TEXT("GetSystemTimes"));
        return 0;
    }

    return AddTimes(ftSysKernel, ftSysUser);
}

ULONGLONG CpuUsage::GetProcessNonIdleTimes() {
    FILETIME ftProcCreation, ftProcExit, ftProcKernel, ftProcUser;

    if (!GetProcessTimes(m_hProcess, &ftProcCreation, &ftProcExit, &ftProcKernel, &ftProcUser) && false) {
        ErrorMsg((LPTSTR)TEXT("GetProcessNonIdleTimes"));
        return 0;
    }

    return AddTimes(ftProcKernel, ftProcUser);
}

ULONGLONG CpuUsage::GetUsageEx() {
    ULONGLONG nCpuCopy = m_nCpuUsage;
    if (::InterlockedIncrement(&m_lRunCount) == 1) {
        if (!EnoughTimePassed()) {
            ::InterlockedDecrement(&m_lRunCount);
            return nCpuCopy;
        }

        ULONGLONG ullSysNonIdleTime = GetSystemNonIdleTimes();
        ULONGLONG ullProcNonIdleTime = GetProcessNonIdleTimes();

        if (!IsFirstRun()) {
            ULONGLONG ullTotalSys = ullSysNonIdleTime - m_ullPrevSysNonIdleTime;
            if (ullTotalSys == 0) {
                ::InterlockedDecrement(&m_lRunCount);
                return nCpuCopy;
            }

            m_nCpuUsage = ULONGLONG((ullProcNonIdleTime - m_ullPrevProcNonIdleTime) * 100.0 / (ullTotalSys));
            m_ullPrevSysNonIdleTime = ullSysNonIdleTime;
            m_ullPrevProcNonIdleTime = ullProcNonIdleTime;
        }

        m_dwLastRun = (ULONGLONG)GetTickCount();
        nCpuCopy = m_nCpuUsage;
    }

    ::InterlockedDecrement(&m_lRunCount);

    return nCpuCopy;
}

BOOL CpuUsage::SetPrivilege(HANDLE hProcess, LPCTSTR lpszPrivilege, BOOL bEnablePrivilege) {
    HANDLE hToken;

    if (!OpenProcessToken(hProcess, TOKEN_ADJUST_PRIVILEGES, &hToken)) {
        ErrorMsg((LPTSTR)TEXT("OpenProcessToken"));
        return FALSE;
    }

    LUID luid;

    if (!LookupPrivilegeValue(NULL, lpszPrivilege, &luid)) {
        ErrorMsg((LPTSTR)TEXT("LookupPrivilegeValue"));
        return FALSE;
    }

    TOKEN_PRIVILEGES tkp;

    tkp.PrivilegeCount = 1;
    tkp.Privileges[0].Luid = luid;
    tkp.Privileges[0].Attributes = (bEnablePrivilege) ? SE_PRIVILEGE_ENABLED : FALSE;

    if (!AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof(TOKEN_PRIVILEGES), (PTOKEN_PRIVILEGES)NULL, (PDWORD)NULL)) {
        ErrorMsg((LPTSTR)TEXT("AdjustTokenPrivileges"));
        return FALSE;
    }

    return TRUE;
}

void CpuUsage::ErrorMsg(LPTSTR lpszFunction) {
    // Retrieve the system error message for the last-error code

    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    DWORD dw = GetLastError();

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        LANG_USER_DEFAULT,
        (LPTSTR)&lpMsgBuf,
        0, NULL);

    // Display the error message

    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
        (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));

    StringCchPrintf((LPTSTR)lpDisplayBuf,
        LocalSize(lpDisplayBuf) / sizeof(TCHAR),
        TEXT("%s failed with error %d: %s"),
        lpszFunction, dw, lpMsgBuf);

    MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);
    ExitProcess(dw);
}
