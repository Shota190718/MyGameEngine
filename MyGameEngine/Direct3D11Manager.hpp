#pragma once


#include <windows.h>		//WindowsApplication���g�p����̂ŕK�{
#include <d3d11.h>			//DirectX11��API�������Ă���
#include <d3dcompiler.h>	//�V�F�[�_�[�̃R���p�C��
//#include <DirectXMath.h>	//�x�N�g����FLOAT3�ȂǕ֗��ȎZ�p���C�u����
#include <wrl/client.h>		//�f�o�C�X�̊Ǘ������₷�����邽�߂�DirectX�ł̃X�}�[�g�|�C���^�̂悤�Ȃ���
#include <memory>			//�X�}�[�g�|�C���^

//DirectX API�֌W�̃����N
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

//Direct3D�Ǘ��N���X
class Direct3D11Manager {

private:
	static std::unique_ptr<Direct3D11Manager> direct3dInstance;
	
	HWND hWnd;
	UINT winWidth; 
	UINT winHeight;
	
public:
	ComPtr<ID3D11Device>			pDevice;			//Direct3D�{��
	ComPtr<ID3D11DeviceContext>		pImmediateContext;	//DX11�̕`�施�ߓ��𑗂邽�߂̂���
	ComPtr<IDXGISwapChain>			pSwapChain;			//�n�[�h�E�F�A�̏�񂪋l�܂��Ă������
	ComPtr<ID3D11RenderTargetView>	pRTView;			//�o�b�O�o�b�t�@��`���Ƃ��Ďw��ł���悤�ɂ�������
	D3D11_VIEWPORT					viewport;			//�E�B���h�E�̃T�C�Y�̎w��
	ComPtr<ID3D11Texture2D>			pDepthStencil;		//�[�x�X�e���V��
	ComPtr<ID3D11DepthStencilView>	pDepthStencilView;	//�[�x�X�e���V���r���[


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