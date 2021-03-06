#include "Window.h"

using namespace Venus;

namespace Venus::Platform
{
    VEWindow* CreatePlatformWindow()
    {
        return new Window();
    }
}

Window::Window()
    : handle(nullptr)
    , width(0)
    , height(0)
{
}

Window::~Window()
{
}

void Window::Create() {
    if (handle)
        return ;

    // Register the window class.
    WNDCLASSEXW windowClass = { 0 };
    windowClass.cbSize = sizeof(windowClass);
    windowClass.style = CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc = WindowProc;
    windowClass.hInstance = ::GetModuleHandleW(NULL);
    windowClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    windowClass.lpszClassName = WINDOW_CLASS_NAME;

    ::RegisterClassExW(&windowClass);

    // Create the window.
    handle = CreateWindowExW(
        0,                              // Optional window styles.
        WINDOW_CLASS_NAME,                     // Window class
        L"Venus Engine",    // Window text
        WS_OVERLAPPEDWINDOW,            // Window style
        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        NULL,       // Parent window    
        NULL,       // Menu
        ::GetModuleHandleW(NULL),  // Instance handle
        NULL        // Additional application data
    );

    if (!handle)
        throw std::exception("Failed Create Window");

    // https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-setwindowlongptrw
    ::SetLastError(0);
    if (!::SetWindowLongPtrW(handle, GWLP_USERDATA, (LONG_PTR)this))
    {
        if (::GetLastError())
            throw std::exception("Failed SetWindowLongPtrW");
    }

    ::ShowWindow(handle, 1);
}

void Window::Destroy()
{
    if (handle)
    {
        ::PostMessageW(handle, WM_CLOSE, 0, 0);
        handle = NULL;
    }
}

void Window::Show()
{
    ::ShowWindow(handle, SW_SHOWNORMAL);
}

void Window::Hide()
{
    ::ShowWindow(handle, SW_HIDE);
}

float Window::AspectRatio() const
{
    return static_cast<float>(width) / static_cast<float>(height);
}

LRESULT Window::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    Window* window = (Window*)::GetWindowLongPtrW(hWnd, GWLP_USERDATA);
    if (window)
    {
        switch (message)
        {
        case WM_SIZE:
        {
            uint32_t width = LOWORD(lParam);
            uint32_t height = HIWORD(lParam);
            if (window->width != width || window->height != height)
            {
                window->width = width;
                window->height = height;
            }
            return 0;
        }

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        }
    }

    // Handle any messages the switch statement didn't.
    return ::DefWindowProcW(hWnd, message, wParam, lParam);
}