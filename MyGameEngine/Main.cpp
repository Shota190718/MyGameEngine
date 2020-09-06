#include "Direct3D11Manager.hpp"
#include <string>
#include <memory>

using namespace std;

//定数宣言
const string WIN_CLASS_NAME = "Sample Game";
const UINT WINDOW_WIDTH = 800;
const UINT WINDOW_HEIGHT = 600;

//プロトタイプ宣言
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(
	_In_		HINSTANCE	hInstance, 
	_In_opt_	HINSTANCE	hPrevInstance, 
	_In_		LPSTR		lpCmdLine, 
	_In_		int			nCmdShow) {

	//ウィンドウクラスを作成
	WNDCLASSEX wc = {
		sizeof(WNDCLASSEX),									//構造体サイズ
		CS_VREDRAW | CS_HREDRAW,							//スタイル
		WndProc,											//ウィンドウプロシージャ
		0L,
		0L,
		hInstance,											//インスタンスハンドル
		LoadIcon(nullptr, IDI_APPLICATION),			//アイコン
		LoadCursor(nullptr, IDC_ARROW),				//マウスカーソル
		(HBRUSH)GetStockObject(BLACK_BRUSH),				//背景
		nullptr,											//メニュー
		WIN_CLASS_NAME.c_str(),								//ウィンドウクラス名
		LoadIcon(nullptr, IDI_APPLICATION)			//小さいアイコン
	};
	RegisterClassEx(&wc);  //クラスを登録

	//ウィンドウサイズの計算
	RECT winRect = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
	AdjustWindowRect(&winRect, WS_OVERLAPPEDWINDOW, FALSE);
	int winW = winRect.right - winRect.left;     //ウィンドウ幅
	int winH = winRect.bottom - winRect.top;     //ウィンドウ高さ

	//ウィンドウを作成
	HWND hWnd = CreateWindow(
		wc.lpszClassName,			//ウィンドウクラス名
		WIN_CLASS_NAME.c_str(),		//タイトルバーに表示する内容
		WS_OVERLAPPEDWINDOW,		//スタイル（普通のウィンドウ）
		CW_USEDEFAULT,				//表示位置左（おまかせ）
		CW_USEDEFAULT,				//表示位置上（おまかせ）
		winW,						//ウィンドウ幅
		winH,						//ウィンドウ高さ
		nullptr,					//親ウインドウ（なし）
		nullptr,					//メニュー（なし）
		hInstance,					//インスタンス
		nullptr						//パラメータ（なし）
	);

	//ウィンドウを表示
	ShowWindow(hWnd, nCmdShow);


	auto& direct3d = Direct3D11Manager::GetInstance();
	
	if (FAILED(direct3d->Initialize(hWnd)))	return -1;

	MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	while (true) {
		
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		if (msg.message == WM_QUIT) return 0;

		//描画開始
		direct3d->BeginDraw();


		//描画終了
		direct3d->EndDraw();
		
	}

	return 0;
}

//ウィンドウプロシージャ（何かあった時によばれる関数）
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	
	switch (msg) {
	case WM_DESTROY:
		PostQuitMessage(0);  //プログラム終了
		return 0;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
}	