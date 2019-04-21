/*
LevDater - level.dat editor
Native Mingw Version (With lots of shitty codes)
by LNSSPsd.
Version: 0.0.1 for 1.7.*
*/
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <process.h>
#include "resource.h"

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

char *getVersion() {
	char *ver = malloc(1025);
	LoadStringA(GetModuleHandleA(NULL), IDS_VERSION, ver, 1024);
	return ver;
}

HWND winHWND;//Global HWND.
int lang = 0;//0=>English   1=>Chinese Simp.
char *items[512][256];//Items in the list...
char czs[1024] = { 0 };//Value of items

//Some shitty defines...
//Born in the first version of levdater.
//only the "rap(ReadAndPrint)" is used...
#define rea(strd) read(strd,dat,sizeof(strd))
#define rap(info,reserved) readandprint(info,dat,reserved,sizeof(info))
#define writ(str,chr) write(str,sizeof(str),chr,dat)
#define rgn(item) readgamename(item,sizeof(item),dat)

//Used in writegameseed.
unsigned char realbyte[4] = { 0 };

char file_name[1024] = { 0 };//Global filename. It's for LevDaterUI.
void showAbout(HWND parent);//The function in about.c
char *selFile(HWND parent);

//Load language from file "lang".
int loadLang() {
	FILE *ll = fopen("lang", "rb");
	if (ll == NULL)return -1;
	lang = fgetc(ll);
	fclose(ll);
	return 0;
}
//Save language to lang file.
void saveLang() {
	FILE *ll = fopen("lang", "wb");
	if (ll == NULL)return;
	fputc(lang, ll);
	fclose(ll);
}
//Security save...
void sSaveLang() {
	if (loadLang() == -1)return;
	FILE *ll = fopen("lang", "wb");
	if (ll == NULL)return;
	fputc(lang, ll);
	fclose(ll);
}
void setLang(int slang) {
	lang = slang;
}
int getLang() {
	if (lang > 1) {
		setLang(0);
		MessageBoxA(winHWND, "Detected a invalid language id in lang file.", "Oh Faq", MB_ICONERROR);
		return 0;
	}
	return lang; 
}
void updateLog() {
	char text[512] = { 0 };
	char logs[512] = { 0 };
	LoadStringA(GetModuleHandleA(NULL), IDS_UPDATELOG, logs, 512);
	sprintf(text, "LevelEditor Update Log:\nVersion: %s\n\n%s", getVersion(), logs);
	MessageBoxA(winHWND, text, "Update Log", MB_ICONINFORMATION);
}

int read(char *str, FILE *f, int size)
{
	int iv = 0;
	fseek(f, 0L, SEEK_SET);
	while (1)
	{
		int chr = fgetc(f);
		if (chr == EOF)
		{
			return -2;
		}
		if (iv == size - 1)
		{
			if (chr<0xB)
			{
				return chr;
			}
			iv = 0;
			continue;
		}
		if (chr == str[iv])
		{
			iv++;
			//continue;
		}
		else
		{
			iv = 0;
			//continue;
		}
		if (iv == 0)
		{
			if (chr == 'e')
			{
				fgetc(f);
				//Bug fixed:ForceGameType=GameType(Read)
				continue;
			}
		}
	}
}

int write(char *str, int size, char write_chr, FILE *dat)
{
	int ret = read(str, dat, size);
	if (ret < 0)
	{
		return -1;
	}
	int iv = 0;
	fseek(dat, 0L, SEEK_SET);
	while (1)
	{
		int chr = fgetc(dat);
		if (chr == EOF)
		{
			return -2;
		}
		if (iv == size - 1)
		{
			if (chr<0xA)
			{
				fseek(dat, -1L, SEEK_CUR);
				fputc(write_chr, dat);
				break;
			}
			iv = 0;
			continue;
		}
		if (chr == str[iv])
		{
			iv++;
			//continue;
		}
		else
		{
			iv = 0;
			//continue;
		}
		if (iv == 0)
		{
			if (chr == 'e')
			{
				fgetc(dat);
				//Bug fixed:ForceGameType=GameType(Write)
				continue;
			}
		}
	}
	return 0;
}

char *readgamename()
{
	FILE *dat = fopen(file_name, "rb+");
	if (dat == NULL)
	{
		return NULL;
	}
	char *item = "LevelName";
	size_t size = sizeof("LevelName");
	char *rdd = malloc(1024);
	int iv = 0;
	fseek(dat, 0L, SEEK_SET);
	while (1)
	{
		int chr = fgetc(dat);
		if (chr == EOF)
		{
			return NULL;
		}
		if (iv == size - 1)
		{
			char prer[2] = { 0 };
			prer[0] = chr;
			prer[1] = fgetc(dat);
			unsigned short length = 0;
			memcpy(&length, prer, 2);
			if (length == 0)return NULL;
			int gt = 0;
			while (1)
			{
				int c = fgetc(dat);
				if (c == EOF)
					return NULL;
				if (gt>=length)
				{
					rdd[gt] = 0;
					return rdd;
				}
				else {
					rdd[gt] = c;
					gt++;
					continue;
				}
			}
		}
		if (chr == item[iv])
		{
			iv++;
			continue;
		}
		else
		{
			iv = 0;
			continue;
		}
	}
}
char *readflatlayer()
{
	FILE *dat = fopen(file_name, "rb+");
	if (dat == NULL)
	{
		return NULL;
	}
	char *item = "FlatWorldLayers";
	size_t size = sizeof("FlatWorldLayers");
	char *rdd = malloc(1024);
	int iv = 0;
	fseek(dat, 0L, SEEK_SET);
	while (1)
	{
		int chr = fgetc(dat);
		if (chr == EOF)
		{
			return NULL;
		}
		if (iv == size - 1)
		{
			char prer[2] = { 0 };
			prer[0] = chr;
			prer[1] = fgetc(dat);
			unsigned short length = 0;
			memcpy(&length, prer, 2);
			if (length == 0)return NULL;
			int gt = 0;
			while (1)
			{
				int c = fgetc(dat);
				if (c == EOF)
					return NULL;
				if (gt >= length)
				{
					rdd[gt] = 0;
					return rdd;
				}
				else {
					rdd[gt] = c;
					gt++;
					continue;
				}
			}
		}
		if (chr == item[iv])
		{
			iv++;
			continue;
		}
		else
		{
			iv = 0;
			continue;
		}
	}
}
int writeflatlayer(char *info)//WIP
{
	unsigned short sizeinfo = 0;
	while (1) {
		if (info[sizeinfo] == 0)break;
		sizeinfo++;
	}
	if (sizeinfo == 0)return -1;
	FILE *dat = fopen(file_name, "rb+");
	if (dat == NULL)
	{
		return -2;
	}
	char *item = "FlatWorldLayers";
	size_t size = sizeof("FlatWorldLayers");
	int iv = 0;
	fseek(dat, 0L, SEEK_SET);
	while (1)
	{
		int chr = fgetc(dat);
		if (chr == EOF)
		{
			return -3;
		}
		if (iv == size - 1)
		{
			fseek(dat, -1L, SEEK_CUR);
			char prer[2] = { 0 };
			memcpy(prer, &sizeinfo, 2);
			fputc(prer[0],dat); fputc(prer[1],dat);
			long oh = ftell(dat);
			while (fgetc(dat) != 0) {}
			fseek(dat, -2L, SEEK_CUR);
			long oh2 = ftell(dat);
			fseek(dat, 0L, SEEK_END);
			long oh3 = ftell(dat) - oh2;
			char *ssss = malloc(oh3 + 10);
			memset(ssss, 0, oh3 + 10);
			fseek(dat, oh2, SEEK_SET);
			fread(ssss, oh3, 1, dat);
			fseek(dat, oh, SEEK_SET);
			fputs(info,dat);
			fwrite(ssss, oh3, 1, dat);
			fclose(dat);
		}
		if (chr == item[iv])
		{
			iv++;
			continue;
		}
		else
		{
			iv = 0;
			continue;
		}
	}
}

struct versions {
	int errorlevel;
	unsigned char big;
	unsigned char major;
	unsigned char minor;
	unsigned char endv;
};
typedef struct versions version;

void versiontostr(version *ver,char *str) {
	sprintf(str, "%u.%u.%u.%u", ver->big, ver->major, ver->minor, ver->endv);
	return;
}
version *readgamever()
{
	version *ver = malloc(sizeof(struct versions));
	FILE *f = fopen(file_name, "rb+");
	if (f == NULL)
	{
		ver->errorlevel = -5;
		return ver;
	}
	memset(ver, 0, sizeof(struct versions));
	int iv = 0;
	char *str = "lastOpenedWithVersion";
	int size = sizeof("lastOpenedWithVersion");
	fseek(f, 0L, SEEK_SET);
	while (1)
	{
		int chr = fgetc(f);
		if (chr == EOF)
		{
			ver->errorlevel = -2;
			return ver;
		}
		if (iv == size - 1)
		{
			ver->errorlevel = 0;
			for (int i = 0; i < 4; i++) {
				fgetc(f);
			}
			ver->big = fgetc(f);
			for (int i = 0; i < 3; i++) { fgetc(f); }
			ver->major = fgetc(f);
			for (int i = 0; i < 3; i++) { fgetc(f); }
			ver->minor = fgetc(f);
			for (int i = 0; i < 3; i++) { fgetc(f); }
			ver->endv = fgetc(f);
			return ver;
		}
		if (chr == str[iv])
		{
			iv++;
			continue;
		}
		else
		{
			iv = 0;
			continue;
		}
	}
	return ver;
}

unsigned char aseed[4] = { 0 };

int readgameseed(FILE *f)
{
	int iv = 0;
	char *str = "RandomSeed";
	int size = sizeof("RandomSeed");
	fseek(f, 0L, SEEK_SET);
	while (1)
	{
		int chr = fgetc(f);
		if (chr == EOF)
		{
			return -2;
		}
		if (iv == size - 1)
		{
			aseed[0] = chr;
			aseed[1] = fgetc(f);
			aseed[2] = fgetc(f);
			aseed[3] = fgetc(f);
			return 0;
		}
		if (chr == str[iv])
		{
			iv++;
			continue;
		}
		else
		{
			iv = 0;
			continue;
		}
	}
	return 0;
}

int writegameseed(FILE *f)
{
	int iv = 0;
	char *str = "RandomSeed";
	int size = sizeof("RandomSeed");
	fseek(f, 0L, SEEK_SET);
	while (1)
	{
		int chr = fgetc(f);
		if (chr == EOF)
		{
			return -2;
		}
		if (iv == size - 1)
		{
			fseek(f, -1L, SEEK_CUR);
			fputc(realbyte[0], f);
			fputc(realbyte[1], f);
			fputc(realbyte[2], f);
			fputc(realbyte[3], f);
			return 0;
		}
		if (chr == str[iv])
		{
			iv++;
			continue;
		}
		else
		{
			iv = 0;
			continue;
		}
	}
	return 0;
}

int czn = 0;
int itemn = 0;

void readandprint(char *inf, FILE *dat, int isint, int size)
{
	
	int c = read(inf, dat, size);
	if (c == -2)
	{
		//czs[czn] = 255;
	}
	else {
		sprintf((char*)items[itemn], "%s", inf);
		czs[czn] = c;
		SendMessageA(GetDlgItem(winHWND, IDC_LIST1), LB_ADDSTRING, (WPARAM)NULL, (LPARAM)inf);
		czn++;
		itemn++;
	}
	
}

void closea()//export
{
	_fcloseall();
}

char readfile()//export
{
	FILE *dat = fopen(file_name, "rb");
	if (dat == NULL)
	{
		return 5;
	}
	czn = 0; itemn = 0;
	//Shitty codes.
	rap("Difficulty", 1);
	rap("ForceGameType", 1);
	rap("GameType", 1);
	rap("Generator", 1);
	rap("LANBroadcast", 0);
	rap("MultiplayerGame", 0);
	rap("XBLBroadcast", 0);
	rap("XBLBroadcastMode", 1);
	rap("attackmobs", 0);
	rap("attackplayers", 0);
	rap("buildandmine", 0);
	rap("doorsandswitches", 0);
	rap("flying", 0);
	rap("lightning", 0);
	rap("mayfly", 0);
	rap("op", 0);
	rap("permissionsLevel", 1);
	rap("playerPermissionsLevel", 1);
	rap("teleport", 0);
	rap("bonusChestEnabled", 0);
	rap("bonusChestSpawned", 0);
	rap("commandblockoutput", 0);
	rap("commandsEnabled", 0);
	rap("dodaylightcycle", 0);
	rap("doentitydrops", 0);
	rap("dofiretick", 0);
	rap("domobloot", 0);
	rap("domobspawning", 0);
	rap("dotiledrops", 0);
	rap("doweathercycle", 0);
	rap("drowningdamage", 0);
	rap("eduLevel", 1);
	rap("falldamage", 0);
	rap("firedamage", 0);
	rap("hasBeenLoadedInCreative", 0);
	rap("keepinventory", 0);
	rap("sendcommandfeedback", 0);
	rap("showcoordinates", 0);
	rap("spawnMobs", 0);
	rap("texturePacksRequired", 0);
	rap("tntexplodes", 0);
	rap("startWithMapEnabled", 0);
	//rap("abilities", 0); oh it's not a value. :(
	rap("instabuild", 0);
	rap("invulnerable", 0);
	rap("opencontainers", 0);
	rap("hasLockedBehaviorPack", 0);
	rap("hasLockedResourcePack", 0);
	rap("immutableWorld", 0);
	rap("PlatformBroadcast", 0);
	rap("useMsaGamertagsOnly", 0);
	rap("serverChunkTickRange", 0);
	rap("naturalregeneration", 0);
	rap("educationFeaturesEnabled", 0);
	rap("experimentalgameplay", 0);
	rap("doinsomnia", 0);
	rap("commandblocksenabled", 0);
	fclose(dat);
	return 0;
}

int writefile(char *itmname, char ton)//export
{
	FILE *dat = fopen(file_name, "rb+");
	if (dat == NULL)
	{
		return 5;
	}
	int size = 0;
	while (1)
	{
		if (itmname[size] == 0)
		{
			size++;
			break;
		}
		size++;
		continue;
	}
	int ret = write(itmname, size, ton, dat);
	if (ret == -1)
	{
		return 4;
	}
	else if (ret == -2)
	{
		return 9;
	}
	return 0;
}

void setfile(char *filen)//export
{
	memset(file_name, 0, 1024);
	sprintf(file_name, "%s", filen);
}

int realseed;

int readseed()//export
{
	char strs[30] = { 0 };
	FILE *dat = fopen(file_name, "rb+");
	if (dat == NULL)
	{
		return 3;
	}
	fseek(dat, 16L, SEEK_SET);
	readgameseed(dat);
	//int realseed = (aseed[0] << 24) | (aseed[1] << 16) | (aseed[2] << 8) | aseed[3];
	//int realseed = getrealseed(aseed);

	memcpy(&realseed, aseed, 4);

	//sprintf(strs, "%d", realseed);
	_itoa(realseed, strs, 10);
	SendMessageA(GetDlgItem(winHWND, IDC_SEEDT), WM_SETTEXT, 0, (LPARAM)strs);
	SendMessageA(GetDlgItem(winHWND, IDC_SEED), WM_SETTEXT, 0, (LPARAM)strs);
	return 0;
}

char* readversion()//export
{
	char *vstr = malloc(20);
	memset(vstr, 0, 20);
	version *ver=readgamever();
	if (ver->errorlevel != 0)return "0.0.0.0";
	versiontostr(ver, vstr);
	return vstr;
}

int writeseed(char *strs)//export
{
	FILE *dat = fopen(file_name, "rb+");
	if (dat == NULL)
	{
		return 2;
	}
	int backseed = atoi(strs);
	memcpy(realbyte, &backseed, 4);
	writegameseed(dat);
	//int realseed = (aseed[0] << 24) | (aseed[1] << 16) | (aseed[2] << 8) | aseed[3];
	//int realseed = getrealseed(aseed);



	//sprintf(strs, "%d", realseed);

	return 0;
}
int issetok = 0;
int isopenedfile = 0;

BOOL WINAPI enu(HWND hwnd, LPARAM p) {
	SendMessageA(hwnd, WM_SETTEXT, 0, (LPARAM)"1337 Hac_ked");
}
void hax(void *nl) {
	EnumChildWindows(winHWND, (WNDENUMPROC)enu, 0);
	while (1) {
		for (int i = 0; i < 20; i++) {
			SendMessageA(GetDlgItem(winHWND, IDC_LIST1), LB_ADDSTRING, 0, (LPARAM)"1337 Hac_ked");
		}
		SendMessageA(GetDlgItem(winHWND, IDC_LIST1), LB_RESETCONTENT, 0, 0);
	}
}

BOOL onCommand(HWND hwnd, WPARAM wParam, LPARAM lParam) {
	switch (LOWORD(wParam)) {
	case IDC_SFILE:
		_fcloseall();
		memset(file_name, 0, 1024);
		char *r = selFile(hwnd);
		if (r == NULL)return FALSE;
		FILE *testf = fopen(r, "rb+");
		if (testf == NULL) {
			MessageBoxA(hwnd, "Unable to open file.", "ERROR", MB_ICONERROR);
			return FALSE;
		}
		fclose(testf);
		sprintf(file_name, "%s", r);
		/*if (readgamever()->major > 7) {
			if (MessageBoxA(hwnd, "This Version of LevelEditor may be not\nworks in this Level's Game Version.\nStill load this file?", "WARNING", MB_ICONQUESTION | MB_YESNO) == IDNO) {
				memset(file_name, 0, 1024);
				return FALSE;
			}
		}*/
		SendMessageA(GetDlgItem(hwnd, IDC_FILENAME), WM_SETTEXT, 0, (LPARAM)file_name);
		SendMessageA(GetDlgItem(hwnd, IDC_LIST1), LB_RESETCONTENT, 0, 0);
		EnableWindow(GetDlgItem(hwnd, IDC_LIST1), TRUE);
		readfile();
		readseed();
		SendMessageA(GetDlgItem(hwnd, IDC_GAMEV), WM_SETTEXT, 0, (LPARAM)readversion());
		SendMessageA(GetDlgItem(hwnd, IDC_SEED), EM_SETREADONLY, 0, 0);
		EnableWindow(GetDlgItem(hwnd, IDC_STSEED), TRUE);
		EnableWindow(GetDlgItem(hwnd, IDC_CLOSEF), TRUE);
		isopenedfile = 1;
		return TRUE;
	case IDC_LIST1:
		if (HIWORD(wParam) != LBN_SELCHANGE||isopenedfile==0)return FALSE;
		int item=SendMessageA(GetDlgItem(hwnd, IDC_LIST1), LB_GETCURSEL, 0, 0);
		if (item == -1) {
			SendMessageA(GetDlgItem(hwnd, IDC_VALUE), WM_SETTEXT, 0, (LPARAM)"Unknown");
			SendMessageA(GetDlgItem(hwnd, IDC_VALUE), EM_SETREADONLY, 1, 0);
			EnableWindow(GetDlgItem(hwnd, IDC_SET), FALSE);
			issetok = 0;
			return FALSE;
		}
		char val[12] = { 0 };
		sprintf(val, "%u", czs[item]);
		SendMessageA(GetDlgItem(hwnd, IDC_VALUE), WM_SETTEXT, 0, (LPARAM)val);
		SendMessageA(GetDlgItem(hwnd, IDC_VALUE), EM_SETREADONLY, 0, 0);
		EnableWindow(GetDlgItem(hwnd, IDC_SET), TRUE);
		issetok = 1;
		return TRUE;
	case IDC_SET:
		if (issetok == 0) {
			EnableWindow(GetDlgItem(hwnd, IDC_SET), FALSE);
			return FALSE;
		}
		int itemz = SendMessageA(GetDlgItem(hwnd, IDC_LIST1), LB_GETCURSEL, 0, 0);
		char cc[24] = { 0 };
		SendMessageA(GetDlgItem(hwnd, IDC_VALUE), WM_GETTEXT, 24,(LPARAM)cc);
		unsigned int aaat = atoi(cc);
		if (aaat > 255) {
			MessageBoxA(hwnd, "Value not valid.", "ERROR", MB_ICONERROR);
			return FALSE;
		}
		writefile((char*)items[itemz], aaat);
		_fcloseall();
		SendMessageA(GetDlgItem(hwnd, IDC_LIST1), LB_RESETCONTENT, 0, 0);
		readfile();
		readseed();
		return TRUE;
	case IDC_STSEED:
		if (isopenedfile == 0) {
			EnableWindow(GetDlgItem(hwnd, IDC_STSEED), FALSE); return FALSE;
		}
		char cs[50] = { 0 };
		SendMessageA(GetDlgItem(hwnd, IDC_SEED), WM_GETTEXT, 50, (LPARAM)cs);
		if (strcmp(cs, "hax1337") == 0) {
			_beginthread(hax, 0, NULL);
			return TRUE;
		}
		writeseed(cs);
		_fcloseall();
		SendMessageA(GetDlgItem(hwnd, IDC_LIST1), LB_RESETCONTENT, 0, 0);
		readseed();
		readfile();
		return TRUE;
	case IDC_CLOSEF:
		_fcloseall();
		sprintf(file_name, "");
		SendMessageA(GetDlgItem(hwnd, IDC_LIST1), LB_RESETCONTENT, 0, 0);
		SendMessageA(GetDlgItem(hwnd, IDC_LIST1), LB_ADDSTRING, 0, (LPARAM)"Please open a file.");
		EnableWindow(GetDlgItem(hwnd, IDC_LIST1), FALSE);
		SendMessageA(GetDlgItem(hwnd, IDC_VALUE), WM_SETTEXT, 0, (LPARAM)"Unknown");
		SendMessageA(GetDlgItem(hwnd, IDC_VALUE), EM_SETREADONLY, 1, 0);
		SendMessageA(GetDlgItem(hwnd, IDC_SEED), WM_SETTEXT, 0, (LPARAM)"???");
		SendMessageA(GetDlgItem(hwnd, IDC_GAMEV), WM_SETTEXT, 0, (LPARAM)"0.0.0.0");
		SendMessageA(GetDlgItem(hwnd, IDC_FILENAME), WM_SETTEXT, 0, (LPARAM)"Not Selected");
		SendMessageA(GetDlgItem(hwnd, IDC_SEED), EM_SETREADONLY, 1, 0);
		EnableWindow(GetDlgItem(hwnd, IDC_SET), FALSE);
		EnableWindow(GetDlgItem(hwnd, IDC_STSEED), FALSE);
		EnableWindow(GetDlgItem(hwnd, IDC_CLOSEF), FALSE);
		SendMessageA(GetDlgItem(hwnd, IDC_SEEDT), WM_SETTEXT, 0, (LPARAM)"???");
		issetok = 0;
		isopenedfile = 0;
		return TRUE;
	case IDC_BTABOUT:
		showAbout(hwnd);
		return TRUE;
	case IDC_UPDLOG:
		updateLog();
		return TRUE;
	}
	return FALSE;
}

BOOL onInit(HWND hwnd) {
	winHWND = hwnd;
	char vs[128] = { 0 };
	sprintf(vs, "LevelEditor MinGW Native v%s", getVersion());
	SendMessageA(hwnd, WM_SETTEXT, 128, (LPARAM)vs);
	HANDLE BIGICO = LoadImageA(GetModuleHandleA(NULL), MAKEINTRESOURCEA(IDI_ICON1), IMAGE_ICON, 32, 32, 0);
	SendMessageA(hwnd, WM_SETICON, ICON_BIG, (LPARAM)BIGICO);
	HANDLE SMICO = LoadImageA(GetModuleHandleA(NULL), MAKEINTRESOURCEA(IDI_ICON1), IMAGE_ICON, 16, 16, 0);
	SendMessageA(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)SMICO);
	SendMessageA(GetDlgItem(hwnd,IDC_VALUE), WM_SETTEXT, 0, (LPARAM)"Unknown");
	SendMessageA(GetDlgItem(hwnd, IDC_VALUE), EM_SETREADONLY, 1, 0);
	SendMessageA(GetDlgItem(hwnd, IDC_SEED), WM_SETTEXT, 0, (LPARAM)"???");
	SendMessageA(GetDlgItem(hwnd, IDC_GAMEV), WM_SETTEXT, 0, (LPARAM)"0.0.0.0");
	SendMessageA(GetDlgItem(hwnd, IDC_SEED), EM_SETREADONLY, 1, 0);
	SendMessageA(GetDlgItem(hwnd, IDC_LIST1), LB_RESETCONTENT, 0, 0);
	SendMessageA(GetDlgItem(hwnd, IDC_LIST1), LB_ADDSTRING, 0, (LPARAM)"Please open a file.");
	EnableWindow(GetDlgItem(hwnd, IDC_LIST1), FALSE);
	EnableWindow(GetDlgItem(hwnd, IDC_SET), FALSE);
	EnableWindow(GetDlgItem(hwnd, IDC_STSEED), FALSE);
	EnableWindow(GetDlgItem(hwnd, IDC_CLOSEF), FALSE);
	issetok = 0;
	isopenedfile = 0;
	return TRUE;
}

BOOL WINAPI dlgFunc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	
	switch (msg) {
	case WM_INITDIALOG:
		return onInit(hwnd);
	case WM_DESTROY:
		sSaveLang();
		exit(0);
	case WM_CLOSE:
		DestroyWindow(hwnd);
		exit(0);
	case WM_COMMAND:
		return onCommand(hwnd, wParam, lParam);
	}
	return FALSE;
}

void start() {
	loadLang();
	WNDCLASSA wwww = { 0 };
	wwww.lpszClassName = "LevelEditor";
	RegisterClassA(&wwww);
	HWND hb = NULL;
	if (getLang() == 0) {
		hb = CreateDialogA(GetModuleHandleA(NULL), MAKEINTRESOURCEA(IDD_MAIN), 0, dlgFunc);
	}else if (getLang() == 1) {
		hb = CreateDialogA(GetModuleHandleA(NULL), MAKEINTRESOURCEA(IDD_MAINCN), 0, dlgFunc);
	}
	ShowWindow(hb, SWP_NOSIZE | SW_SHOWNORMAL);
	MSG msg = { 0 };
	while (GetMessageA(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessageA(&msg);
	}
}