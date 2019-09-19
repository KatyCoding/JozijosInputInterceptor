#include <Windows.h>
#include <strsafe.h>
#include <iostream>
#include <stdio.h>
#include <ctime>
#include "KeyCombos.h"



LRESULT CALLBACK LowLevelKeyboardProc(int, WPARAM, LPARAM);
void ListenForKeyCode();
void MessageLoop();
bool SearchForElement(int*, int, int);
int GetIndexOfFirstOccurance(int*, int, int);

void TestKeyCombos();
void CheckInputs();
double start;
DEVMODE* dev;
KeyCombo* keyCombos;
int* sentInputs;
int numOfKeyCombos;
unsigned int sizeOfPressed = 0;
int* pressed;
bool recievedInputRecently = false;

//to be removed when I read from file.
void SetupKeyCombos();

int main()
{
	sentInputs = new int[1]{ NULL };
	SetupKeyCombos();
	HWND window;
	AllocConsole();
	//window = FindWindowA("ConsoleWindowClass", NULL);
	//ShowWindow(window, 0);
	dev = new DEVMODE();
	MessageLoop();
	return 0;
}


LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{

	if (nCode == HC_ACTION)
	{
		PKBDLLHOOKSTRUCT key = (PKBDLLHOOKSTRUCT)lParam;

		if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)
		{

			//TestKeyCombos();
			if (SearchForElement(pressed, key->vkCode, sizeOfPressed))
				return CallNextHookEx(NULL, nCode, wParam, lParam);
#pragma region ResizePressedArray

			sizeOfPressed++;
			if (sizeOfPressed == 1)
			{
				pressed = new int[sizeOfPressed];
				pressed[0] = key->vkCode;

			}
			else
			{
				int* temp = new int[sizeOfPressed];
				memcpy((void*)temp, pressed, sizeof(int) * (sizeOfPressed - 1));
				pressed = new int[sizeOfPressed];
				memcpy((void*)pressed, temp, sizeof(int) * (sizeOfPressed - 1));
				pressed[sizeOfPressed - 1] = key->vkCode;
				delete[]temp;

			}
			for (int i = 0; i < sizeOfPressed; i++)
			{
				std::cout << (char)pressed[i]<<" " ;
			}
			std::cout << "\n";

#pragma endregion

			//asycKey doesnt work in hook
			//if (GetAsyncKeyState(VK_SHIFT) && GetAsyncKeyState(VK_CONTROL) && GetAsyncKeyState(VK_MENU) && key->vkCode == 0x46)
			//if (key->vkCode == 0x46 || key->vkCode == VK_LCONTROL || key->vkCode == VK_LMENU || key->vkCode == VK_LSHIFT)
			//{
			//	//ShellExecute(nullptr, "open", "C:\\Program Files (x86)\\Windows Media Player\\wmplayer.exe", nullptr, nullptr, SW_SHOW);
			//
			//
			//	if (!SearchForElement(&pressed[0], key->vkCode, 4))
			//	{
			//
			//		for (int i = 0; i < 4; i++)
			//		{
			//			if (pressed[i] == '\0')
			//			{
			//				pressed[i] = key->vkCode;
			//				ListenForKeyCode();
			//				break;
			//			}
			//		}
			//
			//	}
			//}
		}
		if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP)
		{



#pragma region ResizePressedArray

			sizeOfPressed--;
			if (sizeOfPressed == 0)
			{
				delete pressed;
			}
			else {
				int* temp = new int[sizeOfPressed];
				int j = 0;
				for (int i = 0; i < sizeOfPressed + 1; i++)
				{
					if (pressed[i] == key->vkCode)
						continue;
					temp[j] = pressed[i];
					j++;
				}

				pressed = new int[sizeOfPressed];
				memcpy((void*)pressed, temp, sizeof(int) * (sizeOfPressed));
				delete[] temp;

			}
			for (int i = 0; i < sizeOfPressed; i++)
			{
				std::cout << (char)pressed[i]<< " ";
			}
			std::cout << "\n";

#pragma endregion

			//if (key->vkCode == 0x46 || key->vkCode == VK_LCONTROL || key->vkCode == VK_LMENU || key->vkCode == VK_LSHIFT)
			//{
			//	if (SearchForElement(&pressed[0], key->vkCode, 4))
			//	{
			//		for (int i = 0; i < 4; i++)
			//		{
			//			if (pressed[i] == key->vkCode)
			//			{
			//
			//				pressed[i] = '\0';
			//				break;
			//			}
			//		}
			//	}
			//}
		}
	}
	CheckInputs();
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

bool SearchForElement(int* arrayPointer, int searchFor, int length)
{

	for (int i = 0; i < length; i++)
	{

		if (*arrayPointer == searchFor)
			return true;

		arrayPointer++;
	}
	return false;
}

int GetIndexOfFirstOccurance(int* arrayPointer, int searchFor, int length)
{
	for (int i = 0; i < length; i++)
	{
		if (*arrayPointer == searchFor)
			return i;
		arrayPointer++;
	}
	return -1;
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



double timeOfLastCall = -1;
void ListenForKeyCode()
{
	double timePassed;
	if (timeOfLastCall == -1)
		timePassed = 10;
	else
	{
		timePassed = (clock() - timeOfLastCall) / CLOCKS_PER_SEC;
	}

	if (timePassed > 2)
	{
		if (SearchForElement(pressed, 0x46, 4) && SearchForElement(pressed, VK_LCONTROL, 4) && SearchForElement(pressed, VK_LMENU, 4) && SearchForElement(pressed, VK_LSHIFT, 4))
		{
			timeOfLastCall = clock();

		}
	}




}


//TEMP 
void TestKeyCombos()
{
	KeyCombo* testCombos = new KeyCombo[2];
	testCombos[0].keyOutput = (int)'w';
	testCombos[0].requiredKeyPresses = new int[3]{ 'p', 'o', NULL };
	testCombos[1].keyOutput = (int)'a';
	testCombos[1].requiredKeyPresses = new int[3]{ 'u', 'i', NULL };
	for (int i = 0; i < 2; i++)
	{
		std::string log = "Combo to press: ";
		int* iterator = testCombos[i].requiredKeyPresses;
		while (*iterator != NULL)
		{
			log += (char)* iterator;
			iterator++;
		}
		std::cout << log << std::endl;
	}
	delete testCombos;
}

void ReadKeyCombos()
{
	char* cptr;
	FILE* combos;
	fopen_s(&combos, "$(SolutionDir)\bin\combos.txt", "ifstream");
	int size = 10;
	keyCombos = new KeyCombo[size * sizeof(KeyCombo)];
	for (int i = 0; i < size; i++)
	{
		(*keyCombos).requiredKeyPresses = nullptr;
	}
	int* ints = new int[2];
	ints[0] = 2;


}

//whats currently happening is that the inputs from last frame are not being emptied so when the key up event is called the "number of presses" goes
//to 0 but then next fram the key up event is called again even though it shouldnt be called until the key combo is actually unpressed.
//doing it only when the combo is unpressed should cause it to go back to normal and actually possible just straight up work.

void CheckInputs()
{
	int numberOfInputsLastFrame = 0;
	int* lastFramesInputs = sentInputs;
	while (*lastFramesInputs != NULL)
	{
		INPUT input = INPUT();
		input.type = INPUT_KEYBOARD;
		input.ki.wVk = *lastFramesInputs;
		input.ki.dwFlags = KEYEVENTF_KEYUP;
		SendInput(1, &input, sizeof(INPUT));
		numberOfInputsLastFrame++;
		lastFramesInputs++;
	}
	
	for (int i = 0; i < numOfKeyCombos; i++)
	{
		int* iter = keyCombos[i].requiredKeyPresses;
		while (*iter != NULL)
		{
			if (!SearchForElement(pressed, *iter, sizeOfPressed))
				break;
			iter++;
		}
		if (*iter == NULL)
		{
			INPUT input = INPUT();
			input.type = INPUT_KEYBOARD;
			input.ki.wVk = keyCombos[i].keyOutput;
			SendInput(1, &input, sizeof(INPUT));
			int* temp = new int[numberOfInputsLastFrame + 1];
			memcpy((void*)temp, sentInputs, sizeof(int) * (numberOfInputsLastFrame + 1));
			sentInputs = new int[numberOfInputsLastFrame + 2];
			memcpy((void*)sentInputs, temp, sizeof(int) * (numberOfInputsLastFrame + 1));
			sentInputs[numberOfInputsLastFrame] = keyCombos[i].keyOutput;
			sentInputs[numberOfInputsLastFrame + 1] = NULL;
		}
	}







}





void SetupKeyCombos()
{
	numOfKeyCombos = 2;
	keyCombos = new KeyCombo[2];
	keyCombos[0].keyOutput = (int)'w';
	keyCombos[0].requiredKeyPresses = new int[3]{ 'P', 'O', NULL };
	keyCombos[1].keyOutput = (int)'a';
	keyCombos[1].requiredKeyPresses = new int[3]{ 'U', 'I', NULL };
}