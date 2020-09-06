#include "Direct3D11Manager.hpp"
#include <string>
#include <memory>

using namespace std;

//�萔�錾
const string WIN_CLASS_NAME = "Sample Game";
const UINT WINDOW_WIDTH = 800;
const UINT WINDOW_HEIGHT = 600;

//�v���g�^�C�v�錾
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(
	_In_		HINSTANCE	hInstance, 
	_In_opt_	HINSTANCE	hPrevInstance, 
	_In_		LPSTR		lpCmdLine, 
	_In_		int			nCmdShow) {

	//�E�B���h�E�N���X���쐬
	WNDCLASSEX wc = {
		sizeof(WNDCLASSEX),									//�\���̃T�C�Y
		CS_VREDRAW | CS_HREDRAW,							//�X�^�C��
		WndProc,											//�E�B���h�E�v���V�[�W��
		0L,
		0L,
		hInstance,											//�C���X�^���X�n���h��
		LoadIcon(nullptr, IDI_APPLICATION),			//�A�C�R��
		LoadCursor(nullptr, IDC_ARROW),				//�}�E�X�J�[�\��
		(HBRUSH)GetStockObject(BLACK_BRUSH),				//�w�i
		nullptr,											//���j���[
		WIN_CLASS_NAME.c_str(),								//�E�B���h�E�N���X��
		LoadIcon(nullptr, IDI_APPLICATION)			//�������A�C�R��
	};
	RegisterClassEx(&wc);  //�N���X��o�^

	//�E�B���h�E�T�C�Y�̌v�Z
	RECT winRect = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
	AdjustWindowRect(&winRect, WS_OVERLAPPEDWINDOW, FALSE);
	int winW = winRect.right - winRect.left;     //�E�B���h�E��
	int winH = winRect.bottom - winRect.top;     //�E�B���h�E����

	//�E�B���h�E���쐬
	HWND hWnd = CreateWindow(
		wc.lpszClassName,			//�E�B���h�E�N���X��
		WIN_CLASS_NAME.c_str(),		//�^�C�g���o�[�ɕ\��������e
		WS_OVERLAPPEDWINDOW,		//�X�^�C���i���ʂ̃E�B���h�E�j
		CW_USEDEFAULT,				//�\���ʒu���i���܂����j
		CW_USEDEFAULT,				//�\���ʒu��i���܂����j
		winW,						//�E�B���h�E��
		winH,						//�E�B���h�E����
		nullptr,					//�e�E�C���h�E�i�Ȃ��j
		nullptr,					//���j���[�i�Ȃ��j
		hInstance,					//�C���X�^���X
		nullptr						//�p�����[�^�i�Ȃ��j
	);

	//�E�B���h�E��\��
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

		//�`��J�n
		direct3d->BeginDraw();


		//�`��I��
		direct3d->EndDraw();
		
	}

	return 0;
}

//�E�B���h�E�v���V�[�W���i�������������ɂ�΂��֐��j
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	
	switch (msg) {
	case WM_DESTROY:
		PostQuitMessage(0);  //�v���O�����I��
		return 0;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
}	