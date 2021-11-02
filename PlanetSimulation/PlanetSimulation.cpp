// PlanetSimulation.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "nlohmann.h"
#include "Bodies.h"
#include "PlanetSimulation.h"
#include <chrono>


// #pragma comment (lib,"Gdiplus.lib")


#define MAX_LOADSTRING 100
#define ID_1SECOND 101


// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
void SetColor(const HDC& hdc);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

Bodies bodies;
auto start_time = std::chrono::high_resolution_clock::now();
bool move = false;
bool pause = false;
bool init = true;


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_PLANETSIMULATION, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PLANETSIMULATION));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
       
    }

    return (int) msg.wParam;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PLANETSIMULATION));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)CreateSolidBrush(0x00000000);  // (HBRUSH)(COLOR_WINDOW+1); // CreateSolidBrush(0x00000000); //
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_PLANETSIMULATION);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    try
    {
        hInst = hInstance; // Store instance handle in our global variable

        HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

        if (!hWnd)
        {
            return FALSE;
        }
        SetTimer(hWnd, ID_1SECOND, 1, NULL);
        auto windowRectangleUniquePtr = std::unique_ptr<tagRECT>(new tagRECT());
        GetClientRect(hWnd, windowRectangleUniquePtr.get());

        bodies.Initialize("..\\planets.json");
        bodies.setScale(windowRectangleUniquePtr->left, windowRectangleUniquePtr->bottom,
            windowRectangleUniquePtr->right, windowRectangleUniquePtr->top);
        bodies.reset();

        // ModifyStyle(0, WS_CLIPCHILDREN, 0);
        ShowWindow(hWnd, SW_MAXIMIZE);
        UpdateWindow(hWnd);
    }
    catch (const std::invalid_argument& ia)
    {
        const char* port = ia.what();
        size_t size = strlen(port) + 1;
        wchar_t* portName = new wchar_t[size];

        size_t outSize;
        mbstowcs_s(&outSize, portName, size, port, size - 1);
        MessageBox(NULL, portName, NULL, MB_OK);
        return FALSE;
    }

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);

                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
      
    case WM_TIMER:
        {
            if (pause)
                return 0;
            move = true;
            InvalidateRect(hWnd, NULL, TRUE);
            UpdateWindow(hWnd);
        }
        break;
    case WM_KEYDOWN:
        if (wParam == VK_SPACE)
        {
            pause = !pause;
        }
        break;
    case WM_CHAR:
        if (wParam == (unsigned int)'r')
        {
            bodies.reset();
            InvalidateRect(hWnd, NULL, TRUE);
            UpdateWindow(hWnd);
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);

            RECT rect;
            GetClientRect(hWnd, &rect);
            // FillRect((HDC)(wParam), &rect, (HBRUSH)GetStockObject(BLACK_BRUSH));

            auto X1 = rect.left;
            auto Y1 = rect.bottom;
            auto X2 = rect.right;
            auto Y2 = rect.top;
            auto X3 = (X1 + X2) / 2;
            auto Y3 = (Y1 + Y2) / 2;

            if (move)
            {
                bodies.erase(hdc, (int)X3, (int)Y3);
                bodies.move();
                move = false;
            }
            bodies.draw(hdc, (int)X3, (int)Y3);

            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

void SetColor(const HDC& hdc)
{
    SelectObject(hdc, GetStockObject(DC_BRUSH));
    SetDCBrushColor(hdc, RGB(255, 0, 0));
    SetDCPenColor(hdc, RGB(255, 0, 0));
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
