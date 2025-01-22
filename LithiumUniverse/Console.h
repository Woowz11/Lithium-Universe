#pragma once
#include <string>

enum SendLogType {
	SLT_Info           = 0, /* Простое сообщение                    */
	SLT_Important      = 1, /* Важное сообщение                     */
	SLT_Warning        = 2, /* Предупреждение                       */
	SLT_WarningSerious = 3, /* Серьёзное предупреждение             */
	SLT_Error          = 4, /* Ошибка                               */
	SLT_Fatal          = 5, /* Фатальная ошибка                     */
	SLT_Debug          = 6, /* Отладочное                           */
	SLT_DebugRed       = 7, /* Отладочное помеченное красным цветом */
	SLT_DebugGreen     = 8, /* Отладочное помеченное зелёным цветом */
	SLT_DebugBlue      = 9, /* Отладочное помеченное синим цветом   */
	SLT_DebugYellow    = 10 /* Отладочное помеченное жёлтым цветом  */
};

enum MessageType {
	Both = 0,        /* Оба варианта одновременно            */
	OnlyConsole = 1, /* Отправить сообщение только в консоль */
	OnlyLog = 2      /* Отправить сообщение только в логи    */
};

void PrintBase(std::string Base, MessageType MT, SendLogType SLT, std::string Message);

void Print(std::string Base, std::string Message);
void PrintFast(std::string Base, std::string Message);
void PrintVeryFast(std::string Message);
void PrintVeryFastU32(std::u32string Message);
void PrintImportant(std::string Base, std::string Message);
void Warn(std::string Base, std::string Message);
void WarnSerious(std::string Base, std::string Message);
void Error(std::string Base, std::string Message);
void ErrorFromLog(std::string Base, std::string Message);
void Fatal(std::string Base, std::string Message);
void PrintDebug(std::string Base, std::string Message);
void PrintDebugR(std::string Base, std::string Message);
void PrintDebugG(std::string Base, std::string Message);
void PrintDebugB(std::string Base, std::string Message);
void PrintDebugY(std::string Base, std::string Message);

void CloseConsole();
void InstallConsole();