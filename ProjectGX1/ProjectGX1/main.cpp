// CGS HW Project A "Line Land".
// Author: L.Norri CD CGS, FullSail University

// Introduction:
// Welcome to the hardware project of the Computer Graphics Systems class.
// This assignment is fully guided but still requires significant effort on your part. 
// Future classes will demand the habits & foundation you develop right now!  
// It is CRITICAL that you follow each and every step. ESPECIALLY THE READING!!!

// TO BEGIN: Open the word document that acompanies this project and start from the top.

//************************************************************
//************ INCLUDES & DEFINES ****************************
//************************************************************


#include <iostream>
#include <ctime>
#include "XTime.h"


using namespace std;

// BEGIN PART 1
// TODO: PART 1 STEP 1a
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"d3d11.lib")
#include <d3d11.h>
//#include <dxgi.h>
#include <atlbase.h>


//D3D11CreateDeviceAndSwapChain();

// TODO: PART 1 STEP 1b
#include <DirectXMath.h>
//#include <DirectXPackedVector.h>
//#include <DirectXColors.h>
//#include <DirectXCollision.h>
using namespace DirectX;

// TODO: PART 2 STEP 6
#include "Trivial_PS.csh"
#include "Trivial_VS.csh"

#define BACKBUFFER_WIDTH	500
#define BACKBUFFER_HEIGHT	500
#define PI 3.14159f
#define CIRCLE_VERTS 360

//************************************************************
//************ SIMPLE WINDOWS APP CLASS **********************
//************************************************************

class DEMO_APP
{	
	HINSTANCE						application;
	WNDPROC							appWndProc;
	HWND							window;
	// TODO: PART 1 STEP 2
	CComPtr<ID3D11Device> d3Device 				;
	CComPtr<ID3D11DeviceContext> d3DeviceContext;
	CComPtr<ID3D11RenderTargetView> d3RTV		;
	CComPtr<IDXGISwapChain> d3SwapChain			;

	D3D11_VIEWPORT viewPort;
	D3D_FEATURE_LEVEL fLevel = D3D_FEATURE_LEVEL_11_0;
	DXGI_SWAP_CHAIN_DESC dxSwapChainDesc;
	
	// TODO: PART 2 STEP 2
	ID3D11Buffer* d3Buffer;
	ID3D11Buffer* d3Buffer2;
	XTime xTimeInst;
	unsigned int dxNumVerts = 360;
	// BEGIN PART 5
	// TODO: PART 5 STEP 1
	ID3D11Buffer* d3TriangleBuffer;
	unsigned int dxNumTverts = 10 * 20 * 2 * 3;
	unsigned int dxNumTriangles = 10 * 20 * 2;

	// TODO: PART 2 STEP 4
	CComPtr<ID3D11VertexShader> vertexShader;
	CComPtr<ID3D11PixelShader> pixelShader;
	CComPtr<ID3D11InputLayout> d3InputLayout;

	// BEGIN PART 3
	// TODO: PART 3 STEP 1

	// TODO: PART 3 STEP 2b
	struct SEND_TO_VRAM
	{
		XMFLOAT4 constantColor;
		XMFLOAT2 constantOffset;
		XMFLOAT2 padding;
	};
	// TODO: PART 3 STEP 4a
	SEND_TO_VRAM toShader;
	SEND_TO_VRAM BGtoShader;
	
public:
	// BEGIN PART 2
	// TODO: PART 2 STEP 1
	struct SIMPLE_VERTEX
	{
		XMFLOAT2 point = { 0,0 };
	};

	struct triangle
	{
			XMFLOAT2 point1 = { 0,0 };
			XMFLOAT2 point2 = { 0,0 };
			XMFLOAT2 point3 = { 0,0 };
	};
	//bouncing
	double bounceX = 1, bounceY = 1;
	bool shiftedX = false;
	bool shiftedY = false;

	//pulse
	bool decrease = false;
	
	DEMO_APP(HINSTANCE hinst, WNDPROC proc);
	bool Run();
	bool ShutDown();
};

//************************************************************
//************ CREATION OF OBJECTS & RESOURCES ***************
//************************************************************

DEMO_APP::DEMO_APP(HINSTANCE hinst, WNDPROC proc)
{
	// ****************** BEGIN WARNING ***********************// 
	// WINDOWS CODE, I DON'T TEACH THIS YOU MUST KNOW IT ALREADY! 
	application = hinst; 
	appWndProc = proc; 

	WNDCLASSEX  wndClass;
    ZeroMemory( &wndClass, sizeof( wndClass ) );
    wndClass.cbSize         = sizeof( WNDCLASSEX );             
    wndClass.lpfnWndProc    = appWndProc;						
    wndClass.lpszClassName  = L"DirectXApplication";            
	wndClass.hInstance      = application;		               
    wndClass.hCursor        = LoadCursor( NULL, IDC_ARROW );    
    wndClass.hbrBackground  = ( HBRUSH )( COLOR_WINDOWFRAME ); 
	//wndClass.hIcon			= LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_FSICON));
    RegisterClassEx( &wndClass );

	RECT window_size = { 0, 0, BACKBUFFER_WIDTH, BACKBUFFER_HEIGHT };
	AdjustWindowRect(&window_size, WS_OVERLAPPEDWINDOW, false);

	window = CreateWindow(	L"DirectXApplication", L"CGS Hardware Project",	WS_OVERLAPPEDWINDOW & ~(WS_THICKFRAME|WS_MAXIMIZEBOX), 
							CW_USEDEFAULT, CW_USEDEFAULT, window_size.right-window_size.left, window_size.bottom-window_size.top,					
							NULL, NULL,	application, this );												

    ShowWindow( window, SW_SHOW );
	//********************* END WARNING ************************//

	// TODO: PART 1 STEP 3a
	ZeroMemory(&dxSwapChainDesc, sizeof(dxSwapChainDesc));
	dxSwapChainDesc.BufferCount = 1;

	dxSwapChainDesc.BufferDesc.Width = BACKBUFFER_WIDTH;
	dxSwapChainDesc.BufferDesc.Height = BACKBUFFER_HEIGHT;
	dxSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxSwapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
	dxSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	dxSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	dxSwapChainDesc.OutputWindow = window;
	dxSwapChainDesc.SampleDesc.Count = 1;
	dxSwapChainDesc.SampleDesc.Quality = 0;
	dxSwapChainDesc.Windowed = true;
	dxSwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	dxSwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	dxSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	dxSwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	// TODO: PART 1 STEP 3b
	UINT flag = 0;
#if _DEBUG
		flag = D3D11_CREATE_DEVICE_DEBUG;
#else
		flag = 0;
#endif
		D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &fLevel, 1,D3D11_SDK_VERSION,
								  &dxSwapChainDesc, &d3SwapChain, &d3Device, NULL, &d3DeviceContext);
	// TODO: PART 1 STEP 4
		ID3D11Texture2D *scTexture;
		d3SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&scTexture);
		d3Device->CreateRenderTargetView(scTexture, NULL, &d3RTV);
		scTexture->Release();
		scTexture = 0;

	// TODO: PART 1 STEP 5
		viewPort.Width =  (float)BACKBUFFER_WIDTH;
		viewPort.Height = (float)BACKBUFFER_HEIGHT;
		viewPort.MinDepth = 0.0f;
		viewPort.MaxDepth = 1.0f;
		viewPort.TopLeftX = 0.0f;
		viewPort.TopLeftY = 0.0f;
		
	// TODO: PART 2 STEP 3a
		SIMPLE_VERTEX circleVerts[360];
		
		for (unsigned int i = 0; i < dxNumVerts; i++)
		{
			circleVerts[i].point.x = sin(i*PI / 180);
			circleVerts[i].point.y = cos(i*PI / 180);
		}

	// BEGIN PART 4
	// TODO: PART 4 STEP 1
		for (unsigned int i = 0; i < dxNumVerts; i++)
		{
			circleVerts[i].point.x *= 0.2f;
			circleVerts[i].point.y *= 0.2f;
		}

	// TODO: PART 2 STEP 3b
		D3D11_BUFFER_DESC cBuffer;
		cBuffer.Usage = D3D11_USAGE_IMMUTABLE;
		cBuffer.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		cBuffer.CPUAccessFlags = NULL;
		cBuffer.ByteWidth = sizeof(SIMPLE_VERTEX)*dxNumVerts;
		cBuffer.StructureByteStride = sizeof(SIMPLE_VERTEX);
		cBuffer.MiscFlags = NULL;

    // TODO: PART 2 STEP 3c
		D3D11_SUBRESOURCE_DATA srData = { 0 };
		srData.pSysMem = circleVerts;
	// TODO: PART 2 STEP 3d
		d3Device->CreateBuffer(&cBuffer, &srData, &d3Buffer);
	// TODO: PART 5 STEP 2a
		D3D11_BUFFER_DESC tBuffer;
		tBuffer.Usage = D3D11_USAGE_IMMUTABLE;
		tBuffer.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		tBuffer.CPUAccessFlags = NULL;
		tBuffer.ByteWidth = sizeof(triangle)*dxNumTriangles;
		tBuffer.StructureByteStride = sizeof(triangle);
		tBuffer.MiscFlags = NULL; 
	// TODO: PART 5 STEP 2b
		triangle triangleVerts[400];
		float sqOffsetX = 0, sqOffsetY = 0;
		unsigned int triNum = 0;

		for (unsigned int j = 0; j < 20; j++)
		{
			for (unsigned int i = 0; i < 10; i++)
			{
					triangleVerts[0 + triNum].point1.x = -1.0f + sqOffsetX;
					triangleVerts[0 + triNum].point1.y =  0.9f + sqOffsetY;
					triangleVerts[0 + triNum].point2.x = -1.0f + sqOffsetX;
					triangleVerts[0 + triNum].point2.y =  1.0f + sqOffsetY;
					triangleVerts[0 + triNum].point3.x = -0.9f + sqOffsetX;
					triangleVerts[0 + triNum].point3.y =  1.0f + sqOffsetY;
					triangleVerts[1 + triNum].point1.x = -1.0f + sqOffsetX;
					triangleVerts[1 + triNum].point1.y =  0.9f + sqOffsetY;
					triangleVerts[1 + triNum].point2.x = -0.9f + sqOffsetX;
					triangleVerts[1 + triNum].point2.y =  1.0f + sqOffsetY;
					triangleVerts[1 + triNum].point3.x = -0.9f + sqOffsetX;
					triangleVerts[1 + triNum].point3.y =  0.9f + sqOffsetY;
					triNum += 2;
					sqOffsetX += 0.2f;
			}

			if ((j + 1) % 2)
				sqOffsetX = 0.1f;
			else
				sqOffsetX = 0;

			sqOffsetY -= 0.1f;
		}

	// TODO: PART 5 STEP 3
		D3D11_SUBRESOURCE_DATA trData = { 0 };
		trData.pSysMem = triangleVerts;

		d3Device->CreateBuffer(&tBuffer, &trData, &d3TriangleBuffer);
	// TODO: PART 2 STEP 5
	// ADD SHADERS TO PROJECT, SET BUILD OPTIONS & COMPILE

	// TODO: PART 2 STEP 7
		d3Device->CreateVertexShader(Trivial_VS, sizeof(Trivial_VS), NULL, &vertexShader);
		d3Device->CreatePixelShader(Trivial_PS, sizeof(Trivial_PS), NULL, &pixelShader);
	// TODO: PART 2 STEP 8a
		D3D11_INPUT_ELEMENT_DESC layout[]
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};
	
	// TODO: PART 2 STEP 8b
		d3Device->CreateInputLayout(layout, 1, Trivial_VS, sizeof(Trivial_VS), &d3InputLayout);
	// TODO: PART 3 STEP 3
		D3D11_BUFFER_DESC cBuffer2;
		cBuffer2.Usage = D3D11_USAGE_DYNAMIC;
		cBuffer2.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cBuffer2.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cBuffer2.ByteWidth = sizeof(SEND_TO_VRAM);
		cBuffer2.StructureByteStride = sizeof(SEND_TO_VRAM);
		cBuffer2.MiscFlags = NULL;

		d3Device->CreateBuffer(&cBuffer2, NULL, &d3Buffer2);
	// TODO: PART 3 STEP 4b
		XMFLOAT4 color = { 1.0f, 1.0f, 0, 0 };
		XMFLOAT2 offset = { 0,0 };
		toShader.constantColor = color;
		toShader.constantOffset = offset;

		XMFLOAT4 bgColor = { 0, 0, 0, 0 };
		XMFLOAT2 bgOffset = { 0,0 };
		BGtoShader.constantColor = bgColor;
		BGtoShader.constantOffset = bgOffset;

}

//************************************************************
//************ EXECUTION *************************************
//************************************************************

bool DEMO_APP::Run()
{
	// TODO: PART 4 STEP 2	
	xTimeInst.Signal();
	double deltaTime = xTimeInst.Delta();


	// TODO: PART 4 STEP 3
	toShader.constantOffset.x += 1.0f*deltaTime*bounceX;
	toShader.constantOffset.y += 0.5f*deltaTime*bounceY;
	// TODO: PART 4 STEP 5
	if ((BGtoShader.constantColor.x >= 0  || BGtoShader.constantColor.y >= 0 || BGtoShader.constantColor.z >= 0) && decrease)
	{
		if (BGtoShader.constantColor.x >= 0)
		{
			BGtoShader.constantColor.x -= 1.0f*deltaTime;
		}
		if(BGtoShader.constantColor.y >= 0)
		{
			BGtoShader.constantColor.y -= 1.0f*deltaTime;
		}
		if (BGtoShader.constantColor.z >= 0)
		{
			BGtoShader.constantColor.z -= 1.0f*deltaTime;
		}

		if (BGtoShader.constantColor.x <= 0 && BGtoShader.constantColor.y <= 0 && BGtoShader.constantColor.z <= 0)
			decrease = false;
	}
	

	if (toShader.constantOffset.x >= 0.8f || toShader.constantOffset.x <= -0.8f || toShader.constantOffset.y >= 0.8f || toShader.constantOffset.y <= -0.8f)
	{
		if (!shiftedX && toShader.constantOffset.x >= 0.8f)
		{
			shiftedX = true;
			bounceX *= -1;
			BGtoShader.constantColor.x = 0.5f;
		}
		else if (shiftedX && toShader.constantOffset.x <= -0.8f)
		{
			shiftedX = false;
			bounceX *= -1;
			BGtoShader.constantColor.y = 0.5f;
		}
		else if (!shiftedY && toShader.constantOffset.y >= 0.8f)
		{
			shiftedY = true;
			bounceY *= -1;
			BGtoShader.constantColor.z = 0.5f;
			BGtoShader.constantColor.y = 0.5f;
		}
		else if (shiftedY && toShader.constantOffset.y <= -0.8f)
		{
			shiftedY = false;
			bounceY *= -1;
			BGtoShader.constantColor.x = 0.5f;
			BGtoShader.constantColor.y = 0.5f;
		}
		decrease = true;
		xTimeInst.Restart();

	}


	// END PART 4

	// TODO: PART 1 STEP 7a
	ID3D11RenderTargetView* tempArray[1] = {d3RTV};
	d3DeviceContext->OMSetRenderTargets(1, tempArray, NULL);

	// TODO: PART 1 STEP 7b
	d3DeviceContext->RSSetViewports(1, &viewPort);

	// TODO: PART 1 STEP 7c
	float color[4] = {0, 0, 0.5f, 0};
	d3DeviceContext->ClearRenderTargetView(d3RTV, color);
	// TODO: PART 5 STEP 4
	unsigned int tempSize = sizeof(XMFLOAT2);
	unsigned int zero = 0;
	//ID3D11Buffer* buffers[] = {d3Buffer, d3TriangleBuffer};
	//d3DeviceContext->IASetVertexBuffers(0, 1, &d3TriangleBuffer, &tempSize, &zero);
	// TODO: PART 5 STEP 5
	D3D11_MAPPED_SUBRESOURCE mappedSR;
	ZeroMemory(&mappedSR, sizeof(mappedSR));
	d3DeviceContext->Map(d3Buffer2, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mappedSR);
	memcpy(mappedSR.pData, &BGtoShader, sizeof(BGtoShader));	
	d3DeviceContext->VSSetConstantBuffers(0, 1, &d3Buffer2);
	d3DeviceContext->IASetVertexBuffers(0, 1, &d3TriangleBuffer, &tempSize, &zero);
	d3DeviceContext->VSSetShader(vertexShader, NULL, NULL);
	d3DeviceContext->PSSetShader(pixelShader, NULL, NULL);
	d3DeviceContext->IASetInputLayout(d3InputLayout);
	d3DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	d3DeviceContext->Draw(1200, 0);
	//d3SwapChain->Present(1, 0);
	//d3DeviceContext->Unmap(d3Buffer2, NULL);
	// TODO: PART 5 STEP 6
	
	// TODO: PART 5 STEP 7
	
	// END PART 5
	
	// TODO: PART 3 STEP 5
	tempSize = sizeof(SIMPLE_VERTEX);

	ZeroMemory(&mappedSR, sizeof(mappedSR));
	d3DeviceContext->Map(d3Buffer2, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mappedSR);
	memcpy(mappedSR.pData, &toShader, sizeof(toShader));

	// TODO: PART 3 STEP 6
	d3DeviceContext->VSSetConstantBuffers(0, 1, &d3Buffer2);
	// TODO: PART 2 STEP 9a
	d3DeviceContext->IASetVertexBuffers(0, 1, &d3Buffer, &tempSize, &zero);

	// TODO: PART 2 STEP 9b
	d3DeviceContext->VSSetShader(vertexShader, NULL, NULL);
	d3DeviceContext->PSSetShader(pixelShader, NULL, NULL);

	// TODO: PART 2 STEP 9c
	d3DeviceContext->IASetInputLayout(d3InputLayout);

	// TODO: PART 2 STEP 9d
	d3DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

	// TODO: PART 2 STEP 10
	d3DeviceContext->Draw(360, 0);
	
	// END PART 2
	d3DeviceContext->Unmap(d3Buffer2, NULL);

	// TODO: PART 1 STEP 8
	d3SwapChain->Present(1, 0);
	// END OF PART 1
	return true; 
}

//************************************************************
//************ DESTRUCTION ***********************************
//************************************************************

bool DEMO_APP::ShutDown()
{
	// TODO: PART 1 STEP 6
	d3DeviceContext->ClearState();

	UnregisterClass( L"DirectXApplication", application ); 
	return true;
}

//************************************************************
//************ WINDOWS RELATED *******************************
//************************************************************

// ****************** BEGIN WARNING ***********************// 
// WINDOWS CODE, I DON'T TEACH THIS YOU MUST KNOW IT ALREADY!
	
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine,	int nCmdShow );						   
LRESULT CALLBACK WndProc(HWND hWnd,	UINT message, WPARAM wparam, LPARAM lparam );		
int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE, LPTSTR, int )
{
	srand(unsigned int(time(0)));
	DEMO_APP myApp(hInstance,(WNDPROC)WndProc);	
    MSG msg; ZeroMemory( &msg, sizeof( msg ) );
    while ( msg.message != WM_QUIT && myApp.Run() )
    {	
	    if ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
        { 
            TranslateMessage( &msg );
            DispatchMessage( &msg ); 
        }
    }
	myApp.ShutDown(); 
	return 0; 
}
LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    if(GetAsyncKeyState(VK_ESCAPE))
		message = WM_DESTROY;
    switch ( message )
    {
        case ( WM_DESTROY ): { PostQuitMessage( 0 ); }
        break;
    }
    return DefWindowProc( hWnd, message, wParam, lParam );
}
//********************* END WARNING ************************//