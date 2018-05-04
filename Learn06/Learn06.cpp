// Learn06.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "Learn06.h"
#include <d3d11_2.h>
#include <D3Dcompiler.h>
#include <D3DX11.h>
#include <DirectXColors.h>
using namespace DirectX;

#define MAX_LOADSTRING 100

struct SimpleVertex
{
	XMFLOAT3 Postion;
	XMFLOAT3 Normal;
	XMFLOAT4 Color;
	XMFLOAT2 Texcoord;
};

struct ConstantBuffer
{
	XMMATRIX World;
	XMMATRIX View;
	XMMATRIX Projection;
};

struct LightBuffer
{
	XMFLOAT4 LightDir;
	XMFLOAT4 LightColor;
};

// 全局变量: 
HINSTANCE hInst;								// 当前实例
TCHAR szTitle[MAX_LOADSTRING];					// 标题栏文本
TCHAR szWindowClass[MAX_LOADSTRING];			// 主窗口类名
HWND hWnd;
IDXGISwapChain* g_pSwapChain;
ID3D11Device* g_pDevice;
ID3D11DeviceContext* g_pDeviceContext;
D3D_FEATURE_LEVEL g_featureLevel;
ID3D11RenderTargetView* g_pRenderTargetView;
ID3D11Texture2D* g_pDepthStencilTex;
ID3D11DepthStencilView* g_pDepthStencilView;
ID3D11VertexShader* g_pVertexShader;
ID3D11InputLayout* g_pInputLayout;
ID3D11PixelShader* g_pPixelShader;
ID3D11Buffer* g_pVertexBuffer;
ID3D11Buffer* g_pIndexBuffer;
ID3D11Buffer* g_pConstantBuffer;
ID3D11Buffer* g_pLightBuffer;
XMMATRIX g_World;
XMMATRIX g_View;
XMMATRIX g_Projection;
ID3D11ShaderResourceView* g_pTextureResourceView;
ID3D11SamplerState* g_pSamplerLinear;

// 此代码模块中包含的函数的前向声明: 
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
HRESULT				InitDevice();
void				Render();
void				CleanupDevice();

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO:  在此放置代码。
	MSG msg = {};

	// 初始化全局字符串
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_LEARN06, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 执行应用程序初始化: 
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	if (FAILED(InitDevice()))
	{
		CleanupDevice();
		return FALSE;
	}

	// 主消息循环: 
	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, hWnd, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			Render();
		}
	}

	CleanupDevice();
	return (int) msg.wParam;
}



//
//  函数:  MyRegisterClass()
//
//  目的:  注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LEARN06));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_LEARN06);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   函数:  InitInstance(HINSTANCE, int)
//
//   目的:  保存实例句柄并创建主窗口
//
//   注释: 
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{

   hInst = hInstance; // 将实例句柄存储在全局变量中

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);

   return TRUE;
}

//
//  函数:  WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:    处理主窗口的消息。
//
//  WM_COMMAND	- 处理应用程序菜单
//  WM_PAINT	- 绘制主窗口
//  WM_DESTROY	- 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// 分析菜单选择: 
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO:  在此添加任意绘图代码...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

HRESULT InitDevice()
{
	RECT rect;
	GetClientRect(hWnd, &rect);
	UINT width = rect.right - rect.left;
	UINT height = rect.bottom - rect.top;

	UINT createFlags = 0;
#if defined(_DEBUG)
	createFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
	};
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.Width = width;
	sd.BufferDesc.Height = height;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.Flags = 0;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.SwapEffect = DXGI_SWAP_EFFECT_SEQUENTIAL;
	sd.Windowed = true;
	HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createFlags, featureLevels, ARRAYSIZE(featureLevels), D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pDevice, &g_featureLevel, &g_pDeviceContext);
	if (FAILED(hr))
		return hr;

	ID3D11Texture2D* pBackBuffer = nullptr;
	hr = g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void **)&pBackBuffer);
	if (FAILED(hr))
		return hr;
	hr = g_pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_pRenderTargetView);
	if (FAILED(hr))
		return hr;
	D3D11_TEXTURE2D_DESC td;
	ZeroMemory(&td, sizeof(td));
	td.ArraySize = 1;
	td.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	td.CPUAccessFlags = 0;
	td.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	td.Width = width;
	td.Height = height;
	td.MipLevels = 1;
	td.MiscFlags = 0;
	td.SampleDesc.Count = 1;
	td.SampleDesc.Quality = 0;
	td.Usage = D3D11_USAGE_DEFAULT;
	hr = g_pDevice->CreateTexture2D(&td, nullptr, &g_pDepthStencilTex);
	if (FAILED(hr))
		return hr;
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
	dsvd.Flags = 0;
	dsvd.Format = td.Format;
	dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvd.Texture2D.MipSlice = 0;
	hr = g_pDevice->CreateDepthStencilView(g_pDepthStencilTex, &dsvd, &g_pDepthStencilView);
	if (FAILED(hr))
		return hr;

	g_pDeviceContext->OMSetRenderTargets(1, &g_pRenderTargetView, g_pDepthStencilView);

	D3D11_VIEWPORT vp;
	ZeroMemory(&vp, sizeof(vp));
	vp.Width = width;
	vp.Height = height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	g_pDeviceContext->RSSetViewports(1, &vp);

	ID3DBlob* pVSBlob,* pPSBlob,* pErrBlob;
	UINT shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(_DEBUG)
	shaderFlags |= D3DCOMPILE_DEBUG;
#endif
	hr = D3DX11CompileFromFile(L"Learn06.fx", NULL, NULL, "VS", "vs_4_0", shaderFlags, 0, NULL, &pVSBlob, &pErrBlob, NULL);
	if (FAILED(hr))
		return hr;

	hr = g_pDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL, &g_pVertexShader);
	if (FAILED(hr))
		return hr;

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 40, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	hr = g_pDevice->CreateInputLayout(layout, ARRAYSIZE(layout), pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &g_pInputLayout);
	pVSBlob->Release();
	if (FAILED(hr))
		return hr;

	g_pDeviceContext->IASetInputLayout(g_pInputLayout);

	hr = D3DX11CompileFromFile(L"Learn06.fx", NULL, NULL, "PS", "ps_4_0", shaderFlags, 0, NULL, &pPSBlob, &pErrBlob, NULL);
	if (FAILED(hr))
		return hr;

	hr = g_pDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), NULL, &g_pPixelShader);
	pPSBlob->Release();
	if (FAILED(hr))
		return hr;

	SimpleVertex vertices[] =
	{
		{ XMFLOAT3(0.f, 0.f, 0.f), XMFLOAT3(0.f, 0.f, -1.f), XMFLOAT4(1.f, 0.f, 0.f, 1.f), XMFLOAT2(0.f, 0.f) },
		{ XMFLOAT3(1.f, 0.f, 0.f), XMFLOAT3(0.f, 0.f, -1.f), XMFLOAT4(1.f, 0.f, 0.f, 1.f), XMFLOAT2(1.f, 0.f) },
		{ XMFLOAT3(1.f, 1.f, 0.f), XMFLOAT3(0.f, 0.f, -1.f), XMFLOAT4(1.f, 0.f, 0.f, 1.f), XMFLOAT2(1.f, 1.f) },
		{ XMFLOAT3(0.f, 1.f, 0.f), XMFLOAT3(0.f, 0.f, -1.f), XMFLOAT4(1.f, 0.f, 0.f, 1.f), XMFLOAT2(0.f, 1.f)},

		{ XMFLOAT3(0.f, 0.f, 0.f), XMFLOAT3(-1.f, 0.f, 0.f), XMFLOAT4(0.f, 0.f, 1.f, 1.f), XMFLOAT2(0.f, 0.f) },
		{ XMFLOAT3(0.f, 0.f, 1.f), XMFLOAT3(-1.f, 0.f, 0.f), XMFLOAT4(0.f, 0.f, 1.f, 1.f), XMFLOAT2(0.f, 1.f) },
		{ XMFLOAT3(0.f, 1.f, 1.f), XMFLOAT3(-1.f, 0.f, 0.f), XMFLOAT4(0.f, 0.f, 1.f, 1.f), XMFLOAT2(1.f, 1.f) },
		{ XMFLOAT3(0.f, 1.f, 0.f), XMFLOAT3(-1.f, 0.f, 0.f), XMFLOAT4(0.f, 0.f, 1.f, 1.f), XMFLOAT2(1.f, 0.f) },
	};

	WORD indices[] =
	{
		0, 2, 1,
		0, 3, 2,

		4, 6, 7,
		4, 5, 6,
	};

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.ByteWidth = sizeof(SimpleVertex) * ARRAYSIZE(vertices);
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	bd.Usage = D3D11_USAGE_DEFAULT;
	D3D11_SUBRESOURCE_DATA srd;
	ZeroMemory(&srd, sizeof(srd));
	srd.pSysMem = vertices;
	hr = g_pDevice->CreateBuffer(&bd, &srd, &g_pVertexBuffer);
	if (FAILED(hr))
		return hr;
	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;
	g_pDeviceContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.ByteWidth = sizeof(WORD) * ARRAYSIZE(indices);
	srd.pSysMem = indices;
	hr = g_pDevice->CreateBuffer(&bd, &srd, &g_pIndexBuffer);
	if (FAILED(hr))
		return hr;
	g_pDeviceContext->IASetIndexBuffer(g_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	g_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.ByteWidth = sizeof(ConstantBuffer);
	hr = g_pDevice->CreateBuffer(&bd, NULL, &g_pConstantBuffer);
	if (FAILED(hr))
		return hr;

	bd.ByteWidth = sizeof(LightBuffer);
	hr = g_pDevice->CreateBuffer(&bd, NULL, &g_pLightBuffer);
	if (FAILED(hr))
		return hr;

	hr = D3DX11CreateShaderResourceViewFromFile(g_pDevice, L"seafloor.dds", NULL, NULL, &g_pTextureResourceView, NULL);
	if (FAILED(hr))
		return hr;

	D3D11_SAMPLER_DESC sampd;
	ZeroMemory(&sampd, sizeof(sampd));
	sampd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampd.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampd.MaxLOD = D3D11_FLOAT32_MAX;
	sampd.MinLOD = 0;
	hr = g_pDevice->CreateSamplerState(&sampd, &g_pSamplerLinear);
	if (FAILED(hr))
		return hr;

	g_World = XMMatrixIdentity();

	XMVECTOR eye = XMVectorSet(-5.f, 0.f, -10.f, 1.f);
	XMVECTOR up = XMVectorSet(0.f, 1.f, 0.f, 1.f);
	XMVECTOR focus = XMVectorSet(0.f, 0.f, 0.f, 1.f);
	g_View = XMMatrixLookAtLH(eye, focus, up);

	g_Projection = XMMatrixPerspectiveFovLH(XM_PIDIV2, width / (float)height, 1.f, 100.f);

	return S_OK;
}

void Render()
{
	static DWORD start = 0;
	DWORD t = 0;
	if (start <= 0)
	{
		start = GetTickCount();
	}
	else
	{
		t = GetTickCount() - start;
	}

	g_World = XMMatrixRotationY(- (float)t / 1000.f);

	g_pDeviceContext->ClearRenderTargetView(g_pRenderTargetView, DirectX::Colors::AliceBlue);
	g_pDeviceContext->ClearDepthStencilView(g_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0.f);

	g_pDeviceContext->VSSetShader(g_pVertexShader, NULL, 0);
	g_pDeviceContext->PSSetShader(g_pPixelShader, NULL, 0);

	ConstantBuffer cb;
	cb.World = XMMatrixTranspose(g_World);
	cb.View = XMMatrixTranspose(g_View);
	cb.Projection = XMMatrixTranspose(g_Projection);
	g_pDeviceContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cb, 0, 0);
	g_pDeviceContext->VSSetConstantBuffers(0, 1, &g_pConstantBuffer);
	g_pDeviceContext->VSSetConstantBuffers(1, 1, &g_pLightBuffer);

	LightBuffer lb;
	lb.LightDir = XMFLOAT4(-1.0f, 1.f, -1.f, 0.f);
	lb.LightColor = XMFLOAT4(1.f, 1.f, 1.f, 1.f);
	g_pDeviceContext->UpdateSubresource(g_pLightBuffer, 0, NULL, &lb, 0, 0);
	g_pDeviceContext->PSSetConstantBuffers(1, 1, &g_pLightBuffer);
	g_pDeviceContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
	g_pDeviceContext->PSSetShaderResources(0, 1, &g_pTextureResourceView);

	g_pDeviceContext->DrawIndexed(12, 0, 0);

	g_pSwapChain->Present(0, 0);
}

void CleanupDevice()
{
	if (g_pDeviceContext) g_pDeviceContext->ClearState();
	if (g_pSamplerLinear) g_pSamplerLinear->Release();
	if (g_pTextureResourceView) g_pTextureResourceView->Release();
	if (g_pLightBuffer) g_pLightBuffer->Release();
	if (g_pConstantBuffer) g_pConstantBuffer->Release();
	if (g_pIndexBuffer) g_pIndexBuffer->Release();
	if (g_pVertexBuffer) g_pVertexBuffer->Release();
	if (g_pInputLayout) g_pInputLayout->Release();
	if (g_pPixelShader) g_pPixelShader->Release();
	if (g_pVertexShader) g_pVertexShader->Release();
	if (g_pDepthStencilTex) g_pDepthStencilTex->Release();
	if (g_pDepthStencilView) g_pDepthStencilView->Release();
	if (g_pRenderTargetView) g_pRenderTargetView->Release();
	if (g_pSwapChain) g_pSwapChain->Release();
	if (g_pDeviceContext) g_pDeviceContext->Release();
	if (g_pDevice) g_pDevice->Release();
}