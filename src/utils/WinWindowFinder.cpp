#include "WinWindowFinder.h"

#include <QtWinExtras/qwinfunctions.h>
#include "psapi.h"

WinWindowFinder::WinWindowFinder()
{
}

HWND MacOSWindowFinder::findWindow(QString& title)
{
  HWND hwnd = FindWindowW(NULL, (const wchar_t*)name.utf16());
  if(!hwnd) {
    // Fallback for localized
    HWND unityHwnd = FindWindowW(L"UnityWndClass", NULL);
    if(unityHwnd) {
      DWORD procId;
      GetWindowThreadProcessId(unityHwnd, &procId);

      HANDLE handle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, false, procId);
      TCHAR buffer[MAX_PATH] = {0};
      GetModuleFileNameEx((HMODULE)handle, NULL, buffer, MAX_PATH);
      CloseHandle(handle);

      QString path = QString::fromWCharArray(buffer);
      if(path.contains(name)) {
        hwnd = unityHwnd;
      }
    }
  }
  return hwnd;
}

bool WinWindowFinder::isWindowFocused(HWND hwnd) {
  return GetForegroundWindow() == hwnd;
}
