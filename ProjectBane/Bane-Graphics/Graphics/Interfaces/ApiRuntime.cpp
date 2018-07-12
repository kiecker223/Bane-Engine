#include "ApiRuntime.h"
#include "../D3D12/D3D12Runtime.h"


ApiRuntime* ApiRuntime::Runtime = nullptr;

void ApiRuntime::CreateRuntime()
{
	ApiRuntime::SetApiRuntime(new D3D12Runtime());
}
