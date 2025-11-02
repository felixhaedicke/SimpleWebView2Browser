#pragma once


struct WebView2Window;

struct WebView2Window* Create_WebView2Window(LPCWSTR windowName, LPCWSTR startUrl);

void WebView2Window_Destroy(struct WebView2Window* webView2Window);
