#ifndef UNICODE
#define UNICODE
#endif 

#include <iostream>
#include <vector>
#include <windows.h>
#include <map>

using namespace std;

#define WM_INPUT 0x00FF
#define eSuccess true
#define eFailure false


//List of virtual keys: https://help.mjtnet.com/article/262-virtual-key-codes
map<USHORT, string> MapOfValidInputKeys 
{

	{87,"Keyboard W"},
	{65,"Keyboard A"},
	{83,"Keyboard S"},
	{68,"Keyboard D"},
	{38,"Keyboard UpArrow"},
	{39,"Keyboard RightArrow"},
	{40,"Keyboard DownArrow"},
	{37,"Keyboard LeftArrow"}

};

string* GetKeyNameFromVirtualKey(USHORT key)
{

	map<USHORT, string>::iterator entry = MapOfValidInputKeys.find(key);

	if(entry == MapOfValidInputKeys.end())
	{
		cout<<key<<" is not a valid input.\n\n";
		return NULL;
	}
	
	return &entry->second;	

}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
bool RegisterInputDevices(HWND targetWindow);

void WINAPI PaintWindow(HWND windowHandle)
{

	//Setup Painting Window
	PAINTSTRUCT paintStruct;
	HDC handleDeviceContext = BeginPaint(windowHandle, &paintStruct);

	//Paint
	FillRect(handleDeviceContext, &paintStruct.rcPaint, (HBRUSH) (COLOR_WINDOW+1));
	EndPaint(windowHandle, &paintStruct);

}


int WINAPI WinMain(HINSTANCE curentInstance, HINSTANCE previousInstance, LPSTR _, int ShowCommand)
{
	// Register the window class.
    const wchar_t MAIN_HANDLE_CLASS_NAME[]  = L"Sample Window Class";//L"MainHandleWindowClass";
    const wchar_t MAIN_HANDLE_WINDOW_NAME[]  = L"Main Program";

	WNDCLASS WindowClass = 	{};

    WindowClass.lpfnWndProc   = WindowProc;
    WindowClass.hInstance     = curentInstance;
    WindowClass.lpszClassName = MAIN_HANDLE_CLASS_NAME;

	RegisterClass(&WindowClass);

	HWND MainHandleWindow = CreateWindowEx(
		WS_EX_APPWINDOW,
		MAIN_HANDLE_CLASS_NAME,
		MAIN_HANDLE_WINDOW_NAME,
		WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL,
		NULL,
		curentInstance,
		NULL
	);
	
    if (MainHandleWindow == NULL)
    {
		cout<<"Main handle window not defined!\n\n";
        return 0;
    }

	RegisterInputDevices(MainHandleWindow);

    ShowWindow(MainHandleWindow, ShowCommand);    
	
	// Run the message loop.

    MSG message = { };
    while (GetMessage(&message, NULL, 0, 0) > 0)
    {
        TranslateMessage(&message);
        DispatchMessage(&message);
    }

    return 0;
}

bool RegisterInputDevices(HWND targetWindow)
{

	//For Hex references see: https://learn.microsoft.com/en-us/windows-hardware/drivers/hid/hid-architecture#hid-clients-supported-in-windows
	RAWINPUTDEVICE KeyboardInputDevice = {};
	KeyboardInputDevice.usUsagePage = 0x0001; //Keyboard / Keypad class driver and mapper driver
	KeyboardInputDevice.usUsage = 0x0006; //Keyboard
	KeyboardInputDevice.dwFlags = RIDEV_INPUTSINK;
	KeyboardInputDevice.hwndTarget = targetWindow;

	if(RegisterRawInputDevices(&KeyboardInputDevice, 1, sizeof(KeyboardInputDevice)))
	{
		cout<<"Successfully registered Keyboard\n\n";
		return eSuccess;
	}

	cout<<"Failed to register Keyboard\n\n";
	return eFailure;

}

LRESULT CALLBACK WindowProc(HWND windowHandle, UINT message, WPARAM params, LPARAM longParams)
{
	switch(message)
	{
		case WM_DESTROY:
			cout<<"Program super Ending\n\n";
			PostQuitMessage(0);
			return 0;

		case WM_INPUT:
			{
				
				char buffer[sizeof(RAWINPUT)] = {};
  				UINT size = sizeof(RAWINPUT);
  				GetRawInputData(reinterpret_cast<HRAWINPUT>(longParams), RID_INPUT, buffer, &size, sizeof(RAWINPUTHEADER));
				RAWINPUT* raw = reinterpret_cast<RAWINPUT*>(buffer);
				const RAWKEYBOARD& rawKB = raw->data.keyboard;
				string* KeyAsStringPointer = GetKeyNameFromVirtualKey(rawKB.VKey);
				
				if(KeyAsStringPointer != nullptr)
				{
					string KeyAsString = *KeyAsStringPointer;
					cout<<"Receiving Input. Input Is "<<KeyAsString<<" | "<<rawKB.VKey<<"\n\n";

				}

			}
			return 0;

		case WM_PAINT:
			{

				PaintWindow(windowHandle);

			}
			return 0;

	}
	
	return DefWindowProc(windowHandle, message, params, longParams);
}