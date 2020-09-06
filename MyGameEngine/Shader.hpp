#pragma once
#include "Direct3D11Manager.hpp"


class Shader {
	
private:
	VertexShader	pVertexShader;
	InputLayout		pInputLayout;
	HullShader		pHullShader;
	DomainShader	pDomainShader;
	GeometryShader	pGeometryShader;
	Rasterizer		pRasterizer;
	PixelShader		pPixelShader;

public:
	Shader() = default;
	~Shader() = default;

	HRESULT InitVertexShader();
	HRESULT InitHullShader();
	HRESULT InitDomainShader();
	HRESULT InitGeometryShader();
	HRESULT Init
};
