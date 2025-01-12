#include <string>
#pragma once

enum SendLogType {
	Info = 0,             /* Простое сообщение                    */
	Important = 1,        /* Важное сообщение                     */
	Warning = 2,          /* Предупреждение                       */
	WarningSerious = 3,   /* Серьёзное предупреждение             */
	Error = 4,            /* Ошибка                               */
	Fatal = 5,            /* Фатальная ошибка                     */
	DebugRed = 6,         /* Отладочное помеченное красным цветом */
	DebugBlue = 8,        /* Отладочное помеченное синим цветом   */
	DebugGreen = 7,       /* Отладочное помеченное зелёным цветом */
	DebugYellow = 9       /* Отладочное помеченное жёлтым цветом  */
};

enum MessageType {
	Both = 0,        /* Оба варианта одновременно            */
	OnlyConsole = 1, /* Отправить сообщение только в консоль */
	OnlyLog = 2      /* Отправить сообщение только в логи    */
};

void InstallConsole();

void PrintBase(std::string Base, MessageType MT, SendLogType SLT, std::string Message);
void Print(std::string message);
void Print(const char* message);
void Print(double message);
void Print(bool message);