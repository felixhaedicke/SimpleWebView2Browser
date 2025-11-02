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
#include <shlwapi.h>

#include <assert.h>

#include "WebView2EnvOptions.h"


static const WCHAR DEFAULT_TARGET_COMPATIBLE_BROWSER_VERSION[] = L"118.0.2088.76";


struct WebView2EnvOptions
{
    ICoreWebView2EnvironmentOptions ICoreWebView2EnvironmentOptions_iface;
    ICoreWebView2EnvironmentOptionsVtbl vtbl;
    LONG ref;
    LPWSTR additionalBrowserArguments;
    LPWSTR language;
    LPWSTR targetCompatibleBrowserVersion;
    BOOL allowSingleSignOnUsingOSPrimaryAccount;
};

static void
Destroy_WebView2EnvOptions(struct WebView2EnvOptions* const this)
{
    LocalFree(this->additionalBrowserArguments);
    LocalFree(this->language);
    LocalFree(this->targetCompatibleBrowserVersion);
    LocalFree(this);
}

static inline struct WebView2EnvOptions*
GetImpl(ICoreWebView2EnvironmentOptions* const iface)
{
    return CONTAINING_RECORD(iface, struct WebView2EnvOptions, ICoreWebView2EnvironmentOptions_iface);
}

static HRESULT WINAPI WebView2EnvOptions_QueryInterface(
    ICoreWebView2EnvironmentOptions* const iface,
    REFIID riid,
    void** const ret)
{
    if (!IsEqualIID(riid, &IID_ICoreWebView2EnvironmentOptions)
        && !IsEqualIID(riid, &IID_IUnknown))
    {
        *ret = 0;
        return E_NOINTERFACE;
    }

    *ret = iface;
    ICoreWebView2EnvironmentOptions_AddRef(iface);
    return S_OK;
}


static ULONG WINAPI
WebView2EnvOptions_AddRef(ICoreWebView2EnvironmentOptions* const iface)
{
    struct WebView2EnvOptions* const this = GetImpl(iface);
    const ULONG refcount = InterlockedIncrement(&this->ref);
    return refcount;
}

static ULONG WINAPI
WebView2EnvOptions_Release(ICoreWebView2EnvironmentOptions* const iface)
{
    struct WebView2EnvOptions* const this = GetImpl(iface);
    const ULONG refcount = InterlockedDecrement(&this->ref);

    if (0 == refcount) Destroy_WebView2EnvOptions(this);
    return refcount;
}

static HRESULT WINAPI
WebView2EnvOptions_get_AdditionalBrowserArguments(
    ICoreWebView2EnvironmentOptions* const iface,
    LPWSTR* const outValue)
{
    const struct WebView2EnvOptions* const this = GetImpl(iface);
    return SHStrDupW(this->additionalBrowserArguments, outValue);
}

static HRESULT WINAPI
WebView2EnvOptions_put_AdditionalBrowserArguments(
    ICoreWebView2EnvironmentOptions* iface,
    LPCWSTR const value)
{
    struct WebView2EnvOptions* const this = GetImpl(iface);
    LocalFree(this->additionalBrowserArguments);
    this->additionalBrowserArguments = (NULL == value) ? NULL : StrDupW(value);
    return S_OK;
}

static HRESULT WINAPI
WebView2EnvOptions_get_Language(
    ICoreWebView2EnvironmentOptions* const iface,
    LPWSTR* const outValue)
{
    const struct WebView2EnvOptions* const this = GetImpl(iface);
    return SHStrDupW(this->language, outValue);
}

static HRESULT WINAPI
WebView2EnvOptions_put_Language(
    ICoreWebView2EnvironmentOptions* const iface,
    LPCWSTR const value)
{
    struct WebView2EnvOptions* const this = GetImpl(iface);
    LocalFree(this->language);
    this->language = (NULL == value) ? NULL : StrDupW(value);
    return S_OK;
}

static HRESULT WINAPI
WebView2EnvOptions_get_TargetCompatibleBrowserVersion(
    ICoreWebView2EnvironmentOptions* const iface,
    LPWSTR* const outValue)
{
    const struct WebView2EnvOptions* const this = GetImpl(iface);
    return SHStrDupW(
        (NULL == this->targetCompatibleBrowserVersion)
            ? DEFAULT_TARGET_COMPATIBLE_BROWSER_VERSION
            : this->targetCompatibleBrowserVersion,
        outValue);
}

static HRESULT WINAPI
WebView2EnvOptions_put_TargetCompatibleBrowserVersion(
    ICoreWebView2EnvironmentOptions* const iface,
    LPCWSTR const value)
{
    struct WebView2EnvOptions* const this = GetImpl(iface);
    LocalFree(this->targetCompatibleBrowserVersion);
    this->targetCompatibleBrowserVersion = (NULL == value) ? NULL : StrDupW(value);
    return S_OK;
}

static HRESULT WINAPI
WebView2EnvOptions_get_AllowSingleSignOnUsingOSPrimaryAccount(
    ICoreWebView2EnvironmentOptions* const iface,
    BOOL* const outValue)
{
    const struct WebView2EnvOptions* const this = GetImpl(iface);
    *outValue = this->allowSingleSignOnUsingOSPrimaryAccount;
    return S_OK;
}

static HRESULT WINAPI
WebView2EnvOptions_put_AllowSingleSignOnUsingOSPrimaryAccount(
    ICoreWebView2EnvironmentOptions* const iface,
    const BOOL value)
{
    struct WebView2EnvOptions* const this = GetImpl(iface);
    this->allowSingleSignOnUsingOSPrimaryAccount = value;
    return S_OK;
}

ICoreWebView2EnvironmentOptions*
New_WebView2EnvOptions()
{
    struct WebView2EnvOptions* const this = LocalAlloc(LMEM_FIXED | LMEM_ZEROINIT, sizeof(struct WebView2EnvOptions));
    assert(NULL != this);

    this->ref = 1;

    this->vtbl.QueryInterface = WebView2EnvOptions_QueryInterface;
    this->vtbl.AddRef = WebView2EnvOptions_AddRef;
    this->vtbl.Release = WebView2EnvOptions_Release;
    this->vtbl.get_AdditionalBrowserArguments = WebView2EnvOptions_get_AdditionalBrowserArguments;
    this->vtbl.put_AdditionalBrowserArguments = WebView2EnvOptions_put_AdditionalBrowserArguments;
    this->vtbl.get_Language = WebView2EnvOptions_get_Language;
    this->vtbl.put_Language = WebView2EnvOptions_put_Language;
    this->vtbl.get_TargetCompatibleBrowserVersion = WebView2EnvOptions_get_TargetCompatibleBrowserVersion;
    this->vtbl.put_TargetCompatibleBrowserVersion = WebView2EnvOptions_put_TargetCompatibleBrowserVersion;
    this->vtbl.get_AllowSingleSignOnUsingOSPrimaryAccount
        = WebView2EnvOptions_get_AllowSingleSignOnUsingOSPrimaryAccount;
    this->vtbl.put_AllowSingleSignOnUsingOSPrimaryAccount
        = WebView2EnvOptions_put_AllowSingleSignOnUsingOSPrimaryAccount;
    this->ICoreWebView2EnvironmentOptions_iface.lpVtbl = &this->vtbl;
    return &this->ICoreWebView2EnvironmentOptions_iface;
}
