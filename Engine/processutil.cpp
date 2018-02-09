#include "processutil.h"

#ifdef _WIN32
#include <windows.h>
#include <psapi.h>
#include <process.h>
#include <Tlhelp32.h>
#endif



namespace defender_engine
{

#ifdef _WIN32
    // Obtains elevated rights for retrieving information
    // about running processes.
    class AccessPrivilege
    {
    public:
        AccessPrivilege() = default;

        void tryObtainPrivilege()
        {
            if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &m_hMyToken))
            {
               SetPrivilege(m_hMyToken, SE_DEBUG_NAME, TRUE);
            }
        }

        ~AccessPrivilege()
        {
            if (m_hMyToken)
                SetPrivilege(m_hMyToken, SE_DEBUG_NAME, FALSE);
        }

    private:
        bool SetPrivilege(HANDLE hToken, LPCTSTR lpszPrivilege, BOOL bEnablePrivilege);

    private:
        HANDLE m_hMyToken{ 0 };
    };

    bool AccessPrivilege::SetPrivilege(HANDLE hToken, LPCTSTR lpszPrivilege, BOOL bEnablePrivilege)
    {
        TOKEN_PRIVILEGES tp;
        LUID luid;

        if (!LookupPrivilegeValue(
            NULL,           // lookup privilege on local system
            lpszPrivilege,  // privilege to lookup
            &luid))         // receives LUID of privilege
        {
            return false;
        }

        tp.PrivilegeCount = 1;
        tp.Privileges[0].Luid = luid;
        if (bEnablePrivilege)
            tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
        else
            tp.Privileges[0].Attributes = 0;

        // Enable the privilege or disable all privileges.
        if (!AdjustTokenPrivileges(
            hToken,
            FALSE,
            &tp,
            sizeof(TOKEN_PRIVILEGES),
            (PTOKEN_PRIVILEGES)NULL,
            (PDWORD)NULL))
        {
            return false;
        }
        if (GetLastError() == ERROR_NOT_ALL_ASSIGNED)
        {
            return false;
        }

        return true;
    }

    std::wstring GetLastErrorStdStr()
    {
      DWORD error = GetLastError();
      if (error)
      {
        LPVOID lpMsgBuf;
        DWORD bufLen = FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            error,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR) &lpMsgBuf,
            0, NULL );
        if (bufLen)
        {
          LPCTSTR lpMsgStr = (LPCTSTR)lpMsgBuf;
          std::wstring result(lpMsgStr, lpMsgStr+bufLen);

          LocalFree(lpMsgBuf);

          return result;
        }
      }
      return std::wstring();
    }
#endif

ProcessUtil::ProcessUtil() :
    mPrivilege(std::unique_ptr<AccessPrivilege>(new AccessPrivilege))
{
}

ProcessesList ProcessUtil::getAllRunningProcess() const
{
    ProcessesList result;
#ifdef _WIN32
    if (!accessPrivilage)
    {
         mPrivilege->tryObtainPrivilege();
         accessPrivilage = true;
    }

    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE)
        return result;

    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);
    if (!Process32First(hProcessSnap, &pe32))
    {
        return result;
    }
    do
    {
        HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pe32.th32ProcessID);
        if (hProcess != NULL)
        {
            ProcessInfo pi;
            pi.name = QString::fromStdWString(pe32.szExeFile);
            pi.pid = pe32.th32ProcessID;
            result.push_back(pi);
            CloseHandle(hProcess);
        }
    } while (Process32Next(hProcessSnap, &pe32));

    CloseHandle(hProcessSnap);
#endif
    return result;
}

void ProcessUtil::killProcess(const QString& name) const
{
#ifdef _WIN32
    if (!accessPrivilage)
    {
         mPrivilege->tryObtainPrivilege();
         accessPrivilage = true;
    }

    HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);
    PROCESSENTRY32 pEntry;
    pEntry.dwSize = sizeof (pEntry);
    BOOL hRes = Process32First(hSnapShot, &pEntry);
    while (hRes)
    {
        if (QString::fromWCharArray(pEntry.szExeFile) == name)
        {
            HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, 0,
                                          (DWORD) pEntry.th32ProcessID);
            if (hProcess != NULL)
            {
                TerminateProcess(hProcess, 9);
                CloseHandle(hProcess);
            }
        }
        hRes = Process32Next(hSnapShot, &pEntry);
    }
    CloseHandle(hSnapShot);
#endif
}

bool ProcessUtil::killProcessById(long pid, QString& errorString) const
{
#ifdef _WIN32
    if (!accessPrivilage)
    {
         mPrivilege->tryObtainPrivilege();
         accessPrivilage = true;
    }

    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, 0, (DWORD)pid);
    if (hProcess != NULL)
    {
        TerminateProcess(hProcess, 9);
        CloseHandle(hProcess);
    }
    else
    {
        errorString = QString::fromStdWString(GetLastErrorStdStr());
        return false;
    }
    return true;
#endif
}

ProcessUtil::~ProcessUtil()
{
}

}
