// EverEditLoader.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <shlwapi.h>
#include <shellapi.h>

// 链接必要的库文件
#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "shell32.lib")

// 【关键修改】强制设置为 Windows 子系统，隐藏控制台黑框
// 这样就不需要调用 GetConsoleWindow 了，完美兼容 VC6
#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")

int main() {
    HKEY hKey = NULL;
    LONG lResult = 0;
    char szSubKeyName[256];  // VC6 对 MAX_PATH 支持有时不稳定，直接用数字
    DWORD dwIndex = 0;
    DWORD dwNameLen = 256;
    BOOL bFound = FALSE;

    // 1. 打开注册表项 HKEY_CURRENT_USER\Software\Classes
    lResult = RegOpenKeyEx(
        HKEY_CURRENT_USER, 
        "Software\\Classes", 
        0, 
        KEY_ALL_ACCESS, 
        &hKey
    );

    if (lResult == ERROR_SUCCESS) {
        // 2. 枚举子项，寻找包含 "ZQBK" 的键名
        while (TRUE) {
            dwNameLen = 256;
            lResult = RegEnumKeyEx(
                hKey, 
                dwIndex, 
                szSubKeyName, 
                &dwNameLen, 
                NULL, 
                NULL, 
                NULL, 
                NULL
            );

            if (lResult == ERROR_NO_MORE_ITEMS) {
                break;
            }

            if (lResult == ERROR_SUCCESS) {
                // 检查键名中是否包含 "ZQBK"
                if (strstr(szSubKeyName, "ZQBK") != NULL) {
                    // 3. 构造完整路径并删除
                    char szFullPath[512];
                    sprintf(szFullPath, "Software\\Classes\\%s", szSubKeyName);
                    
                    // SHDeleteKey 递归删除
                    SHDeleteKey(HKEY_CURRENT_USER, szFullPath);
                    
                    bFound = TRUE;
                    break; 
                }
            }
            
            dwIndex++;
        }
        RegCloseKey(hKey);
    }

    // 4. 运行 everedit.exe
    ShellExecute(
        NULL, 
        "open", 
        "everedit.exe", 
        NULL, 
        NULL, 
        SW_SHOWNORMAL
    );

    return 0;
}
