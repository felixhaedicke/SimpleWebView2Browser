/*
MIT License

Copyright (c) 2025 Felix Hädicke <felixhaedicke@web.de>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <windows.h>

#include <assert.h>

#include "WebView2Window.h"


const WCHAR DEFAULT_URL[] = L"https://duckduckgo.com";
const WCHAR WINDOW_NAME[] = L"WebView2 Browser";

int APIENTRY wWinMain(
    _In_ const HINSTANCE hInstance,
    _In_opt_ const HINSTANCE hPrevInstance,
    _In_ LPWSTR const lpCmdLine,
    _In_ const int nCmdShow)
{
    SetProcessDPIAware();
    HRESULT status = CoInitialize(NULL);
    assert(S_OK == status);

    int argc;
    LPWSTR* const args = CommandLineToArgvW(GetCommandLineW(), &argc);
    LPCWSTR url = (argc > 1) ? args[1] : DEFAULT_URL;
    struct WebView2Window* webView2Window = Create_WebView2Window(WINDOW_NAME, url);
    LocalFree(args);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    WebView2Window_Destroy(webView2Window);

    return (int)msg.wParam;
}
