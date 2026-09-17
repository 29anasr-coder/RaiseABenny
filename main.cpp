#include <windows.h>\
#include <exdisp.h>\
#include <olectl.h>\
#include <shlwapi.h>\
\
#pragma comment(lib, "shlwapi.lib")\
\
HINSTANCE hInst;\
HWND hWndMain;\
IWebBrowser2* pWebBrowser = NULL;\
\
void GetLocalHtmlPath(wchar_t* buffer, DWORD size) \{\
    GetModuleFileNameW(NULL, buffer, size);\
    PathRemoveFileSpecW(buffer);\
    PathAppendW(buffer, L"index.html");\
\}\
\
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) \{\
    switch (message) \{\
    case WM_CREATE: \{\
        OleInitialize(NULL);\
        RECT rc;\
        GetClientRect(hWnd, &rc);\
\
        HRESULT hr = CoCreateInstance(CLSID_WebBrowser, NULL, CLSCTX_INPROC_SERVER, IID_IWebBrowser2, (void**)&pWebBrowser);\
        if (SUCCEEDED(hr)) \{\
            IOleObject* pOleObject = NULL;\
            pWebBrowser->QueryInterface(IID_IOleObject, (void**)&pOleObject);\
            if (pOleObject) \{\
                pOleObject->SetClientSite(NULL);\
                pOleObject->DoVerb(OLEIVERB_INPLACEACTIVATE, NULL, NULL, 0, hWnd, &rc);\
                pOleObject->Release();\
            \}\
\
            wchar_t htmlPath[MAX_PATH];\
            GetLocalHtmlPath(htmlPath, MAX_PATH);\
\
            BSTR bstrUrl = SysAllocString(htmlPath);\
            VARIANT varEmpty;\
            VariantInit(&varEmpty);\
            pWebBrowser->Navigate(bstrUrl, &varEmpty, &varEmpty, &varEmpty, &varEmpty);\
            SysFreeString(bstrUrl);\
\
            pWebBrowser->put_Visible(VARIANT_TRUE);\
        \}\
        break;\
    \}\
    case WM_SIZE: \{\
        if (pWebBrowser) \{\
            long width = LOWORD(lParam);\
            long height = HIWORD(lParam);\
            pWebBrowser->put_Width(width);\
            pWebBrowser->put_Height(height);\
        \}\
        break;\
    \}\
    case WM_DESTROY:\
        if (pWebBrowser) \{\
            pWebBrowser->Release();\
        \}\
        OleUninitialize();\
        PostQuitMessage(0);\
        break;\
    default:\
        return DefWindowProc(hWnd, message, wParam, lParam);\
    \}\
    return 0;\
\}\
\
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) \{\
    hInst = hInstance;\
    \
    WNDCLASSEXW wcex = \{ 0 \};\
    wcex.cbSize = sizeof(WNDCLASSEX);\
    wcex.style = CS_HREDRAW | CS_VREDRAW;\
    wcex.lpfnWndProc = WndProc;\
    wcex.hInstance = hInstance;\
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);\
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);\
    wcex.lpszClassName = L"BennyXPClass";\
\
    RegisterClassExW(&wcex);\
\
    hWndMain = CreateWindowW(L"BennyXPClass", L"Raise A Benny", WS_OVERLAPPEDWINDOW,\
        CW_USEDEFAULT, 0, 800, 600, NULL, NULL, hInstance, NULL);\
\
    if (!hWndMain) return FALSE;\
\
    ShowWindow(hWndMain, nCmdShow);\
    UpdateWindow(hWndMain);\
\
    MSG msg;\
    while (GetMessage(&msg, NULL, 0, 0)) \{\
        TranslateMessage(&msg);\
        DispatchMessage(&msg);\
    \}\
\
    return (int)msg.wParam;\
\}}
