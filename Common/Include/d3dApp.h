#pragma once
#include "d3dUtil.h"
#include "GameTimer.h"

class D3DApp
{
public:
	static const UINT SwapChainBufferCount = 2;
	static D3DApp* GetApp();
	virtual LRESULT MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	D3DApp(HINSTANCE hInstance);
	~D3DApp();
	virtual void Init();
	void Run();
	
protected:
	void CalculateFrameState();
	virtual void Update();
	virtual void Draw();
	virtual void OnResize();
	virtual void CreateRtvAndDsvDescriptorHeaps();

	virtual void OnMouseDown(WPARAM key, int x, int y);
	virtual void OnMouseUp(WPARAM key, int x, int y);
	virtual void OnMouseMove(WPARAM key, int x, int y);

private:
	static D3DApp* mApp;
	HWND mhMainWnd;
	HINSTANCE mhAppInst;
	GameTimer mTimer;
	bool mAppPaused;
	bool mResizing;
	bool mMinimized;
	bool mMaximized;

protected:
	void LogAdapters();
	void LogAdapterOutputs(IDXGIAdapter* adapter);
	void LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format);
	void InitializeDirect3D();
	void InitializeWindow();
	void CreateCommandObjects();
	void CreateSwapChain();
	void CreateRenderTargetViews();
	void CreateDepthStencilView();
	void SetViewport();
	void Set4xMsaaState(bool state);
	void FlushCommandQueue();
	
	D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView() const;
	D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView() const;
	ID3D12Resource* CurrentBackBuffer() const
	{ return mSwapChainBuffer[mCurrBackBuffer].Get(); }

	Microsoft::WRL::ComPtr<IDXGIFactory4> mdxgiFactory;
	Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice;
	Microsoft::WRL::ComPtr<ID3D12Fence> mFence;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> mCommandQueue;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> mDirectCmdListAlloc;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList;
	Microsoft::WRL::ComPtr<IDXGISwapChain> mdxgiSwapChain;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mRTVHeap;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mDSVHeap;
	Microsoft::WRL::ComPtr<ID3D12Resource> mSwapChainBuffer[SwapChainBufferCount];
	Microsoft::WRL::ComPtr<ID3D12Resource> mDepthStencilBuffer;

	UINT m4xMsaaQuality;
	bool m4xMsaaState;
	UINT mCurrBackBuffer = 0;
	UINT mRTVDescriptorSize;
	UINT mDSVDescriptorSize;
	UINT64 mCurrentFence = 0;
	D3D12_VIEWPORT mScreenViewport;
	D3D12_RECT mScissorRect;

	DXGI_FORMAT mBackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGI_FORMAT mDepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	D3D_DRIVER_TYPE md3dDriverType = D3D_DRIVER_TYPE_HARDWARE;
	UINT mClientWidth = 1920;
	UINT mClientHeight = 1080;
	std::wstring mMainWndCaption = L"D3D App";
};
