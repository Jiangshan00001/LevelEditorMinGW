#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <process.h>
#include <windows.h>
#include "resource.h"

void showAbout(HWND parent);
void setLang(int language);
int getLang();
void saveLang();

void __chkstk_ms(){}

HWND thish;
BOOL abtCommand(HWND hwnd, WPARAM wParam, LPARAM lParam) {
	switch (LOWORD(wParam)) {
	case IDC_OK:
		DestroyWindow(hwnd);
		return TRUE;
	case IDC_LANG:
		if (HIWORD(wParam) != CBN_SELCHANGE)return FALSE;
		setLang(SendMessageA(GetDlgItem(hwnd, IDC_LANG), CB_GETCURSEL, 0, 0));
		saveLang();
		if (getLang() == 0) {
			if (MessageBoxA(thish, "Language changed.\nRestart LevelEditor to update language.\nRestart Now?", "Done", MB_ICONINFORMATION | MB_YESNO) == IDYES) {
				char *name[1024] = { 0 };
				GetModuleFileNameA(NULL, name, 1024);
				ShellExecuteA(0, "open", name, "", "", SW_SHOWNORMAL);
				exit(0);
			}
		}else if (getLang() == 1) {
			if (MessageBoxA(thish, "语言修改完成。\n重新启动LevelEditor来更新语言。\n要现在重启吗？", "操作完成", MB_ICONINFORMATION|MB_YESNO) == IDYES) {
				char *name[1024] = { 0 };
				GetModuleFileNameA(NULL, name, 1024);
				ShellExecuteA(0, "open", name, "", "", SW_SHOWNORMAL);
				exit(0);
			}
		}
		DestroyWindow(hwnd);
		showAbout(thish);
		return TRUE;
	}
	return FALSE;
}

BOOL abtonInit(HWND hwnd) {
	HANDLE BIGICO = LoadImageA(GetModuleHandleA(NULL), MAKEINTRESOURCEA(IDI_ICON1), IMAGE_ICON, 32, 32, 0);
	SendMessageA(GetDlgItem(hwnd,IDC_PIC), STM_SETICON, (WPARAM)BIGICO,(LPARAM)0);
	char vs[60] = { 0 };
	getLang()?sprintf(vs, "版本: %s", getVersion()):sprintf(vs, "Version: %s", getVersion());
	SendMessageA(GetDlgItem(hwnd,IDC_VS), WM_SETTEXT, 60, (LPARAM)vs);
	//SendMessageA(GetDlgItem(hwnd, IDC_RLS), WM_SETTEXT, 0, "Released all rights reserved. Open source will come soon.");
	SendMessageA(GetDlgItem(hwnd, IDC_LANG), CB_ADDSTRING, 0, "简体中文");
	SendMessageA(GetDlgItem(hwnd, IDC_LANG), CB_ADDSTRING, 0, "English");
	SendMessageA(GetDlgItem(hwnd, IDC_LANG), CB_SETCURSEL, getLang(), 0);
	return TRUE;
}

BOOL WINAPI abtFunc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_INITDIALOG:
		return abtonInit(hwnd);
	case WM_CLOSE:
		DestroyWindow(hwnd);
		return TRUE;
	case WM_COMMAND:
		return abtCommand(hwnd,wParam,lParam);
	}
	return FALSE;
}

void showAbout(HWND parent) {
	thish = parent;
	WNDCLASSA wwww = { 0 };
	wwww.lpszClassName = "LevelEditor_AboutBox_LevDater";
	RegisterClassA(&wwww);
	HWND hb = NULL;
	if (getLang() == 0) {
		hb = CreateDialogA(GetModuleHandleA(NULL), MAKEINTRESOURCEA(IDD_ABOUT), parent, abtFunc);
	}
	else if (getLang() == 1) {
		hb = CreateDialogA(GetModuleHandleA(NULL), MAKEINTRESOURCEA(IDD_ABOUTCN), parent, abtFunc);
	}
	ShowWindow(hb, SWP_NOSIZE | SW_SHOWNORMAL);
	MSG msg = { 0 };
	while (GetMessageA(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessageA(&msg);
	}
}