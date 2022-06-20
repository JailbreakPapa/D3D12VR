#pragma once
#include <Windows.h>
#include <d3d12.h>
#include <wrl.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include "BlackSpaceDirectX.h"
#include "BSTime.h"
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib,"D3d12.lib")
class VRD3D12 {
public:
	template<typename A>
	using COM = Microsoft::WRL::ComPtr<A>;

    const static UINT Buffers = 3;
    RECT WindowRect{};

	COM<ID3D12Device8> Device;
    COM<ID3D12CommandQueue> Queue;
    COM<ID3D12CommandAllocator> Alloc[Buffers];
    COM<IDXGIFactory5> Factory;
    COM<IDXGISwapChain3> Swap;
    COM<ID3D12Resource> BBuffer[Buffers];
    COM<ID3D12GraphicsCommandList> List;
    COM<ID3D12DescriptorHeap> RTVH;
    COM<ID3D12PipelineState> PSO;
    COM<ID3D12Debug> Debug;
    COM<ID3D12Resource> RT[Buffers];
    COM<ID3D12RootSignature> RSO;
    COM<ID3D12Resource> VertexBuffer;

    UINT RTVSize{};
    D3D12_BLEND_DESC blend_desc = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    D3D12_DEPTH_STENCIL_DESC depth_stencil_state = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    D3D12_RASTERIZER_DESC rasterize_desc = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    DXGI_SAMPLE_DESC sampleDesc = { 1, 0 };

    std::vector<D3D12_INPUT_ELEMENT_DESC> inputvector{
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
    };

    D3D12_VIEWPORT vp;
    D3D12_RECT sr;
    ID3D12CommandList* const commandLists[1] = {List.Get()};
    //FenceObjects
	COM<ID3D12Fence> Fence;
	UINT FenceValue = 0;
    UINT64 FrameFenceValues[Buffers] = {};
    HANDLE FenceEvent;
    GameTimer g;
    //Window Objects 
    bool Windowed = true;
    int Width;
    int Height;
    bool g_VSync;
    bool g_TearingSupported;
    //Functions
    void CreateContexts(HWND win);
    void InitBaseAssets();
    void SetFenceEvents();
    void FindAdaptors();
    void WaitForPrevFrame();
    void Render();
protected:
    enum class ShaderT
    {
	    ST_VS = 0x1,
        ST_PS = 0x1,
        //Add other shader types if needed.
    };
    bool CreateShader(LPCWSTR ShaderName, std::string Main,ShaderT& Type);
};