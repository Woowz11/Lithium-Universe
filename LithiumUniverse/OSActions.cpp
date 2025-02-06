#include <Windows.h>
#include <string>

/* Вызвать всплывающее окно с ошибкой */
void CrashMessageBox(std::wstring CrashInfo) {
	MessageBox(
		NULL,
		CrashInfo.c_str(),
		L"Crash!",
		MB_ICONERROR | MB_OK
	);
}

/* Не найден нужный DLL файл! */
void NotFoundDLLMessageBox(std::wstring DLLFileName) {
	CrashMessageBox(L"Not found DLL file [ " + DLLFileName + L" ]!");
}