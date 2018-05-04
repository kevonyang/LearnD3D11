// Learn05.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "Learn05.h"
#include <d3d11_2.h>
#include <D3DX11.h>
#include <d3dcompiler.h>
#include <DirectXColors.h>

#define MAX_LOADSTRING 100

struct SimpleVertex
{
	DirectX::XMFLOAT3 Pos;
	DirectX::XMFLOAT4 Color;
};

struct ConstantBuffer
{
	DirectX::XMMATRIX mWorld;
	DirectX::XMMATRIX mView;
	DirectX::XMMATRIX mProjection;
};

// 全局变量: 
HINSTANCE hInst;								// 当前实例
TCHAR szTitle[MAX_LOADSTRING];					// 标题栏文本
TCHAR szWindowClass[MAX_LOADSTRING];			// 主窗口类名
HWND hWnd;
ID3D11Device* g_pD3DDevice;
ID3D11DeviceContext* g_pD3DDeviceContext;
IDXGISwapChain* g_pSwapChain;
D3D_FEATURE_LEVEL g_featureLevel;
ID3D11RenderTargetView* g_pRenderTargetView;
ID3D11Texture2D* g_pDepthStencil;
ID3D11DepthStencilView* g_pDepthStencilView;
ID3D11VertexShader* g_pVertexShader;
ID3D11PixelShader* g_pPixelShader;
ID3D11InputLayout* g_pInputLayout;
ID3D11Buffer* g_pVertexBuffer;
ID3D11Buffer* g_pIndexBuffer;
ID3D11Buffer* g_pConstantBuffer;
DirectX::XMMATRIX g_World;
DirectX::XMMATRIX g_View;
DirectX::XMMATRIX g_Projection;


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
	LoadString(hInstance, IDC_LEARN05, szWindowClass, MAX_LOADSTRING);
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
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LEARN05));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_LEARN05);
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
	HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createFlags, featureLevels, ARRAYSIZE(featureLevels), D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pD3DDevice, &g_featureLevel, &g_pD3DDeviceContext);
	if (FAILED(hr))
		return hr;

	ID3D11Texture2D* pBackBuffer;
	hr = g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer);
	if (FAILED(hr))
		return hr;
	hr = g_pD3DDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_pRenderTargetView);
	pBackBuffer->Release();
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
	hr = g_pD3DDevice->CreateTexture2D(&td, nullptr, &g_pDepthStencil);
	if (FAILED(hr))
		return hr;
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
	ZeroMemory(&dsvd, sizeof(dsvd));
	dsvd.Format = td.Format;
	dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvd.Texture2D.MipSlice = 0;
	g_pD3DDevice->CreateDepthStencilView(g_pDepthStencil, &dsvd, &g_pDepthStencilView);
	if (FAILED(hr))
		return hr;

	g_pD3DDeviceContext->OMSetRenderTargets(1, &g_pRenderTargetView, g_pDepthStencilView);

	D3D11_VIEWPORT vp;
	vp.Width = width;
	vp.Height = height;
	vp.MaxDepth = 1.0f;
	vp.MinDepth = 0.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	g_pD3DDeviceContext->RSSetViewports(1, &vp);

	UINT shaderFlags = 0;
#if defined(_DEBUG)
	shaderFlags |= D3DCOMPILE_DEBUG;
#endif
	ID3DBlob *pVSBlob, *pErrBlob, *pPSBlob;
	hr = D3DX11CompileFromFile(L"Learn05.fx", NULL, NULL, "VS", "vs_4_0", shaderFlags, 0, NULL, &pVSBlob, &pErrBlob, NULL);
	if (FAILED(hr))
	{
		if (pErrBlob)
			pErrBlob->Release();
		return hr;
	}

	hr = g_pD3DDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL, &g_pVertexShader);
	if (FAILED(hr))
		return hr;

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	hr = g_pD3DDevice->CreateInputLayout(layout, ARRAYSIZE(layout), pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &g_pInputLayout);
	pVSBlob->Release();
	if (FAILED(hr))
		return hr;
	g_pD3DDeviceContext->IASetInputLayout(g_pInputLayout);

	hr = D3DX11CompileFromFile(L"Learn05.fx", NULL, NULL, "PS", "ps_4_0", shaderFlags, 0, NULL, &pPSBlob, &pErrBlob, NULL);
	if (FAILED(hr))
	{
		if (pErrBlob)
			pErrBlob->Release();
		return hr;
	}
	hr = g_pD3DDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), NULL, &g_pPixelShader);
	pPSBlob->Release();
	if (FAILED(hr))
		return hr;

	/*SimpleVertex vertices[] =
	{
		{ DirectX::XMFLOAT3(0.f, 0.f, 0.f), DirectX::XMFLOAT4(0.f, 1.f, 1.f, 1.f) },
		{ DirectX::XMFLOAT3(2.f, 0.f, 0.f), DirectX::XMFLOAT4(0.f, 1.f, 1.f, 1.f) },
		{ DirectX::XMFLOAT3(2.f, 2.f, 0.f), DirectX::XMFLOAT4(0.f, 1.f, 1.f, 1.f) },
		{ DirectX::XMFLOAT3(0.f, 2.f, 0.f), DirectX::XMFLOAT4(0.f, 1.f, 1.f, 1.f) },
	};*/
	SimpleVertex vertices[] =
	{
		{ DirectX::XMFLOAT3(0.f, 0.f, 0.1f), DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
		{ DirectX::XMFLOAT3(0.f, 0.5f, 0.1f), DirectX::XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f) },
		{ DirectX::XMFLOAT3(0.5f, 0.f, 0.1f), DirectX::XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f) },
		{ DirectX::XMFLOAT3(0.5f, 0.5f, 0.1f), DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
	};
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.ByteWidth = sizeof(SimpleVertex) * 4;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	bd.Usage = D3D11_USAGE_DEFAULT;
	D3D11_SUBRESOURCE_DATA srd;
	ZeroMemory(&srd, sizeof(srd));
	srd.pSysMem = vertices;
	hr = g_pD3DDevice->CreateBuffer(&bd, &srd, &g_pVertexBuffer);
	if (FAILED(hr))
		return hr;

	UINT strides = sizeof(SimpleVertex);
	UINT offset = 0;
	g_pD3DDeviceContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &strides, &offset);

	WORD indices[] =
	{
		0, 1, 2,
		2, 1, 3,
	};
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.ByteWidth = sizeof(WORD) * 6;
	srd.pSysMem = indices;
	hr = g_pD3DDevice->CreateBuffer(&bd, &srd, &g_pIndexBuffer);
	if (FAILED(hr))
		return hr;
	
	g_pD3DDeviceContext->IASetIndexBuffer(g_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	g_pD3DDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.ByteWidth = sizeof(ConstantBuffer);
	hr=g_pD3DDevice->CreateBuffer(&bd, NULL, &g_pConstantBuffer);
	if (FAILED(hr))
		return hr;

	DirectX::XMVECTOR eye = DirectX::XMVectorSet(0.f, 0.f, -10.f, 0.f);
	DirectX::XMVECTOR focus = DirectX::XMVectorSet(0.f, 0.f, 1.f, 0.f);
	DirectX::XMVECTOR up = DirectX::XMVectorSet(0.f, 1.f, 0.f, 0.f);
	
	g_World = DirectX::XMMatrixIdentity();
	g_View = DirectX::XMMatrixLookAtLH(eye, focus, up);
	g_Projection = DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV2, width / (float)height, 0.1f, 100.0f);

	return S_OK;
}

void Render()
{
	static float t = 0;
	static DWORD timeStart = 0;
	DWORD timeCur = GetTickCount();
	if (timeStart == 0)
		timeStart = timeCur;
	t = (timeCur - timeStart) / 1000.f;

	g_pD3DDeviceContext->ClearRenderTargetView(g_pRenderTargetView, DirectX::Colors::AliceBlue);
	g_pD3DDeviceContext->ClearDepthStencilView(g_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	ConstantBuffer cb;
	cb.mWorld = DirectX::XMMatrixTranspose(DirectX::XMMatrixRotationY(t));
	cb.mView = DirectX::XMMatrixTranspose(g_View);
	cb.mProjection = DirectX::XMMatrixTranspose(g_Projection);
	g_pD3DDeviceContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cb, NULL, NULL);

	g_pD3DDeviceContext->VSSetShader(g_pVertexShader, NULL, 0);
	g_pD3DDeviceContext->VSSetConstantBuffers(0, 1, &g_pConstantBuffer);
	g_pD3DDeviceContext->PSSetShader(g_pPixelShader, NULL, 0);
	g_pD3DDeviceContext->DrawIndexed(6, 0, 0);

	ConstantBuffer cb1;
	cb1.mWorld = DirectX::XMMatrixTranspose(DirectX::XMMatrixTranslation(-5.f, 0.f, 0.f) * DirectX::XMMatrixRotationY(t));
	cb1.mView = DirectX::XMMatrixTranspose(g_View);
	cb1.mProjection = DirectX::XMMatrixTranspose(g_Projection);
	g_pD3DDeviceContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cb1, NULL, NULL);
	g_pD3DDeviceContext->DrawIndexed(6, 0, 0);

	g_pSwapChain->Present(0, 0);
}

void CleanupDevice()
{
	if (g_pD3DDeviceContext) g_pD3DDeviceContext->ClearState();
	if (g_pVertexBuffer) g_pVertexBuffer->Release();
	if (g_pIndexBuffer) g_pIndexBuffer->Release();
	if (g_pConstantBuffer) g_pConstantBuffer->Release();
	if (g_pInputLayout) g_pInputLayout->Release();
	if (g_pVertexShader) g_pVertexShader->Release();
	if (g_pPixelShader) g_pPixelShader->Release();
	if (g_pDepthStencilView) g_pDepthStencilView->Release();
	if (g_pRenderTargetView) g_pRenderTargetView->Release();
	if (g_pSwapChain) g_pSwapChain->Release();
	if (g_pD3DDeviceContext) g_pD3DDeviceContext->Release();
	if (g_pD3DDevice) g_pD3DDevice->Release();
}