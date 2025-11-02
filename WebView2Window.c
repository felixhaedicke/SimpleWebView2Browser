#include <windows.h>
#include <shlwapi.h>

#include <assert.h>

#include <WebView2.h>

#include "WebView2Window.h"

#include "WebView2EnvOptions.h"
#include "CreateWebView2CtrlCompletedHandler.h"
#include "CreateWebView2EnvCompletedHandler.h"


static const WCHAR CLASS_NAME[] = L"WebView2 Window Class Name";

static BOOL g_WindowClassRegistered = FALSE;

struct WebView2Window
{
    LPWSTR startUrl;

    HWND window;

    ICoreWebView2EnvironmentOptions* webView2Options;
    ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler* webView2EnvCompletedHandler;
    ICoreWebView2CreateCoreWebView2ControllerCompletedHandler* webView2CtrlCompletedHandler;

    ICoreWebView2Environment* webView2Env;
    ICoreWebView2Controller* webView2Ctrl;
    ICoreWebView2* webView2;
};

static void
WebView2Window_RefreshBounds(struct WebView2Window* const this)
{
    assert(NULL != this);

    if (NULL != this->webView2Ctrl)
    {
        RECT rect;
        const BOOL status = GetWindowRect(this->window, &rect);
        assert(FALSE != status);
        rect.right -= rect.left;
        rect.bottom -= rect.top;
        rect.left = 0;
        rect.top = 0;
        ICoreWebView2Controller_SetBoundsAndZoomFactor(this->webView2Ctrl, rect, 1.0);
    }
}

static LRESULT CALLBACK
WebView2Window_WndProc(
    const HWND hWnd,
    const UINT message,
    const WPARAM wParam,
    const LPARAM lParam)
{
    switch (message)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_SIZE:
        WebView2Window_RefreshBounds((struct WebView2Window*)GetWindowLongPtr(hWnd, GWLP_USERDATA));
        break;
    case WM_ERASEBKGND:
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

static void
WebView2Window_HandleControllerCreated(const HRESULT errorCode, ICoreWebView2Controller* const ctrl, void* ctx)
{
    assert(S_OK == errorCode);
    assert(NULL != ctrl);
    assert(NULL != ctx);

    struct WebView2Window* const this = (struct WebView2Window*)ctx;

    assert(NULL == this->webView2Ctrl);
    this->webView2Ctrl = ctrl;
    ICoreWebView2Controller_AddRef(ctrl);

    WebView2Window_RefreshBounds(this);
    assert(NULL == this->webView2);
    ICoreWebView2Controller_get_CoreWebView2(ctrl, &this->webView2);

    assert(NULL != this->startUrl);
    const HRESULT navigateStatus = ICoreWebView2_Navigate(this->webView2, this->startUrl);
    assert(S_OK == navigateStatus);
}

static void
WebView2Window_HandleEnvCreated(const HRESULT errorCode, ICoreWebView2Environment* const env, void* const ctx)
{
    assert(S_OK == errorCode);
    assert(NULL != env);
    assert(NULL != ctx);

    struct WebView2Window* const this = (struct WebView2Window*)ctx;

    this->webView2Env = env;
    ICoreWebView2Environment_AddRef(env);

    const HRESULT createControllerStatus
        = ICoreWebView2Environment_CreateCoreWebView2Controller(
            this->webView2Env,
            this->window,
            this->webView2CtrlCompletedHandler);
    assert(S_OK == createControllerStatus);
}

static void
RegisterWindowClass()
{
    WNDCLASSEXW wcex;
    memset(&wcex, 0, sizeof(wcex));
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WebView2Window_WndProc;
    wcex.lpszClassName = CLASS_NAME;

    const ATOM registerStatus = RegisterClassExW(&wcex);
    assert(0 != registerStatus);
}

static void
WebView2Window_InitWindow(struct WebView2Window* const this, LPCWSTR const windowName)
{
    this->window = CreateWindowW(
        CLASS_NAME,
        windowName,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        0,
        CW_USEDEFAULT,
        0,
        NULL,
        NULL,
        GetModuleHandle(NULL),
        NULL);
    assert(NULL != this->window);

    SetWindowLongPtr(this->window, GWLP_USERDATA, (LONG_PTR)this);
    ShowWindow(this->window, SW_SHOW);
    UpdateWindow(this->window);
}

static void
WebView2Window_InitWebView2(struct WebView2Window* const this)
{
    this->webView2Options = New_WebView2EnvOptions();
    this->webView2EnvCompletedHandler
        = New_CreateWebView2EnvCompletedHandler(WebView2Window_HandleEnvCreated, this);
    this->webView2CtrlCompletedHandler
        = New_CreateWebView2CtrlCompletedHandler(WebView2Window_HandleControllerCreated, this);

    const HRESULT createEnvStatus = CreateCoreWebView2EnvironmentWithOptions(
        NULL,
        NULL,
        this->webView2Options,
        this->webView2EnvCompletedHandler);
    assert(S_OK == createEnvStatus);
}

struct WebView2Window*
Create_WebView2Window(LPCWSTR const windowName, LPCWSTR const startUrl)
{
    assert(NULL != windowName);
    assert(NULL != startUrl);

    struct WebView2Window* const this = LocalAlloc(LMEM_FIXED | LMEM_ZEROINIT, sizeof(struct WebView2Window));
    assert(NULL != this);

    this->startUrl = StrDupW(startUrl);

	if (TRUE != g_WindowClassRegistered)
	{
        RegisterWindowClass();
		g_WindowClassRegistered = TRUE;
	}

    WebView2Window_InitWindow(this, windowName);
    WebView2Window_InitWebView2(this);

    return this;
}

void WebView2Window_Destroy(struct WebView2Window* webView2Window)
{
    ICoreWebView2Controller_Release(webView2Window->webView2Ctrl);
    ICoreWebView2Environment_Release(webView2Window->webView2Env);
    ICoreWebView2_Release(webView2Window->webView2);

    ICoreWebView2EnvironmentOptions_Release(webView2Window->webView2Options);
    ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler_Release(webView2Window->webView2EnvCompletedHandler);
    ICoreWebView2CreateCoreWebView2ControllerCompletedHandler_Release(webView2Window->webView2CtrlCompletedHandler);

    DestroyWindow(webView2Window->window);

    LocalFree(webView2Window->startUrl);
    LocalFree(webView2Window);
}
