#pragma once


#include <windows.h>		//WindowsApplicationを使用するので必須
#include <d3d11.h>			//DirectX11のAPIが入っている
#include <d3dcompiler.h>	//シェーダーのコンパイラ
//#include <DirectXMath.h>	//ベクトルやFLOAT3など便利な算術ライブラリ
#include <wrl/client.h>		//デバイスの管理をしやすくするためのDirectX版のスマートポインタのようなもの
#include <memory>			//スマートポインタ

//DirectX API関係のリンク
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

using Microsoft::WRL::ComPtr;

using ConstantBuffer	= ComPtr<ID3D11Buffer>;
using VertexBuffer		= ComPtr<ID3D11Buffer>;
using IndexBuffer		= ComPtr<ID3D11Buffer>;
using StructuredBuffer	= ComPtr<ID3D11Buffer>;
using InputLayout		= ComPtr<ID3D11InputLayout>;
using VertexShader		= ComPtr<ID3D11VertexShader>;
using GeometryShader	= ComPtr<ID3D11GeometryShader>;
using PixelShader		= ComPtr<ID3D11PixelShader>;
using ComputeShader		= ComPtr<ID3D11ComputeShader>;
using ShaderTexture		= ComPtr<ID3D11ShaderResourceView>;
using ComputeOutputView	= ComPtr<ID3D11UnorderedAccessView>;

//Direct3D管理クラス
class Direct3D11Manager {

private:
	static std::unique_ptr<Direct3D11Manager> direct3dInstance;
	
	HWND hWnd;
	UINT winWidth; 
	UINT winHeight;
	
public:
	ComPtr<ID3D11Device>			pDevice;			//Direct3D本体
	ComPtr<ID3D11DeviceContext>		pImmediateContext;	//DX11の描画命令等を送るためのもの
	ComPtr<IDXGISwapChain>			pSwapChain;			//ハードウェアの情報が詰まっているもの
	ComPtr<ID3D11RenderTargetView>	pRTView;			//バッグバッファを描画先として指定できるようにしたもの
	D3D11_VIEWPORT					viewport;			//ウィンドウのサイズの指定
	ComPtr<ID3D11Texture2D>			pDepthStencil;		//深度ステンシル
	ComPtr<ID3D11DepthStencilView>	pDepthStencilView;	//深度ステンシルビュー


private:
	Direct3D11Manager();
public:
	~Direct3D11Manager() = default;

	static std::unique_ptr<Direct3D11Manager>& GetInstance();
	static void Destroy();

public:
	HRESULT Initialize(HWND hWnd);

	void BeginDraw() const;
	void EndDraw() const;
};
