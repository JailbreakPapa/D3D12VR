
#include "BlackSpaceDirectX.h"

auto  CD3DX12_CPU_DESCRIPTOR_HANDLE::operator==(const ComPtr<CD3DX12_CPU_DESCRIPTOR_HANDLE>& com)
{
	ptr = com->ptr;
	return *this;
}
