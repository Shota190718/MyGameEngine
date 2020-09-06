#include "Direct3D11Manager.hpp"

#include <array>
#include <vector>

#include "Global.hpp"

using namespace std;

std::unique_ptr<Direct3D11Manager> Direct3D11Manager::direct3dInstance;

Direct3D11Manager::Direct3D11Manager()
	: hWnd(nullptr)
	, winWidth(0)
	, winHeight(0)
	, viewport({0, 0, 0, 0, 0, 0 }) {
}

std::unique_ptr<Direct3D11Manager>& Direct3D11Manager::GetInstance() {

	if (!direct3dInstance) {

		struct Instantiate : Direct3D11Manager { Instantiate() {} };
		direct3dInstance = make_unique<Instantiate>();
	}
	return direct3dInstance;
}

void Direct3D11Manager::Destroy() {

	direct3dInstance.reset();
}

HRESULT Direct3D11Manager::Initialize(HWND hWnd) {

	this->hWnd = hWnd;
	
	ComPtr<IDXGIFactory> factory;

	HRESULT hr = CreateDXGIFactory1(IID_PPV_ARGS(&factory));
	MyUtilities::CatchErrorResult(hr, "ファクトリ生成に失敗");

	vector<ComPtr<IDXGIAdapter>> adapters;
	ComPtr<IDXGIAdapter> tmpAdapter = nullptr;
	for (int i = 0; factory->EnumAdapters(i, &tmpAdapter) != DXGI_ERROR_NOT_FOUND; ++i) {

		adapters.emplace_back(tmpAdapter.Detach());
	}
	
	for (auto&& adapter : adapters) {

		DXGI_ADAPTER_DESC aDesc;
		adapter->GetDesc(&aDesc);
		wstring strDesc = aDesc.Description;

		if (strDesc.find(L"NVIDIA") != string::npos) {

			tmpAdapter = adapter;
			break;
		}
	}
	adapters.clear();
	
	UINT flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef _DEBUG
	flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	
	RECT rect;
	GetClientRect(hWnd, &rect);
	winWidth = rect.right;
	winHeight = rect.bottom;

	//スワップチェイン設定
	DXGI_SWAP_CHAIN_DESC scDesc;
	ZeroMemory(&scDesc, sizeof(scDesc));
	scDesc.BufferCount = 1;						//バックバッファの枚数
	scDesc.BufferDesc.Width = winWidth;			//画面幅
	scDesc.BufferDesc.Height = winHeight;		//画面高さ
	scDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// 何色使えるか
	scDesc.BufferDesc.RefreshRate.Numerator = 60;
	scDesc.BufferDesc.RefreshRate.Denominator = 1;
	scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	//バックバッファの使い道＝画面に描画するために
	scDesc.OutputWindow = hWnd;			//ウィンドウハンドル
	scDesc.SampleDesc.Count = 1;		//アンチエイリアスの設定
	scDesc.SampleDesc.Quality = 0;		//アンチエイリアスの設定
	scDesc.Windowed = TRUE;				//ウィンドウモードかフルスクリーンか

	array<D3D_FEATURE_LEVEL, 4> levels = {
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0
	};
	D3D_FEATURE_LEVEL featureLevel;

	hr = D3D11CreateDeviceAndSwapChain(
		tmpAdapter.Get(),			// どのビデオアダプタを使用するか？既定ならばnullptrで
		D3D_DRIVER_TYPE_HARDWARE,	// ドライバのタイプを渡す。ふつうはHARDWARE
		nullptr,					// 上記をD3D_DRIVER_TYPE_SOFTWAREに設定しないかぎりnullptr
		flags,						// 何らかのフラグを指定する。（デバッグ時はD3D11_CREATE_DEVICE_DEBUG？）
		levels.data(),				// デバイス、コンテキストのレベルを設定。nullptrにしとけばOK
		(UINT)levels.size(),		// 上の引数でレベルを何個指定したか
		D3D11_SDK_VERSION,			// SDKのバージョン。必ずこの値
		&scDesc,					// 上でいろいろ設定した構造体
		&pSwapChain,				// 無事完成したSwapChainのアドレスが返ってくる
		&pDevice,					// 無事完成したDeviceアドレスが返ってくる
		&featureLevel,				// 無事完成したDevice、Contextのレベルが返ってくる
		&pImmediateContext			// 無事完成したContextのアドレスが返ってくる
	);
	MyUtilities::CatchErrorResult(hr, "デバイス、スワップチェイン生成失敗");

	
	ComPtr<ID3D11Texture2D> pBackBuffer;

	//一時的にバックバッファを取得
	hr = pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
	MyUtilities::CatchErrorResult(hr, "バックバッファ取得失敗");

	//レンダーターゲットビュー作成
	hr = pDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, &pRTView);
	MyUtilities::CatchErrorResult(hr, "レンダーターゲットビュー作成失敗");

	pBackBuffer.Reset();

	
	D3D11_TEXTURE2D_DESC txDesc;
	ZeroMemory(&txDesc, sizeof(txDesc));
	txDesc.Width = winWidth;
	txDesc.Height = winHeight;
	txDesc.MipLevels = 1;
	txDesc.ArraySize = 1;
	txDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	txDesc.SampleDesc.Count = 1;
	txDesc.SampleDesc.Quality = 0;
	txDesc.Usage = D3D11_USAGE_DEFAULT;
	txDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	txDesc.CPUAccessFlags = 0;
	txDesc.MiscFlags = 0;
	hr = pDevice->CreateTexture2D(&txDesc, nullptr, &pDepthStencil);
	MyUtilities::CatchErrorResult(hr, "深度ステンシル作成失敗");

	D3D11_DEPTH_STENCIL_VIEW_DESC dsDesc;
	ZeroMemory(&dsDesc, sizeof(dsDesc));
	dsDesc.Format = txDesc.Format;
	dsDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsDesc.Texture2D.MipSlice = 0;
	hr = pDevice->CreateDepthStencilView(pDepthStencil.Get(), &dsDesc, &pDepthStencilView);
	MyUtilities::CatchErrorResult(hr, "深度ステンシルビュー作成失敗");


	viewport.Width = (FLOAT)winWidth;	//幅
	viewport.Height = (FLOAT)winHeight;//高さ
	viewport.MinDepth = 0.0f;	//手前
	viewport.MaxDepth = 1.0f;	//奥
	viewport.TopLeftX = 0;	//左
	viewport.TopLeftY = 0;	//上
	

	//データを画面に描画するための一通りの設定（パイプライン）
	pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);  // データの入力種類を指定
	pImmediateContext->OMSetRenderTargets(1, pRTView.GetAddressOf(), pDepthStencilView.Get());            // 描画先を設定
	pImmediateContext->RSSetViewports(1, &viewport);

	
	return hr;
}

void Direct3D11Manager::BeginDraw() const {

	//背景の色
	float clearColor[4] = { 0.0f, 0.5f, 0.5f, 1.0f };//R,G,B,A

	//画面をクリア
	pImmediateContext->ClearRenderTargetView(pRTView.Get(), clearColor);

	//深度バッファクリア
	pImmediateContext->ClearDepthStencilView(pDepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void Direct3D11Manager::EndDraw() const {

	//スワップ（バックバッファを表に表示する）
	pSwapChain->Present(0, 0);
}
