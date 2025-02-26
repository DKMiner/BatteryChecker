#include <Windows.h>
#include <mmsystem.h>
#include <shellapi.h>

HWND notificationWindow = NULL;

VOID CALLBACK TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) {
    // Check if the charger is connected
    SYSTEM_POWER_STATUS powerStatus;
    GetSystemPowerStatus(&powerStatus);
    if (powerStatus.ACLineStatus != 1) {
        // Play the sound if the charger is disconnected
        PlaySound("C:/Windows/Media/chimes.wav", NULL, SND_FILENAME | SND_ASYNC);

        // Check if the notification window is already open
        if (notificationWindow == NULL) {
            // Create a new notification window (invisible)
            notificationWindow = CreateWindowEx(0, "STATIC", "", WS_OVERLAPPEDWINDOW, 0, 0, 0, 0, NULL, NULL, NULL, NULL);
            SetWindowLong(notificationWindow, GWL_STYLE, GetWindowLong(notificationWindow, GWL_STYLE) & ~WS_SYSMENU);
        }

        // Show the MessageBox every time the charger is disconnected
        MessageBox(notificationWindow, "Charger disconnected!", "Charger Notifier", MB_OK | MB_TOPMOST); // | MB_ICONEXCLAMATION);
    } else {
        // Charger is connected, close the notification window
        if (notificationWindow != NULL) {
            DestroyWindow(notificationWindow);
            notificationWindow = NULL;
        }
    }
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CLOSE:
        // Minimize to system tray
        NOTIFYICONDATA trayIcon;
        trayIcon.cbSize = sizeof(NOTIFYICONDATA);
        trayIcon.hWnd = hwnd;
        trayIcon.uID = 1;
        trayIcon.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
        trayIcon.uCallbackMessage = WM_USER;
        trayIcon.hIcon = LoadIcon(NULL, IDI_APPLICATION);
        strcpy(trayIcon.szTip, "Charger Notifier");
        Shell_NotifyIcon(NIM_ADD, &trayIcon);
        break;
    case WM_USER:
        // Handle tray icon messages
        switch (lParam) {
            case WM_RBUTTONDOWN:
            // Show context menu
            POINT pt;
            GetCursorPos(&pt);
            HMENU hMenu = CreatePopupMenu();
            AppendMenu(hMenu, MF_STRING, 1, "Quit");
            SetForegroundWindow(hwnd); // Add this line
            TrackPopupMenu(hMenu, TPM_BOTTOMALIGN | TPM_LEFTALIGN, pt.x, pt.y, 0, hwnd, NULL);
            PostMessage(hwnd, WM_NULL, 0, 0); // Add this line
            break;
        }
        break;
    case WM_CONTEXTMENU:
        // Destroy context menu if it's still visible
        DestroyMenu((HMENU)lParam);
        break;
    case WM_NULL:
        // Destroy context menu if it's still visible
        {
            HMENU hMenu = GetMenu(hwnd);
            if (hMenu != NULL) {
                DestroyMenu(hMenu);
            }
        }
        break;
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

int main() {
    // Remove the console window
    FreeConsole();

    // Register the window class
    WNDCLASSEX wc = {0};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = 0;
    wc.lpfnWndProc = WindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = GetModuleHandle(NULL);
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.lpszClassName = "ChargerNotifier";
    RegisterClassEx(&wc);

    // Create the window
    HWND hwnd = CreateWindowEx(0, "ChargerNotifier", "", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, GetModuleHandle(NULL), NULL);

    // Initialize the sound player
    PlaySound(NULL, NULL, SND_PURGE);

    // Create a timer to check the charger status
    SetTimer(hwnd, 1, 1500, (TIMERPROC)TimerProc);

    // Minimize to system tray
    NOTIFYICONDATA trayIcon;
    trayIcon.cbSize = sizeof(NOTIFYICONDATA);
    trayIcon.hWnd = hwnd;
    trayIcon.uID = 1;
    trayIcon.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    trayIcon.uCallbackMessage = WM_USER;
    trayIcon.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    strcpy(trayIcon.szTip, "Charger Notifier");
    Shell_NotifyIcon(NIM_ADD, &trayIcon);

    // Run the message loop
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        if (msg.message == WM_COMMAND && msg.wParam == 1) {
            // Quit the application
            PostQuitMessage(0);
        }
        if (msg.message == WM_CONTEXTMENU) {
            // Destroy context menu
            HMENU hMenu = (HMENU)msg.lParam;
            if (hMenu != NULL) {
                DestroyMenu(hMenu);
            }
        }
    }

    return 0;
}