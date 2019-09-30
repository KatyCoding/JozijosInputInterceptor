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


bool IsAnotherLargerComboBeingTriggered(KeyCombo smallerCombo);
void TestKeyCombos();
void CheckInputsOnAdd();
void CheckInputsOnRemove();
void MapKeyOutputs();
void ReadKeyCombos();
double start;
DEVMODE* dev;
KeyCombo* keyCombos;
int* sentInputs;
int numOfKeyCombos;
int sizeOfPressed = 0;
int* pressed;
bool recievedInputRecently = false;

INPUT* inputsToSend;


int main()
{
	sentInputs = new int[1]{ NULL };
	//SetupKeyCombos();
	//HWND window;
	ReadKeyCombos();
	//AllocConsole();
	//window = FindWindowA("ConsoleWindowClass", NULL);
	//ShowWindow(window, 0);
	MessageLoop();
	return 0;
}


LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode == HC_ACTION)
	{


		PKBDLLHOOKSTRUCT key = (PKBDLLHOOKSTRUCT)lParam;
		char keyChar = (char)key->vkCode;
		//std::cout << keyChar << '\n';
		//if (!SearchForElement(pressed, key->vkCode, sizeOfPressed))
		//{
		//	bool log = true;
		//	for (int i = 0; i < numOfKeyCombos; i++) {
		//		if (key->vkCode == keyCombos[i].keyOutput)
		//			log = false;
		//		//std::cout << "\nSpecial key: " << (char)key->vkCode;
		//	}
		//	if (log)
		//		std::cout << "\ncode not in pressed: " <<keyChar;
		//}
		//for (int i = 0; i < numOfKeyCombos; i++) {
		//	//if (key->vkCode == keyCombos[i].keyOutput)
		//		//std::cout << "\nSpecial key: " << (char)key->vkCode;
		//}

		//std::cout << "\nkey input action... key was: " << keyChar << "\n";

		if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)
		{
			for (int i = 0; i < numOfKeyCombos; i++)
			{
				if (keyCombos[i].keyOutput == key->scanCode)
				{
					return CallNextHookEx(NULL, nCode, wParam, lParam);

				}
			}

			if (SearchForElement(pressed, key->vkCode, sizeOfPressed))
			{
				CheckInputsOnAdd();
				return CallNextHookEx(NULL, nCode, wParam, lParam);
			}
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
				delete[] pressed;
				pressed = new int[sizeOfPressed];
				memcpy((void*)pressed, temp, sizeof(int) * (sizeOfPressed - 1));
				pressed[sizeOfPressed - 1] = key->vkCode;
				CheckInputsOnAdd();
				delete[]temp;
				temp = nullptr;

			}

#pragma endregion



		}
		if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP)
		{
			//std::cout << "Key Up event for key: " << (char)key->vkCode << "\n";
			if (!SearchForElement(pressed, key->vkCode, sizeOfPressed))
			{
				//CheckInputsOnRemove();
				return CallNextHookEx(NULL, nCode, wParam, lParam);
			}
#pragma region ResizePressedArray

			sizeOfPressed--;
			if (sizeOfPressed == 0)
			{
				delete[] pressed;
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
				delete[] pressed;
				pressed = new int[sizeOfPressed];
				memcpy((void*)pressed, temp, sizeof(int) * (sizeOfPressed + 1));
				if (temp != nullptr)
					delete[] temp;

			}


#pragma endregion


			CheckInputsOnRemove();
		}



	}



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


	while (GetMessage(&msg, NULL, 0, 0) > 0)
	{
		//CheckInputsOnAdd();
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




	FILE* combos;
	fopen_s(&combos, "./bin/combos.txt", "rt");
	if (combos == nullptr)
		perror("FUCK OFF");
	fseek(combos, 0, SEEK_END);
	size_t size = ftell(combos);
	fseek(combos, 0, SEEK_SET);

	char* contents = (char*)malloc(sizeof(char) * size);//new char[size + 1];
	fread(contents, sizeof(char), (size + 1), combos);
	contents[size] = '\0';
	std::cout << contents << "\n";
	int numberOfCombosToRead = 1;
	for (int i = 0; i < size + 1; i++)
	{
		if (contents[i] == ',')
			numberOfCombosToRead++;

	}
	numOfKeyCombos = numberOfCombosToRead;
	delete[] keyCombos;
	keyCombos = new KeyCombo[numberOfCombosToRead];
	std::string inputs = "";
	bool readingOut = false;
	int index = 0;
	for (int i = 0; i < size + 1; i++)
	{
		if (index >= numberOfCombosToRead)
			break;
		if (contents[i] == ' ' || contents[i] == '"')
			continue;
		if (contents[i] == ',')
		{
			index++;
			continue;
		}
		if (contents[i] == ':')
		{
			readingOut = true;
			int strSize = inputs.length();
			const char* chars = inputs.c_str();
			keyCombos[index].requiredKeyPresses = new int[strSize + 1];
			for (int j = 0; j < strSize; j++)
			{
				keyCombos[index].requiredKeyPresses[j] = (int)chars[j];
			}
			inputs = "";
			keyCombos[index].requiredKeyPresses[strSize] = NULL;
			continue;
		}

		if (!readingOut)
		{

			inputs += contents[i];

			continue;
		}
		keyCombos[index].keyOutput = (int)contents[i];
		readingOut = false;
	}

	MapKeyOutputs();

}


void CheckInputsOnRemove()
{
	int numberOfInputsLastFrame = 0;
	int* lastFramesInputs = sentInputs;

	while (*lastFramesInputs != NULL)
	{

		numberOfInputsLastFrame++;
		lastFramesInputs++;
	}
	//std::cout << numberOfInputsLastFrame << '\n';

	for (int i = 0; i < numOfKeyCombos; i++)
	{
		if (SearchForElement(sentInputs, keyCombos[i].keyOutput, numberOfInputsLastFrame))
		{
			int* iter = keyCombos[i].requiredKeyPresses;

			while (*iter != NULL)
			{
				if (!SearchForElement(pressed, *iter, sizeOfPressed))
				{
					//std::cout << "\n breaking because character: " << (char)* iter << " was not found.";
					break;

				}
				iter++;
			}
			if (*iter == NULL)
				continue;
			INPUT input = INPUT();
			input.type = INPUT_KEYBOARD;
			input.ki.wVk = 0;
			input.ki.wScan = keyCombos[i].keyOutput;
			input.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;


			if (numberOfInputsLastFrame == 1)
			{
				delete[] sentInputs;
				sentInputs = new int[1];
				sentInputs[0] = NULL;
			}
			else
			{
				//std::cout<<"Removing"

				int* temp = new int[numberOfInputsLastFrame];
				int j = 0;
				for (int k = 0; k < numberOfInputsLastFrame + 1; k++)
				{
					if (sentInputs[k] == keyCombos[i].keyOutput)
						continue;
					temp[j] = sentInputs[k];
					j++;
				}

				delete[] sentInputs;
				sentInputs = new int[numberOfInputsLastFrame];
				memcpy((void*)sentInputs, temp, sizeof(int) * (numberOfInputsLastFrame - 1));
				sentInputs[numberOfInputsLastFrame - 1] = NULL;
				delete[] temp;
			}
			SendInput(1, &input, sizeof(INPUT));

		}
	}

}


void CheckInputsOnAdd()
{

	//std::cout << "Number of keys pressed: " << sizeOfPressed << "\n";
	//std::cout <<"Number of sent inputs last frame: " << numberOfInputsLastFrame << "\n";
	for (int i = 0; i < numOfKeyCombos; i++)
	{
		int* iter = keyCombos[i].requiredKeyPresses;
		//std::cout << "\nTrying to press key: " << (char)keyCombos[i].keyOutput;
		while (*iter != NULL)
		{
			if (!SearchForElement(pressed, *iter, sizeOfPressed))
			{
				//std::cout << "\n breaking because character: " << (char)* iter << " was not found.";
				break;

			}
			iter++;
		}
		if (*iter == NULL)
		{
			//if(!IsAnotherLargerComboBeingTriggered(keyCombos[i])) do this:
			INPUT input = INPUT();
			input.type = INPUT_KEYBOARD;
			input.ki.wVk = 0;
			input.ki.wScan = keyCombos[i].keyOutput;
			input.ki.dwFlags = KEYEVENTF_SCANCODE;
			//std::cout << "\nSending Input: " << (char)input.ki.wVk;
			int numberOfInputsLastFrame = 0;
			int* lastFramesInputs = sentInputs;

			while (*lastFramesInputs != NULL)
			{

				numberOfInputsLastFrame++;
				lastFramesInputs++;
			}
			if (!SearchForElement(sentInputs, input.ki.wScan, numberOfInputsLastFrame))
			{
				//std::cout << "Adding key: " << input.ki.wScan << " to sent inputs" << "\n";
				int* temp = new int[numberOfInputsLastFrame + 1];
				memcpy((void*)temp, sentInputs, (sizeof(int) * (numberOfInputsLastFrame + 1)));
				delete[] sentInputs;
				sentInputs = new int[numberOfInputsLastFrame + 2];
				memcpy((void*)sentInputs, temp, sizeof(int) * (numberOfInputsLastFrame + 1));
				delete[] temp;
				sentInputs[numberOfInputsLastFrame] = keyCombos[i].keyOutput;
				sentInputs[numberOfInputsLastFrame + 1] = NULL;
			}
			//else
			//	std::cout << "Skipping bc found already\n";
			SendInput(1, &input, sizeof(INPUT));

		}

	}







}

bool IsAnotherLargerComboBeingTriggered(KeyCombo smallerCombo)
{
	//check through other combos to see if any of them contain the smaller combo:
	//ex) if green+red = "F" but green+red+yellow = "E" we want the larger combo "E" to trigger instead of "F"
	//we come here when "F" is triggered then we go through all the combos containing green+red and if any of them are being pressed we stop "F" from going
	//by returning true here.

	return false;
}



void MapKeyOutputs()
{
	HKL keyboard = GetKeyboardLayout(0);
	for (int i = 0; i < numOfKeyCombos; i++)
	{
		keyCombos[i].keyOutput = MapVirtualKeyExA(keyCombos[i].keyOutput, MAPVK_VK_TO_VSC, keyboard);
	}
	//UINT asc = MapVirtualKeyExA((int)'A', MAPVK_VK_TO_VSC, keyboard);
	//UINT avc = MapVirtualKeyExA(asc, MAPVK_VSC_TO_VK, keyboard);
	//char achar = MapVirtualKeyExA(avc, MAPVK_VK_TO_CHAR, keyboard);
}