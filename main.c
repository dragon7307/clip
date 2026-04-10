#include <windows.h>
#include <stdio.h>
#include <string.h>

#ifndef VERSION
#define VERSION "1.0.0"
#endif

static int set_clipboard_text(const char* text);

int main(int argc, const char* argv[])
{
    if (argc < 2) 
    {
        printf("Usage: clip <text>\n");
        return 1;
    }

    if(strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-?") == 0) 
    {
        printf("Usage: clip <file_path>\n");
        return 0;
    }
    
    if(strcmp(argv[1], "-v") == 0 || strcmp(argv[1], "--version") == 0) 
    {
        printf("%s\n", VERSION);
        return 0;
    }
    
    const char* text = argv[1];
    
    return set_clipboard_text(text);
}

static int set_clipboard_text(const char* text)
{
    HWND hwnd = GetConsoleWindow();
    
    if(!OpenClipboard(hwnd))
    {
        fprintf(stderr, "Failed to open clipboard\n");
        return 1;
    }
    
    EmptyClipboard();
    
    int len = strlen(text) + 1;
    HGLOBAL clipboardTextHandle = (HGLOBAL) GlobalAlloc(GMEM_MOVEABLE, len);
    
    if(clipboardTextHandle == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for clipboard\n");
        goto close_clipboard;
    }

    LPTSTR clipboardText = (LPTSTR) GlobalLock(clipboardTextHandle);
    
    if(clipboardText == NULL)
    { 
        goto dealloc;
    }

    memcpy(clipboardText, text, len);
    GlobalUnlock(clipboardTextHandle);
    
    if(!SetClipboardData(CF_TEXT, clipboardTextHandle))
    {
        goto dealloc;
    }

    CloseClipboard();
    return 0;
    
    dealloc:
        GlobalFree(clipboardTextHandle);
    close_clipboard:
        CloseClipboard();
    error:
        DWORD error = GetLastError();
        char errorMessage[256];
        FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, error, 0, errorMessage, sizeof(errorMessage), NULL);
        fprintf(stderr, "Error: %s\n", errorMessage);
        return 1;
}