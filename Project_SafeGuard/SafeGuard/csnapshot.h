#ifndef CSNAPSHOT_H
#define CSNAPSHOT_H

#include <windows.h>
#include <vector>
#include <TlHelp32.h>

using std::vector;

class CSnapShot {
public:
    CSnapShot();
    bool ProcessRoundRobin();
    bool ThreadRoundRobin();
    bool ModuleRoundRobin(DWORD dwPid);
    bool IsAdmin();

public:
    vector<PROCESSENTRY32> m_Process_vec;
    vector<THREADENTRY32> m_Thread_vec;
    vector <MODULEENTRY32> m_Module_vec;
    HANDLE m_hProcess;
    HMODULE* m_pModuleHandleArr;
    DWORD m_dwBuffSize;
};

#endif // CSNAPSHOT_H
