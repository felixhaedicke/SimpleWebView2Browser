SimpleWebView2Browser
=====================
Very simple Windows Web browser using Microsoft's WebView2 API.

It is a simple window with an embedded a WebView2 web browser.
You can specify a URL as command line parameter.

This is mainly intended as a programming example for embedding WebView2 using OOP in C.

Build instructions
------------------
You can build this program using Visual Studio 2022, using the included solution file.

For building with gcc / clang, you can use the Makefile.
First, you need to obtain the WebView2 SDK [NuGet](https://www.nuget.org/) package,
and extract it to the webview2-sdk subfolder:

    curl -L https://www.nuget.org/api/v2/package/Microsoft.Web.WebView2/1.0.2151.40 -o webview2-sdk.zip && unzip webview2-sdk.zip -d webview2-sdk && rm webview2-sdk.zip
    
And for cross compiling on a systems with case sensitive systems, e. g. Linux, the `WebView2.h`
header needs to be modified:

    sed -i s/EventToken.h/eventtoken.h/g webview2-sdk/build/native/include/WebView2.h 

To build a release build using GCC (command gcc), just run `make`

    make

Or when the compiler is `x86_64-w64-mingw32-gcc`:

    make CC=x86_64-w64-mingw32-gcc

Example for building a x86 (32 bit) debug build using `i686-w64-mingw32-gcc`:

    make CC=i686-w64-mingw32-gcc ARCH=x86 DEBUG=y
    
See Makefile for a list of customizable variables.

