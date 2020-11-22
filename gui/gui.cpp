#include <d3d12.h>
#include <dxgi1_4.h>
#include <tchar.h>
#include <d3d12.h>
#include <vector>
#include <chrono>
#include <iostream>

#include "..\imgui\imgui.h"
#include "..\imgui\imgui_impl_win32.h"
#include "..\imgui\imgui_impl_dx12.h"
#include "..\gui\gui.h"
#include "../utility/xor.hpp"


struct FrameContext
{
	ID3D12CommandAllocator* CommandAllocator;
	UINT64                  FenceValue;
};

// Data
static int const                    NUM_FRAMES_IN_FLIGHT = 3;
static FrameContext                 g_frameContext[NUM_FRAMES_IN_FLIGHT] = {};
static UINT                         g_frameIndex = 0;

static int const                    NUM_BACK_BUFFERS = 3;
static ID3D12Device* g_pd3dDevice = NULL;
static ID3D12DescriptorHeap* g_pd3dRtvDescHeap = NULL;
static ID3D12DescriptorHeap* g_pd3dSrvDescHeap = NULL;
static ID3D12CommandQueue* g_pd3dCommandQueue = NULL;
static ID3D12GraphicsCommandList* g_pd3dCommandList = NULL;
static ID3D12Fence* g_fence = NULL;
static HANDLE                       g_fenceEvent = NULL;
static UINT64                       g_fenceLastSignaledValue = 0;
static IDXGISwapChain3* g_pSwapChain = NULL;
static HANDLE                       g_hSwapChainWaitableObject = NULL;
static ID3D12Resource* g_mainRenderTargetResource[NUM_BACK_BUFFERS] = {};
static D3D12_CPU_DESCRIPTOR_HANDLE  g_mainRenderTargetDescriptor[NUM_BACK_BUFFERS] = {};
HWND hwnd;
ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
std::chrono::steady_clock::time_point begin;

ImFont* beyno_font_large = nullptr;
ImFont* beyno_font_small = nullptr;
ImFont* tit_font_small = nullptr;
ImFont* tit_font_large = nullptr;
int horizontal = 0;
int vertical = 0;

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
void WaitForLastSubmittedFrame();
FrameContext* WaitForNextFrameResources();
void ResizeSwapChain(HWND hWnd, int width, int height);
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
WNDCLASSEX wc;


void begin_draw() {
	// Start the Dear ImGui frame
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void end_draw() {
	// Rendering
	FrameContext* frameCtxt = WaitForNextFrameResources();
	UINT backBufferIdx = g_pSwapChain->GetCurrentBackBufferIndex();
	frameCtxt->CommandAllocator->Reset();

	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = g_mainRenderTargetResource[backBufferIdx];
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

	g_pd3dCommandList->Reset(frameCtxt->CommandAllocator, NULL);
	g_pd3dCommandList->ResourceBarrier(1, &barrier);
	g_pd3dCommandList->ClearRenderTargetView(g_mainRenderTargetDescriptor[backBufferIdx], (float*)&clear_color, 0, NULL);
	g_pd3dCommandList->OMSetRenderTargets(1, &g_mainRenderTargetDescriptor[backBufferIdx], FALSE, NULL);
	g_pd3dCommandList->SetDescriptorHeaps(1, &g_pd3dSrvDescHeap);
	ImGui::Render();
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), g_pd3dCommandList);
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	g_pd3dCommandList->ResourceBarrier(1, &barrier);
	g_pd3dCommandList->Close();

	g_pd3dCommandQueue->ExecuteCommandLists(1, (ID3D12CommandList* const*)&g_pd3dCommandList);

	g_pSwapChain->Present(1, 0); // Present with vsync
	//g_pSwapChain->Present(0, 0); // Present without vsync

	UINT64 fenceValue = g_fenceLastSignaledValue + 1;
	g_pd3dCommandQueue->Signal(g_fence, fenceValue);
	g_fenceLastSignaledValue = fenceValue;
	frameCtxt->FenceValue = fenceValue;
}

void menu_end() {
	WaitForLastSubmittedFrame();
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	CleanupDeviceD3D();
	::DestroyWindow(hwnd);
	::UnregisterClass(wc.lpszClassName, wc.hInstance);
}

void menu_loop() {
	auto rgb = [](float r, float g, float b, float a) { return ImVec4(r / 255, g / 255, b / 255, a / 255); };
	long long time_passed = 0;

	MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	while (msg.message != WM_QUIT)
	{
		if (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
			continue;
		}

		begin_draw();

		ImGui::SetNextWindowPos(ImVec2(-1, -1));
		ImGui::SetNextWindowSize(ImVec2(400, 300));
		static const auto flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar;
		ImGui::Begin("###Overflow", 0, flags);
		{
			ImGui::PushFont(beyno_font_large);
			ImGui::SetCursorPosX((375 / 2) - (ImGui::CalcTextSize("OVERFLOW").x / 2));
			ImGui::SetCursorPosY(50);
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(rgb(237, 74, 74, 255)));
			ImGui::Text("OVERFLOW");
			ImGui::PopStyleColor();
			ImGui::PopFont();			

			ImGui::PushFont(tit_font_large);
			std::string status = "loading...";

			std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();

			time_passed = std::chrono::duration_cast<std::chrono::seconds> (now - begin).count();
			if (time_passed > 4) {
				status = "installing dependencies...";
				if (time_passed == 5)
					status = "installing dependencies.";
				else if (time_passed == 6)
					status = "installing dependencies..";
				else if (time_passed == 7)
					status = "installing dependencies...";
			}
			else {
				if (time_passed == 1)
					status = "loading.";
				else if (time_passed == 2)
					status = "loading..";
				else if (time_passed == 3)
					status = "loading...";
				else if (time_passed > 3 && time_passed < 4)
					status = "loading.";
			}
			
			if (time_passed > 16) {
				status = "fatal error: failed to install DX11";
			}

			ImGui::SetCursorPosX((375 / 2) - (ImGui::CalcTextSize(status.c_str()).x / 2));
			ImGui::SetCursorPosY(ImGui::CalcTextSize("OVERFLOW").y + 50 + 25);
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(rgb(255, 255, 255, 255)));

			ImGui::Text(status.c_str());
			int y;
			if (horizontal == 2256 && vertical == 1504)
				y = 300 - ImGui::CalcTextSize("NMan7#2091 | OVERFLOW.RED | UNDETECTED").y - 55;
			else
				y = 300 - ImGui::CalcTextSize("NMan7#2091 | OVERFLOW.RED | UNDETECTED").y - 45;

			ImGui::PushFont(tit_font_small);
			ImGui::SetCursorPosY(y);
			ImGui::SetCursorPosX((365 / 2) - (ImGui::CalcTextSize("NMan7#2091 | OVERFLOW.RED | UNDETECTED").x / 2));
			ImGui::Text("NMan7#2091  |  ");
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(rgb(50, 205, 50, 255)));
			ImGui::SameLine();
			ImGui::Text("UNDETECTED  ");
			ImGui::PopStyleColor();

			ImGui::SameLine();
			ImGui::Text("|  OVERFLOW.RED");

			ImGui::PopStyleColor();
			ImGui::PopFont();
			ImGui::PopFont();
		}
		ImGui::End();

		end_draw();

		if (time_passed >= 20)
			break;
	}
	menu_end();
}

// Get the horizontal and vertical screen sizes in pixel
void GetDesktopResolution(int& horizontal, int& vertical)
{
	RECT desktop;
	// Get a handle to the desktop window
	const HWND hDesktop = GetDesktopWindow();
	// Get the size of screen to the variable desktop
	GetWindowRect(hDesktop, &desktop);
	// The top left corner will have coordinates (0,0)
	// and the bottom right corner will have coordinates
	// (horizontal, vertical)
	horizontal = desktop.right;
	vertical = desktop.bottom;
}

void menu_init() {
	GetDesktopResolution(horizontal, vertical);

	wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("Overflow Cheats Loader"), NULL };
	::RegisterClassEx(&wc);
	hwnd = ::CreateWindow(wc.lpszClassName, _T("Overflow Cheats Loader"), WS_OVERLAPPED | WS_MINIMIZEBOX | WS_SYSMENU, (horizontal / 2) - (400 / 2), (vertical / 2) - (300 / 2), 400, 300, NULL, NULL, wc.hInstance, NULL);

	if (!CreateDeviceD3D(hwnd))
	{
		CleanupDeviceD3D();
		::UnregisterClass(wc.lpszClassName, wc.hInstance);
		return;
	}

	::ShowWindow(hwnd, SW_SHOWDEFAULT);
	::UpdateWindow(hwnd);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui::StyleColorsDark();
	ImGuiStyle* style = &ImGui::GetStyle();
	ImVec4* colors = style->Colors;

	auto rgb = [](float r, float g, float b, float a) { return ImVec4(r / 255, g / 255, b / 255, a / 255); };

	colors[ImGuiCol_WindowBg] = ImVec4(rgb(34, 34, 34, 255));
	ImGui::GetStyle().WindowRounding = 0.0f;


	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX12_Init(g_pd3dDevice, NUM_FRAMES_IN_FLIGHT,
		DXGI_FORMAT_R8G8B8A8_UNORM, g_pd3dSrvDescHeap,
		g_pd3dSrvDescHeap->GetCPUDescriptorHandleForHeapStart(),
		g_pd3dSrvDescHeap->GetGPUDescriptorHandleForHeapStart());
	//io.Fonts->AddFontDefault();
	beyno_font_large = io.Fonts->AddFontFromFileTTF("fonts\\BEYNO.ttf", 55.0f);
	beyno_font_small = io.Fonts->AddFontFromFileTTF("fonts\\BEYNO.ttf", 60.0f);
	tit_font_large = io.Fonts->AddFontFromFileTTF("fonts\\TITILLUN.ttf", 32.5f);
	tit_font_small = io.Fonts->AddFontFromFileTTF("fonts\\TITILLUN.ttf", 22.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
	//ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
	//IM_ASSERT(font != NULL)

	SetWindowLongPtr(hwnd, GWL_STYLE, GetWindowLongPtr(hwnd, GWL_STYLE) & ~(WS_CAPTION | WS_THICKFRAME));
	begin = std::chrono::steady_clock::now();

	return;
}

bool CreateDeviceD3D(HWND hWnd)
{
	// Setup swap chain
	DXGI_SWAP_CHAIN_DESC1 sd;
	{
		ZeroMemory(&sd, sizeof(sd));
		sd.BufferCount = NUM_BACK_BUFFERS;
		sd.Width = 0;
		sd.Height = 0;
		sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.Flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		sd.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
		sd.Scaling = DXGI_SCALING_STRETCH;
		sd.Stereo = FALSE;
	}

	// [DEBUG] Enable debug interface
#ifdef DX12_ENABLE_DEBUG_LAYER
	ID3D12Debug* pdx12Debug = NULL;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&pdx12Debug))))
		pdx12Debug->EnableDebugLayer();
#endif

	// Create device
	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
	if (D3D12CreateDevice(NULL, featureLevel, IID_PPV_ARGS(&g_pd3dDevice)) != S_OK)
		return false;

	// [DEBUG] Setup debug interface to break on any warnings/errors
#ifdef DX12_ENABLE_DEBUG_LAYER
	if (pdx12Debug != NULL)
	{
		ID3D12InfoQueue* pInfoQueue = NULL;
		g_pd3dDevice->QueryInterface(IID_PPV_ARGS(&pInfoQueue));
		pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
		pInfoQueue->Release();
		pdx12Debug->Release();
	}
#endif

	{
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		desc.NumDescriptors = NUM_BACK_BUFFERS;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		desc.NodeMask = 1;
		if (g_pd3dDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&g_pd3dRtvDescHeap)) != S_OK)
			return false;

		SIZE_T rtvDescriptorSize = g_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = g_pd3dRtvDescHeap->GetCPUDescriptorHandleForHeapStart();
		for (UINT i = 0; i < NUM_BACK_BUFFERS; i++)
		{
			g_mainRenderTargetDescriptor[i] = rtvHandle;
			rtvHandle.ptr += rtvDescriptorSize;
		}
	}

	{
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		desc.NumDescriptors = 1;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		if (g_pd3dDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&g_pd3dSrvDescHeap)) != S_OK)
			return false;
	}

	{
		D3D12_COMMAND_QUEUE_DESC desc = {};
		desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		desc.NodeMask = 1;
		if (g_pd3dDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(&g_pd3dCommandQueue)) != S_OK)
			return false;
	}

	for (UINT i = 0; i < NUM_FRAMES_IN_FLIGHT; i++)
		if (g_pd3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&g_frameContext[i].CommandAllocator)) != S_OK)
			return false;

	if (g_pd3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, g_frameContext[0].CommandAllocator, NULL, IID_PPV_ARGS(&g_pd3dCommandList)) != S_OK ||
		g_pd3dCommandList->Close() != S_OK)
		return false;

	if (g_pd3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&g_fence)) != S_OK)
		return false;

	g_fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (g_fenceEvent == NULL)
		return false;

	{
		IDXGIFactory4* dxgiFactory = NULL;
		IDXGISwapChain1* swapChain1 = NULL;
		if (CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory)) != S_OK ||
			dxgiFactory->CreateSwapChainForHwnd(g_pd3dCommandQueue, hWnd, &sd, NULL, NULL, &swapChain1) != S_OK ||
			swapChain1->QueryInterface(IID_PPV_ARGS(&g_pSwapChain)) != S_OK)
			return false;
		swapChain1->Release();
		dxgiFactory->Release();
		g_pSwapChain->SetMaximumFrameLatency(NUM_BACK_BUFFERS);
		g_hSwapChainWaitableObject = g_pSwapChain->GetFrameLatencyWaitableObject();
	}

	CreateRenderTarget();
	return true;
}

void CleanupDeviceD3D()
{
	CleanupRenderTarget();
	if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = NULL; }
	if (g_hSwapChainWaitableObject != NULL) { CloseHandle(g_hSwapChainWaitableObject); }
	for (UINT i = 0; i < NUM_FRAMES_IN_FLIGHT; i++)
		if (g_frameContext[i].CommandAllocator) { g_frameContext[i].CommandAllocator->Release(); g_frameContext[i].CommandAllocator = NULL; }
	if (g_pd3dCommandQueue) { g_pd3dCommandQueue->Release(); g_pd3dCommandQueue = NULL; }
	if (g_pd3dCommandList) { g_pd3dCommandList->Release(); g_pd3dCommandList = NULL; }
	if (g_pd3dRtvDescHeap) { g_pd3dRtvDescHeap->Release(); g_pd3dRtvDescHeap = NULL; }
	if (g_pd3dSrvDescHeap) { g_pd3dSrvDescHeap->Release(); g_pd3dSrvDescHeap = NULL; }
	if (g_fence) { g_fence->Release(); g_fence = NULL; }
	if (g_fenceEvent) { CloseHandle(g_fenceEvent); g_fenceEvent = NULL; }
	if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }

#ifdef DX12_ENABLE_DEBUG_LAYER
	IDXGIDebug1* pDebug = NULL;
	if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&pDebug))))
	{
		pDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_SUMMARY);
		pDebug->Release();
	}
#endif
}

void CreateRenderTarget()
{
	for (UINT i = 0; i < NUM_BACK_BUFFERS; i++)
	{
		ID3D12Resource* pBackBuffer = NULL;
		g_pSwapChain->GetBuffer(i, IID_PPV_ARGS(&pBackBuffer));
		g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, g_mainRenderTargetDescriptor[i]);
		g_mainRenderTargetResource[i] = pBackBuffer;
	}
}

void CleanupRenderTarget()
{
	WaitForLastSubmittedFrame();

	for (UINT i = 0; i < NUM_BACK_BUFFERS; i++)
		if (g_mainRenderTargetResource[i]) { g_mainRenderTargetResource[i]->Release(); g_mainRenderTargetResource[i] = NULL; }
}

void WaitForLastSubmittedFrame()
{
	FrameContext* frameCtxt = &g_frameContext[g_frameIndex % NUM_FRAMES_IN_FLIGHT];

	UINT64 fenceValue = frameCtxt->FenceValue;
	if (fenceValue == 0)
		return; // No fence was signaled

	frameCtxt->FenceValue = 0;
	if (g_fence->GetCompletedValue() >= fenceValue)
		return;

	g_fence->SetEventOnCompletion(fenceValue, g_fenceEvent);
	WaitForSingleObject(g_fenceEvent, INFINITE);
}

FrameContext* WaitForNextFrameResources()
{
	UINT nextFrameIndex = g_frameIndex + 1;
	g_frameIndex = nextFrameIndex;

	HANDLE waitableObjects[] = { g_hSwapChainWaitableObject, NULL };
	DWORD numWaitableObjects = 1;

	FrameContext* frameCtxt = &g_frameContext[nextFrameIndex % NUM_FRAMES_IN_FLIGHT];
	UINT64 fenceValue = frameCtxt->FenceValue;
	if (fenceValue != 0) // means no fence was signaled
	{
		frameCtxt->FenceValue = 0;
		g_fence->SetEventOnCompletion(fenceValue, g_fenceEvent);
		waitableObjects[1] = g_fenceEvent;
		numWaitableObjects = 2;
	}

	WaitForMultipleObjects(numWaitableObjects, waitableObjects, TRUE, INFINITE);

	return frameCtxt;
}

void ResizeSwapChain(HWND hWnd, int width, int height)
{
	DXGI_SWAP_CHAIN_DESC1 sd;
	g_pSwapChain->GetDesc1(&sd);
	sd.Width = width;
	sd.Height = height;

	IDXGIFactory4* dxgiFactory = NULL;
	g_pSwapChain->GetParent(IID_PPV_ARGS(&dxgiFactory));

	g_pSwapChain->Release();
	CloseHandle(g_hSwapChainWaitableObject);

	IDXGISwapChain1* swapChain1 = NULL;
	dxgiFactory->CreateSwapChainForHwnd(g_pd3dCommandQueue, hWnd, &sd, NULL, NULL, &swapChain1);
	swapChain1->QueryInterface(IID_PPV_ARGS(&g_pSwapChain));
	swapChain1->Release();
	dxgiFactory->Release();

	g_pSwapChain->SetMaximumFrameLatency(NUM_BACK_BUFFERS);

	g_hSwapChainWaitableObject = g_pSwapChain->GetFrameLatencyWaitableObject();
	assert(g_hSwapChainWaitableObject != NULL);
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_SIZE:
		if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
		{
			WaitForLastSubmittedFrame();
			ImGui_ImplDX12_InvalidateDeviceObjects();
			CleanupRenderTarget();
			ResizeSwapChain(hWnd, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam));
			CreateRenderTarget();
			ImGui_ImplDX12_CreateDeviceObjects();
		}
		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
		break;
	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;
	}
	return ::DefWindowProc(hWnd, msg, wParam, lParam);
}