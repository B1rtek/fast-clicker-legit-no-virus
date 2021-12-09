#include <windows.h>
#include <string>
#include <thread>
#include <chrono>

#pragma comment(linker, "\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

using namespace std;
using namespace std::literals;

int windowWidth = 200, windowHeight = 120;

LRESULT CALLBACK WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

MSG message;

HWND hwnd;
HWND hSpeedBox, hTitle, hToggle, hSpeed, hActive, hPs;

unsigned clickSpeed = 10;
bool clicking = false;
bool prevHotkeyDown = false;

auto globalClock = chrono::steady_clock::now();

void click() {
    mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
    mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
}

[[noreturn]] void clicker() {
    while (true) {
        bool hotkeyDown = GetAsyncKeyState(VK_INSERT);
        if (hotkeyDown && !prevHotkeyDown) {
            if (clicking) {
                clicking = false;
                SetWindowTextA(hActive, "Inactive");
            } else {
                clicking = true;
                SetWindowTextA(hActive, "ACTIVE");
            }
        }
        prevHotkeyDown = hotkeyDown;
        if (clickSpeed <= 0) {
            globalClock += 10ms;
            this_thread::sleep_until(globalClock);
            continue;
        } else {
            if (clicking) {
                unsigned clickTime = 1000000 / clickSpeed;
                globalClock += chrono::microseconds(clickTime);
                click();
                this_thread::sleep_until(globalClock);
            } else {
                globalClock += 10ms;
                this_thread::sleep_until(globalClock);
            }
        }
    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    string programName = "FastClickerLegitNoVirus";
    auto className = const_cast<LPSTR>(programName.c_str());
    WNDCLASSEXA window;
    window.cbSize = sizeof(WNDCLASSEX);
    window.style = 0;
    window.lpfnWndProc = WindowProc;
    window.cbClsExtra = 0;
    window.cbWndExtra = 0;
    window.hInstance = hInstance;
    window.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    window.hCursor = LoadCursor(nullptr, IDC_ARROW);
    window.hbrBackground = (HBRUSH) (COLOR_WINDOW);
    window.lpszMenuName = nullptr;
    window.lpszClassName = className;
    window.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);

    if (!RegisterClassExA(&window)) {
        MessageBoxA(nullptr, "Failed registering main window!", "Error", MB_OK | MB_ICONSTOP | MB_SYSTEMMODAL);
        return 1;
    }

    hwnd = CreateWindowExA(WS_EX_WINDOWEDGE | WS_EX_TOPMOST, className, className, WS_OVERLAPPEDWINDOW,
                           GetSystemMetrics(SM_CXSCREEN) / 2 - windowWidth / 2,
                           GetSystemMetrics(SM_CYSCREEN) / 2 - windowHeight / 2, windowWidth, windowHeight, nullptr,
                           nullptr, hInstance, nullptr);
    if (hwnd == nullptr) {
        MessageBoxA(nullptr, "Failed creating main window!", "Error", MB_OK | MB_ICONSTOP | MB_SYSTEMMODAL);
        return 2;
    }

    //controls

    auto hNormalFont = (HFONT) GetStockObject(DEFAULT_GUI_FONT);

    hTitle = CreateWindowExA(0, "STATIC", "Fast Clicker Legit No Virus (TM)", WS_CHILD | WS_VISIBLE, 16, 10, 160,
                             20, hwnd, nullptr, nullptr, nullptr);
    SendMessageA(hTitle, WM_SETFONT, (WPARAM) hNormalFont, 0);

    hToggle = CreateWindowExA(0, "STATIC", "Press INS to toggle", WS_CHILD | WS_VISIBLE, 20, 32, 100,
                              20, hwnd, nullptr, nullptr, nullptr);
    SendMessageA(hToggle, WM_SETFONT, (WPARAM) hNormalFont, 0);

    hActive = CreateWindowExA(0, "STATIC", "Inactive", WS_CHILD | WS_VISIBLE, 120, 32, 80,
                              20, hwnd, nullptr, nullptr, nullptr);
    SendMessageA(hActive, WM_SETFONT, (WPARAM) hNormalFont, 0);

    hSpeed = CreateWindowExA(0, "STATIC", "Speed:", WS_CHILD | WS_VISIBLE, 40, 57, 40,
                             20, hwnd, nullptr, nullptr, nullptr);
    SendMessageA(hSpeed, WM_SETFONT, (WPARAM) hNormalFont, 0);

    hPs = CreateWindowExA(0, "STATIC", "cps", WS_CHILD | WS_VISIBLE, 145, 57, 30,
                          20, hwnd, nullptr, nullptr, nullptr);
    SendMessageA(hPs, WM_SETFONT, (WPARAM) hNormalFont, 0);

    hSpeedBox = CreateWindowExA(WS_EX_CLIENTEDGE, "EDIT", nullptr, WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL, 80, 55, 60,
                                20, hwnd, nullptr, nullptr, nullptr);
    SendMessageA(hSpeedBox, WM_SETFONT, (WPARAM) hNormalFont, 0);
    SetWindowTextA(hSpeedBox, to_string(clickSpeed).c_str());

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    thread clickThread(clicker);
    clickThread.detach();
    globalClock = chrono::steady_clock::now();

    while (GetMessageA(&message, nullptr, 0, 0)) {
        TranslateMessage(&message);
        DispatchMessageA(&message);
    }
    return message.wParam; // NOLINT(cppcoreguidelines-narrowing-conversions)
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CLOSE: {
            DestroyWindow(hWnd);
            break;
        }
        case WM_DESTROY: {
            PostQuitMessage(0);
            break;
        }
        case WM_COMMAND: {
            if ((HWND) lParam == hSpeedBox) {
                DWORD len = GetWindowTextLengthA(hSpeedBox);
                auto buffer = (LPSTR) GlobalAlloc(GPTR, len + 1);
                GetWindowTextA(hSpeedBox, buffer, int(len) + 1);
                string speed = buffer;
                unsigned clkSpeed;
                try {
                    clkSpeed = stoul(buffer);
                } catch (const std::exception& e) {
                    clkSpeed = 0;
                }
                clickSpeed = clkSpeed;
                GlobalFree(buffer);
            }
            break;
        }
        default: {
            return DefWindowProcA(hWnd, msg, wParam, lParam);
        }
    }
    return DefWindowProcA(hWnd, msg, wParam, lParam);
}