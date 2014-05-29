#include "systeminfo.h"
#include <QLocale>
#define WINVER 0x0501
#define _WIN32_WINNT 0x0501
#include <windows.h>
#include <stdio.h>

SystemInfo::SystemInfo()
{
    getOSInfo();
    getCPUInfo();
    getGraphicCardInfo();
    getMemorySize();
    getEndian();

    QLocale locale;
    language = locale.languageToString(locale.language());
}

void SystemInfo::getOSInfo() {
    switch(QSysInfo::WindowsVersion) {
    case QSysInfo::WV_32s:
        OSVersion = "Windows 3.1 with Win 32s";
        break;
    case QSysInfo::WV_95:
        OSVersion = "Windows 95";
        break;
    case QSysInfo::WV_98:
        OSVersion = "Windows 98";
        break;
    case QSysInfo::WV_Me:
        OSVersion = "Windows Me";
        break;
    case QSysInfo::WV_NT:
        OSVersion = "Windows NT (operating system version 4.0)";
        break;
    case QSysInfo::WV_2000:
        OSVersion = "Windows 2000 (operating system version 5.0)";
        break;
    case QSysInfo::WV_XP:
        OSVersion = "Windows XP (operating system version 5.1)";
        break;
    case QSysInfo::WV_2003:
        OSVersion = "Windows Server 2003, Windows Server 2003 R2, Windows Home Server, Windows XP Professional x64 Edition (operating system version 5.2)";
        break;
    case QSysInfo::WV_VISTA:
        OSVersion = "Windows Vista, Windows Server 2008 (operating system version 6.0)";
        break;
    case QSysInfo::WV_WINDOWS7:
        OSVersion = "Windows 7, Windows Server 2008 R2 (operating system version 6.1)";
        break;
    case QSysInfo::WV_CE:
        OSVersion = "Windows CE";
        break;
    case QSysInfo::WV_CENET:
        OSVersion = "Windows CE .NET";
        break;
    case QSysInfo::WV_CE_5:
        OSVersion = "Windows CE 5.x";
        break;
    case QSysInfo::WV_CE_6:
        OSVersion = "Windows CE 6.x";
        break;
    case QSysInfo::WV_DOS_based:
        OSVersion = "MS-DOS-based version of Windows";
        break;
    case QSysInfo::WV_NT_based:
        OSVersion = "NT-based version of Windows";
        break;
    case QSysInfo::WV_CE_based:
        OSVersion = "CE-based version of Windows";
        break;
    default:
        OSVersion = "Unknown Windows version";
    }
}

void SystemInfo::getCPUInfo() {
    HKEY hKey;
    DWORD dwDataSize, dwType = 0;
    char *procName;
    long result;

    result = RegOpenKeyExA(HKEY_LOCAL_MACHINE, "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", 0, KEY_READ, &hKey);
    if(result == ERROR_SUCCESS) {
        result = RegQueryValueExA(hKey, "ProcessorNameString", 0, &dwType, NULL, &dwDataSize );
        if(result != ERROR_SUCCESS)
            return;

        procName = (char *)malloc(dwDataSize);
        result = RegQueryValueExA(hKey, "ProcessorNameString", 0, &dwType, (LPBYTE)procName, &dwDataSize );
        if(result != ERROR_SUCCESS)
            return;

        this->cpuinfo = procName;
    }
}

void SystemInfo::getGraphicCardInfo() {
    HKEY topKey;
    HKEY hKey;
    DWORD dwType, dwIndex = 0;
    DWORD dwDataSize = 256;
    char keyName[256];
    char keyEnumName[256];
    char graphicName[256];
    long result;

    result = RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Control\\Video", 0, KEY_ENUMERATE_SUB_KEYS, &topKey);
    if(result == ERROR_SUCCESS) {
        while((result = RegEnumKeyExA(topKey, dwIndex , keyEnumName, &dwDataSize, NULL, NULL, NULL, NULL)) == ERROR_SUCCESS) {
            memset(keyName, 0, sizeof(keyName));
            sprintf(keyName, "SYSTEM\\CurrentControlSet\\Control\\Video\\%s\\0000", keyEnumName);
            result = RegOpenKeyExA(HKEY_LOCAL_MACHINE, keyName, 0, KEY_READ, &hKey);
            if(result == ERROR_SUCCESS) {
                result = RegQueryValueExA(hKey, "DriverDesc", 0, &dwType, (LPBYTE)graphicName, &dwDataSize );
                if(result == ERROR_SUCCESS) {
                    if(!graphicscardinfo.isEmpty())
                        this->graphicscardinfo.append("\n");
                    graphicscardinfo = graphicName;
                }
            }
            dwDataSize = 256;
            memset(keyEnumName, 0, sizeof(keyEnumName));
            dwIndex++;
        }
    }
}

void SystemInfo::getMemorySize() {
    //byte to mb
    int DIV = 1048576;
    MEMORYSTATUSEX statex;

    statex.dwLength = sizeof(statex);
    GlobalMemoryStatusEx(&statex);

    this->memorysize = statex.ullTotalPhys/DIV;
}
