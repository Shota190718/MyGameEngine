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
	MyUtilities::CatchErrorResult(hr, "�t�@�N�g�������Ɏ��s");

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

	//�X���b�v�`�F�C���ݒ�
	DXGI_SWAP_CHAIN_DESC scDesc;
	ZeroMemory(&scDesc, sizeof(scDesc));
	scDesc.BufferCount = 1;						//�o�b�N�o�b�t�@�̖���
	scDesc.BufferDesc.Width = winWidth;			//��ʕ�
	scDesc.BufferDesc.Height = winHeight;		//��ʍ���
	scDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// ���F�g���邩
	scDesc.BufferDesc.RefreshRate.Numerator = 60;
	scDesc.BufferDesc.RefreshRate.Denominator = 1;
	scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	//�o�b�N�o�b�t�@�̎g��������ʂɕ`�悷�邽�߂�
	scDesc.OutputWindow = hWnd;			//�E�B���h�E�n���h��
	scDesc.SampleDesc.Count = 1;		//�A���`�G�C���A�X�̐ݒ�
	scDesc.SampleDesc.Quality = 0;		//�A���`�G�C���A�X�̐ݒ�
	scDesc.Windowed = TRUE;				//�E�B���h�E���[�h���t���X�N���[����

	array<D3D_FEATURE_LEVEL, 4> levels = {
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0
	};
	D3D_FEATURE_LEVEL featureLevel;

	hr = D3D11CreateDeviceAndSwapChain(
		tmpAdapter.Get(),			// �ǂ̃r�f�I�A�_�v�^���g�p���邩�H����Ȃ��nullptr��
		D3D_DRIVER_TYPE_HARDWARE,	// �h���C�o�̃^�C�v��n���B�ӂ���HARDWARE
		nullptr,					// ��L��D3D_DRIVER_TYPE_SOFTWARE�ɐݒ肵�Ȃ�������nullptr
		flags,						// ���炩�̃t���O���w�肷��B�i�f�o�b�O����D3D11_CREATE_DEVICE_DEBUG�H�j
		levels.data(),				// �f�o�C�X�A�R���e�L�X�g�̃��x����ݒ�Bnullptr�ɂ��Ƃ���OK
		(UINT)levels.size(),		// ��̈����Ń��x�������w�肵����
		D3D11_SDK_VERSION,			// SDK�̃o�[�W�����B�K�����̒l
		&scDesc,					// ��ł��낢��ݒ肵���\����
		&pSwapChain,				// ������������SwapChain�̃A�h���X���Ԃ��Ă���
		&pDevice,					// ������������Device�A�h���X���Ԃ��Ă���
		&featureLevel,				// ������������Device�AContext�̃��x�����Ԃ��Ă���
		&pImmediateContext			// ������������Context�̃A�h���X���Ԃ��Ă���
	);
	MyUtilities::CatchErrorResult(hr, "�f�o�C�X�A�X���b�v�`�F�C���������s");

	
	ComPtr<ID3D11Texture2D> pBackBuffer;

	//�ꎞ�I�Ƀo�b�N�o�b�t�@���擾
	hr = pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
	MyUtilities::CatchErrorResult(hr, "�o�b�N�o�b�t�@�擾���s");

	//�����_�[�^�[�Q�b�g�r���[�쐬
	hr = pDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, &pRTView);
	MyUtilities::CatchErrorResult(hr, "�����_�[�^�[�Q�b�g�r���[�쐬���s");

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
	MyUtilities::CatchErrorResult(hr, "�[�x�X�e���V���쐬���s");

	D3D11_DEPTH_STENCIL_VIEW_DESC dsDesc;
	ZeroMemory(&dsDesc, sizeof(dsDesc));
	dsDesc.Format = txDesc.Format;
	dsDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsDesc.Texture2D.MipSlice = 0;
	hr = pDevice->CreateDepthStencilView(pDepthStencil.Get(), &dsDesc, &pDepthStencilView);
	MyUtilities::CatchErrorResult(hr, "�[�x�X�e���V���r���[�쐬���s");


	viewport.Width = (FLOAT)winWidth;	//��
	viewport.Height = (FLOAT)winHeight;//����
	viewport.MinDepth = 0.0f;	//��O
	viewport.MaxDepth = 1.0f;	//��
	viewport.TopLeftX = 0;	//��
	viewport.TopLeftY = 0;	//��
	

	//�f�[�^����ʂɕ`�悷�邽�߂̈�ʂ�̐ݒ�i�p�C�v���C���j
	pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);  // �f�[�^�̓��͎�ނ��w��
	pImmediateContext->OMSetRenderTargets(1, pRTView.GetAddressOf(), pDepthStencilView.Get());            // �`����ݒ�
	pImmediateContext->RSSetViewports(1, &viewport);

	
	return hr;
}

void Direct3D11Manager::BeginDraw() const {

	//�w�i�̐F
	float clearColor[4] = { 0.0f, 0.5f, 0.5f, 1.0f };//R,G,B,A

	//��ʂ��N���A
	pImmediateContext->ClearRenderTargetView(pRTView.Get(), clearColor);

	//�[�x�o�b�t�@�N���A
	pImmediateContext->ClearDepthStencilView(pDepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void Direct3D11Manager::EndDraw() const {

	//�X���b�v�i�o�b�N�o�b�t�@��\�ɕ\������j
	pSwapChain->Present(0, 0);
}
