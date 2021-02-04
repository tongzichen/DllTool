#pragma once
#include <Windows.h>
#include <imagehlp.h>
class BasePe
{
private:
	LPTSTR lpFilename;
	PIMAGE_DOS_HEADER DosHead;
	PIMAGE_NT_HEADERS PeHead;
	PIMAGE_FILE_HEADER PeFileHead;
	PIMAGE_OPTIONAL_HEADER PeOptionalHead;
public:
	BasePe(LPTSTR Path = L"") :lpFilename(Path) {}
	bool SetValue();
	bool LoadFileR();
	bool IsPEFile();
	PIMAGE_NT_HEADERS  GetNtHeaders()
	{
		return PeHead;
	}
	PIMAGE_DOS_HEADER  GetDosHeaders()
	{
		return DosHead;
	}
	PIMAGE_FILE_HEADER GetFileHeaders()
	{
		return PeFileHead;
	}
	PIMAGE_OPTIONAL_HEADER GetOptionHeaders()
	{
		return PeOptionalHead;
	}
};
bool BasePe::SetValue()
{//��ʼ��������Աһ����ʧ�ܷ���false;
	if (!LoadFileR())return false;
	PeHead = reinterpret_cast<PIMAGE_NT_HEADERS>((DWORD)DosHead + DosHead->e_lfanew);
	if (!PeHead)return false;
	PeFileHead = &PeHead->FileHeader;
	if (!PeFileHead)return false;
	PeOptionalHead = &PeHead->OptionalHeader;
	if (!PeOptionalHead)return false;
	return true;
}
bool BasePe::LoadFileR()
{//�����ļ�����ʼ��DosHead��Ա����ʼ���ɹ�����TRUE
	if (lpFilename == L"")	return false;
	HANDLE hFile;
	HANDLE hMapping;
	LPVOID ImageBase;
	hFile = CreateFile(lpFilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL, 0);

	if (!hFile)return false;

	hMapping = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
	if (!hMapping)
	{
		CloseHandle(hFile);
		return FALSE;
	}
	ImageBase = MapViewOfFile(hMapping, FILE_MAP_READ, 0, 0, 0);
	if (!ImageBase)
	{
		CloseHandle(hMapping);
		CloseHandle(hFile);
		return FALSE;
	}
	DosHead = static_cast<PIMAGE_DOS_HEADER>(ImageBase);
	return true;
}
bool BasePe::IsPEFile()
{//�жϼ��ص��ļ��Ƿ���PE�ļ���˳�㽫PeHead��ֵ
	if (!DosHead)
		return false;
	if (DosHead->e_magic != IMAGE_DOS_SIGNATURE)
		return false;
	if (PeHead->Signature != IMAGE_NT_SIGNATURE)
		return false;

	return true;
}