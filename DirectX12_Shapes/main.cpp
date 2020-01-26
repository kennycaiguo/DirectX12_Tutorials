#include "ShapesApp.h"


int WINAPI wWinMain(
	HINSTANCE hInstance, HINSTANCE prevInstance,
	PWSTR cmdLine, int showCmd)
{
#if defined(DEBUG) || defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	try
	{
		ShapesApp app(hInstance);
		app.Init();
		app.Run();
	}
	catch (const DxException & e)
	{
		MessageBox(nullptr, e.ToString().c_str(), L"HR Failed", MB_OK);
		return 0;
	}

	return 0;
}