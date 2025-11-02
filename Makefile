CC=gcc

ARCH=x64

CFLAGS_DEBUG=-O0 -g
CFLAGS_RELEASE=-Os -DNDEBUG -Wno-unused-variable

DEBUG=n
ifeq ($(DEBUG),y)
    CFLAGS_DEBUG_RELEASE=$(CFLAGS_DEBUG)
else
    CFLAGS_DEBUG_RELEASE=$(CFLAGS_RELEASE)
endif

CFLAGS=-municode -Wall -Wextra -Wno-unused-parameter -DCOBJMACROS -isystem ./webview2-sdk/build/native/include $(CFLAGS_DEBUG_RELEASE) $(CFLAGS_EXTRA)
LDFLAGS=-municode -L./webview2-sdk/build/native/$(ARCH) -luser32 -lshell32 -lole32 -lshlwapi -luuid -l"WebView2Loader.dll" $(LDFLAGS_EXTRA)

OBJ=CreateWebView2CtrlCompletedHandler.o CreateWebView2EnvCompletedHandler.o WebView2EnvOptions.o WebView2Window.o main.o


.PHONY: all
all: SimpleWebView2Browser.exe WebView2Loader.dll

.PHONY: clean
clean:
	$(RM) *.o SimpleWebView2Browser.exe WebView2Loader.dll

.PHONY: nuget
nuget:
	nuget restore

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

SimpleWebView2Browser.exe: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

WebView2Loader.dll: webview2-sdk/build/native/$(ARCH)/WebView2Loader.dll
	cp $< $@
