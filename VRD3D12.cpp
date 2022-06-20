#include "VRD3D12.h"
static UINT BBIndex{ 3};
void VRD3D12::CreateContexts(HWND win)
{
	ThrowIfFailed(CreateDXGIFactory2(0, IID_PPV_ARGS(&Factory)));
	ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&Debug)));
	Debug->EnableDebugLayer();
	ThrowIfFailed(D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&Device)));
    D3D12_COMMAND_QUEUE_DESC cmd_queue_desc = {};
    cmd_queue_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    cmd_queue_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	ThrowIfFailed(Device->CreateCommandQueue(&cmd_queue_desc, IID_PPV_ARGS(&Queue)));
    FindAdaptors();
    
    DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
    swapChainDesc.BufferDesc.Width = Width;
    swapChainDesc.BufferDesc.Height = Height;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.OutputWindow = win;
    swapChainDesc.BufferCount = Buffers;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.Windowed = Windowed;
    swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    ComPtr<IDXGISwapChain> swapChain;
    ThrowIfFailed(Factory->CreateSwapChain(
        Queue.Get(),   
        &swapChainDesc,
        &swapChain
    ));
    ThrowIfFailed(swapChain.As(&Swap));
    BBIndex = Swap->GetCurrentBackBufferIndex();
    
    D3D12_DESCRIPTOR_HEAP_DESC d = {};
    d.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    d.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    d.NumDescriptors = Buffers;
    ThrowIfFailed(Device->CreateDescriptorHeap(&d, IID_PPV_ARGS(&RTVH)));
    RTVSize = Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtv_handle(RTVH->GetCPUDescriptorHandleForHeapStart());
    for(UINT a = 0; a < Buffers; a++)
    {
        ThrowIfFailed(Swap->GetBuffer(a, IID_PPV_ARGS(&RT[a])));
        Device->CreateRenderTargetView(RT[a].Get(), nullptr, rtv_handle);
        rtv_handle.Offset(1, RTVSize);
    }
    BBIndex = Swap->GetCurrentBackBufferIndex();
   for(static int l = 0; l < Buffers; l++)
   {
	   
       ThrowIfFailed(Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&Alloc[l])));
   }
   ThrowIfFailed(Device->CreateCommandList(NULL, D3D12_COMMAND_LIST_TYPE_DIRECT,Alloc[BBIndex].Get(), NULL, IID_PPV_ARGS(&List)));
   ThrowIfFailed(List->Close());
   //Alloc[Buffers]->Reset();
   SetFenceEvents();
}

   
void VRD3D12::InitBaseAssets()
{
    //Create RS.
    D3D12_STATIC_SAMPLER_DESC s[1];
    s[0].Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
    s[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
    s[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
    s[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
    s[0].MaxLOD = D3D12_FLOAT32_MAX;
    s[0].MinLOD = 0.0f;
    s[0].MipLODBias = 0;
    s[0].ShaderRegister = 0;
    s[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
    s[0].MaxAnisotropy = 0;
    s[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
    s[0].BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
    s[0].RegisterSpace = 0;
    CD3DX12_ROOT_SIGNATURE_DESC rsd(0, nullptr, 1, s, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
    COM<ID3DBlob> Sign;
    COM<ID3DBlob> Error{ nullptr };
    ThrowIfFailed(D3D12SerializeRootSignature(&rsd, D3D_ROOT_SIGNATURE_VERSION_1, &Sign, nullptr));
    ThrowIfFailed(Device->CreateRootSignature(NULL, Sign->GetBufferPointer(), Sign->GetBufferSize(), IID_PPV_ARGS(&RSO)));
    RSO->SetName(L"MainRSO");
   // CreateShader()

}

void VRD3D12::SetFenceEvents()
{
    auto num_back_buffers = BBIndex;
    // create the fence
    for (auto p{ 0 }; p < Buffers; p++) {
        ThrowIfFailed(Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&Fence[p])));
        FenceValue[p] = 0; // set the initial fence value to 0
    }
    // create a handle to a fence event
    FenceEvent= CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (FenceEvent == nullptr) {
        throw "Failed to create fence event.";
    }
    
}

void VRD3D12::FindAdaptors()
{
    HRESULT hr;
    COM<IDXGIAdapter1> ta;
   int index = 0;
    while(Factory->EnumAdapters1(index,ta.GetAddressOf())!= DXGI_ERROR_NOT_FOUND)
    {
        DXGI_ADAPTER_DESC1 desc;
       ta->GetDesc1(&desc);

        // Skip software adapters.
        if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
            index++;
            continue;
        }
        hr = D3D12CreateDevice(ta.Get(), D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&Device));
        if (SUCCEEDED(hr))
            break;

        index++;
    }

    if (ta == nullptr) {
        throw "No comaptible adapter found.";
    }
    
}

void VRD3D12::Render()
{
    Alloc[BBIndex]->Reset();
    auto index = Swap->GetCurrentBackBufferIndex();
	static float clear_color[4] = { 0.568f, 0.733f, 1.0f, 1.0f };
    BBIndex = Swap->GetCurrentBackBufferIndex();
    ThrowIfFailed(List->Reset(Alloc[BBIndex].Get(), nullptr));
    //DrawCommands Here
    // Transition to RENDER_TARGET
    CD3DX12_RESOURCE_BARRIER begin_transition = CD3DX12_RESOURCE_BARRIER::Transition(
        RT[BBIndex].Get(),
        D3D12_RESOURCE_STATE_PRESENT,
        D3D12_RESOURCE_STATE_RENDER_TARGET
    );
    List->ResourceBarrier(1, &begin_transition);
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtv_handle(RTVH->GetCPUDescriptorHandleForHeapStart(), BBIndex, RTVSize);
    List->OMSetRenderTargets(1, &rtv_handle, false, nullptr);
    List->ClearRenderTargetView(rtv_handle, clear_color, NULL, nullptr);
   // InitBaseAssets();
    CD3DX12_RESOURCE_BARRIER end_transition = CD3DX12_RESOURCE_BARRIER::Transition(
        RT[BBIndex].Get(),
        D3D12_RESOURCE_STATE_RENDER_TARGET,
        D3D12_RESOURCE_STATE_PRESENT
    );
    List->ResourceBarrier(1, &end_transition);
    ThrowIfFailed(List->Close());
    ID3D12CommandList** cmd_lists = new ID3D12CommandList * [1];
    cmd_lists[0] = List.Get();
    Queue->ExecuteCommandLists(1, cmd_lists);
    // GPU Signal
    ThrowIfFailed(Queue->Signal(Fence[BBIndex].Get(), FenceValue[BBIndex]));
    ThrowIfFailed(Swap->Present(1, 0));
    WaitForPrevFrame();
    // Update our frame index
    BBIndex= Swap->GetCurrentBackBufferIndex();
    index = Swap->GetCurrentBackBufferIndex();
}

bool VRD3D12::CreateShader(LPCWSTR ShaderName, std::string Main, ShaderT& Type)
{
    COM<ID3DBlob> ShaderB;
    COM<ID3DBlob> Error;
    LPCSTR type{ "" };
   
	    if (Type == ShaderT::ST_PS)
	    {
		    type = "ps_5_0";
		    return type;
	    }
        else if (Type == ShaderT::ST_VS)
        {
            type = "vs_5_0";
            return type;
        }
    ThrowIfFailed(D3DCompileFromFile(ShaderName, nullptr, nullptr, (LPCSTR)Main.c_str(), (LPCSTR)type, D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
        0,
        ShaderB.GetAddressOf(),
        Error.GetAddressOf()));
    return true;
}

void VRD3D12::WaitForPrevFrame()
{
    if (Fence[BBIndex]->GetCompletedValue() < FenceValue[BBIndex]) {
        // we have the fence create an event which is signaled once the fence's current value is "fence_value"
        HRESULT hr = Fence[BBIndex]->SetEventOnCompletion(FenceValue[BBIndex], FenceEvent);
        if (FAILED(hr)) {
            throw "Failed to set fence event.";
        }
        WaitForSingleObject(FenceEvent, INFINITE);
    }

    // increment fenceValue for next frame
    FenceValue[BBIndex]++;
}

