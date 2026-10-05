#include <Windows.h>
#include <logger2.h>
#include <config2.h>
#include <plugin2.h>
#include <CommCtrl.h>
#include <string>
#include "P2PEarthquakeAPI.h"
#include <nlohmann/json.hpp>

#define WINDOW_CLASS_NAME L"WindowClass"
#define WINDOW_NAME L"プラグイン"

#define MY_BUTTON1 1001

using json = nlohmann::json;

namespace {
    const int WINDOW_WIDTH = 1280;
    const int WINDOW_HEIGHT = 720;
    LOG_HANDLE* logger = nullptr;
    CONFIG_HANDLE* config = nullptr;
    EDIT_HANDLE* edit_handle = nullptr;

    COMMON_PLUGIN_TABLE common_plugin_table = {
        L"Aviutl Earhquake Plugin",								// プラグインの名前
        L"Aviutl Earhquake Plugin By mochineko",		// プラグインの情報
    };

    //---------------------------------------------------------------------
    //	必要とする本体バージョン番号取得関数 (未定義なら呼ばれません)
    //---------------------------------------------------------------------
    EXTERN_C __declspec(dllexport) DWORD RequiredVersion() {
        return 2003300;
    }

    //---------------------------------------------------------------------
    //	ログ出力機能初期化関数 (未定義なら呼ばれません)
    //---------------------------------------------------------------------
    EXTERN_C __declspec(dllexport) void InitializeLogger(LOG_HANDLE* handle) {
        logger = handle;
    }

    //---------------------------------------------------------------------
    //	設定関連初期化関数 (未定義なら呼ばれません)
    //---------------------------------------------------------------------
    EXTERN_C __declspec(dllexport) void InitializeConfig(CONFIG_HANDLE* handle) {
        config = handle;
    }

    //---------------------------------------------------------------------
    //	プラグインDLL初期化関数 (未定義なら呼ばれません)
    //---------------------------------------------------------------------
    EXTERN_C __declspec(dllexport) bool InitializePlugin(DWORD version) {
        return true;
    }

    //---------------------------------------------------------------------
    //	プラグインDLL解放関数 (未定義なら呼ばれません)
    //---------------------------------------------------------------------
    EXTERN_C __declspec(dllexport) void UninitializePlugin() {
    }

    //---------------------------------------------------------------------
    //	汎用プラグイン構造体のポインタを渡す関数
    //---------------------------------------------------------------------
    EXTERN_C __declspec(dllexport) COMMON_PLUGIN_TABLE* GetCommonPluginTable(void) {
        return &common_plugin_table;
    }

    LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        switch (msg) {
        case WM_DESTROY: {
            PostQuitMessage(0); //メッセージループ終了（=アプリを終了）
            break;
        }
        case WM_COMMAND: {
            switch (LOWORD(wParam)) {
            case MY_BUTTON1: {
                edit_handle->call_edit_section_param(&msg, [](void* message, EDIT_SECTION* edit) {
                    // エイリアスデータからオブジェクトを作成
                    auto& layer = edit->info->layer;
                    auto& frame = edit->info->frame;
                    MessageBox(NULL, (std::to_wstring(layer) + L"," + std::to_wstring(frame)).c_str(), NULL, MB_OK);
                    });
            }
            }
            break;
        }
        case WM_KEYDOWN: {
            MessageBox(NULL, L"key down", L"", MB_OK);
            switch (wParam) {
            case VK_ESCAPE: {

            }
            }
        }
        }
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }


    EXTERN_C __declspec(dllexport) void RegisterPlugin(HOST_APP_TABLE* host) {
        WNDCLASSEX wndClass = {};
        wndClass.cbSize = sizeof(WNDCLASSEX);
        wndClass.lpszClassName = WINDOW_CLASS_NAME;
        wndClass.lpfnWndProc = WndProc;
        wndClass.hInstance = GetModuleHandle(0);
        wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
        wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
        RegisterClassEx(&wndClass);

        //ウインドウを作成する
        HWND hwnd = CreateWindow(
            WINDOW_CLASS_NAME, //クラス名
            WINDOW_NAME, //ウインドウタイトル
            WS_POPUP, //ウインドウスタイル
            CW_USEDEFAULT, //表示位置X（デフォルト）
            CW_USEDEFAULT, //表示位置Y（デフォルト）
            CW_USEDEFAULT, //ウインドウ幅
            CW_USEDEFAULT, //ウインドウ高さ
            NULL, //親ウインドウ
            NULL, //メニュー
            GetModuleHandle(0), //インスタンス
            NULL //パラメータ
        );

        std::string result = P2PEarthquakeAPI::GetEarthquake();
        int size_needed = MultiByteToWideChar(CP_UTF8, 0, &result[0], (int)result.size(), NULL, 0);
        std::wstring wstr(size_needed, 0);
        MultiByteToWideChar(CP_UTF8, 0, &result[0], (int)result.size(), &wstr[0], size_needed);

        
        try {
            json data = json::parse(wstr);
            int maxScale = data.at(0).at("earthquake").at("maxScale").get<int>();

            CreateWindowEx(
                0,
                WC_STATIC,
                config->translate(config, (L"最大震度：" + std::to_wstring(maxScale)).c_str()),
                WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                10, 10, 200, 300,
                hwnd,
                (HMENU)MY_BUTTON1,
                GetModuleHandle(0),
                nullptr
            );
        }
        catch (const std::exception& e) {
            MessageBoxA(NULL, e.what(), "JSON Error", MB_OK);
            return;
        }


        if (hwnd == NULL) {
            return;
        }

        host->register_window_client(L"地震プラグイン", hwnd);
        edit_handle = host->create_edit_handle();

    }


}