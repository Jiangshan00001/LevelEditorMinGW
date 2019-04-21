#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <process.h>
#include <windows.h>
#include "resource.h"

char *fn;
int ok = 0;

BOOL sfCommand(HWND hwnd, WPARAM wParam, LPARAM lParam) {
	switch (LOWORD(wParam)) {
	case IDOK:
		SendMessageA(GetDlgItem(hwnd, IDC_FN), WM_GETTEXT, 1024, (LPARAM)fn);
		ok = 1;
		DestroyWindow(hwnd);
		return TRUE;
	case IDCANCEL:
		fn = NULL;
		DestroyWindow(hwnd);
		ok = 1;
		return TRUE;
	}
	return FALSE;
}

BOOL sfonInit(HWND hwnd) {
	SendMessageA(GetDlgItem(hwnd, IDC_FN), WM_SETTEXT, 0, (LPARAM)"level.dat");
	return TRUE;
}

BOOL WINAPI sfFunc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_INITDIALOG:
		return sfonInit(hwnd);
	case WM_COMMAND:
		return sfCommand(hwnd, wParam, lParam);
	}
	return FALSE;
}

char *selFile(HWND parent) {
	ok = 0;
	fn = malloc(1024);
	memset(fn, 0, 1024);
	WNDCLASSA wwww = { 0 };
	wwww.lpszClassName = "LevelEditor_SELFILED";
	RegisterClassA(&wwww);
	HWND hb = CreateDialogA(GetModuleHandleA(NULL), MAKEINTRESOURCEA(IDD_SELFILE), parent, sfFunc);
	ShowWindow(hb, SWP_NOSIZE | SW_SHOWNORMAL);
	MSG msg = { 0 };
	while (GetMessageA(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessageA(&msg);
		if (ok == 1)return fn;
	}
	return fn;
}