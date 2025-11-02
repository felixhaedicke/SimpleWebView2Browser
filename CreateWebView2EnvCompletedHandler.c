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

#include "CreateWebView2EnvCompletedHandler.h"


struct CreateWebView2EnvCompletedHandler
{
    ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler
        ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler_iface;
    ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandlerVtbl vtbl;
    LONG ref;
    WebViewEnvCreatedCb cb;
    void* ctx;
};

static inline struct CreateWebView2EnvCompletedHandler*
GetImpl(ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler* const iface)
{
    return CONTAINING_RECORD(
        iface,
        struct CreateWebView2EnvCompletedHandler,
        ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler_iface);
}

static HRESULT WINAPI
CreateWebView2EnvCompletedHandler_QueryInterface(
    ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler* const iface,
    REFIID riid,
    void** const ret)
{
    if (!IsEqualIID(riid, &IID_ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler)
        && !IsEqualIID(riid, &IID_IUnknown))
    {
        *ret = 0;
        return E_NOINTERFACE;
    }

    *ret = iface;
    ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler_AddRef(iface);
    return S_OK;
}

static ULONG WINAPI
CreateWebView2EnvCompletedHandler_AddRef(
    ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler* const iface)
{
    struct CreateWebView2EnvCompletedHandler* const this
        = GetImpl(iface);
    const ULONG refcount = InterlockedIncrement(&this->ref);
    return refcount;
}

static ULONG WINAPI
CreateWebView2EnvCompletedHandler_Release(
    ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler* const iface)
{
    struct CreateWebView2EnvCompletedHandler* const this
        = GetImpl(iface);
    const ULONG refcount = InterlockedDecrement(&this->ref);
    if (0 == refcount) LocalFree(this);
    return refcount;
}

static HRESULT WINAPI
CreateWebView2EnvCompletedHandler_Invoke(
    ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler* const iface,
    const HRESULT errorCode,
    ICoreWebView2Environment* const createdEnvironment)
{
    struct CreateWebView2EnvCompletedHandler* const this
        = GetImpl(iface);
    this->cb(errorCode, createdEnvironment, this->ctx);
    return S_OK;
}

ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler*
New_CreateWebView2EnvCompletedHandler(WebViewEnvCreatedCb cb, void* const ctx)
{
    struct CreateWebView2EnvCompletedHandler* const this
        = LocalAlloc(LMEM_FIXED, sizeof(struct CreateWebView2EnvCompletedHandler));
    assert(NULL != this);

    this->ref = 1;
    this->cb = cb;
    this->ctx = ctx;
    
    this->vtbl.QueryInterface = CreateWebView2EnvCompletedHandler_QueryInterface;
    this->vtbl.AddRef = CreateWebView2EnvCompletedHandler_AddRef;
    this->vtbl.Release = CreateWebView2EnvCompletedHandler_Release;
    this->vtbl.Invoke = CreateWebView2EnvCompletedHandler_Invoke;
    this->ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler_iface.lpVtbl = &this->vtbl;
    return &this->ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler_iface;
}
