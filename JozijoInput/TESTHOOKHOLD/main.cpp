#include <Windows.h>
#include <iostream>

LRESULT CALLBACK LowLevelKeyboardProc(int, WPARAM, LPARAM);
void MessageLoop();

int main()
{
	MessageLoop();
	return 0;
}

void MessageLoop()
{
	HHOOK hook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, NULL, NULL);

	MSG msg;

	while (!GetMessage(&msg, NULL, NULL, NULL))
	{

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{

	if (nCode == HC_ACTION)
	{
		PKBDLLHOOKSTRUCT key = (PKBDLLHOOKSTRUCT)lParam;
		char keyChar = (char)key->vkCode;
	
		if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)
		{
			std::cout << "Down: " << keyChar << '\n';

		}
		if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP)
		{
			std::cout << "Up: " << keyChar << '\n';

		}
	}
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}