#include <Windows.h>
#include <iostream>
#include <string>
#include <chrono>
#include <vector>
#include <ctime>
#include <map>

#include "Console.h";

/* Скрипт [Вывод сообщений в консоль и логирование их] */

/* Начало цветового кода */
const std::string ColorCodePrefix = "$$";

/* Массив цветовых кодов */
const std::map<std::string, std::string> ColorCodes = {
	{ColorCodePrefix + "_","\u001b[37m"     }, /* Белый (ресет) */
	{ColorCodePrefix + "R","\u001b[31m"     }, /* Красный    */
	{ColorCodePrefix + "G","\u001b[32m"     }, /* Зелёный    */
	{ColorCodePrefix + "B","\u001b[34m"     }, /* Синий      */
	{ColorCodePrefix + "Y","\u001b[33m"     }, /* Жёлтый     */
	{ColorCodePrefix + "A","\u001b[38;5;14m"}, /* Голубой    */
	{ColorCodePrefix + "P","\u001b[35m"     }, /* Фиолетовый */
	{ColorCodePrefix + "L","\u001b[38;5;8m" }  /* Серый      */
};

/* Приставки к типу сообщения (связано с SendLogType) */
const std::vector<std::string> LogTypePrefixes = {
	"RII",
	"RIM",
	"RWR",
	"RWS",
	"RER",
	"RFT",
	"RDR",
	"RDB",
	"RDG",
	"RDY"
};

/* Консоль */
HANDLE Console;

/* Сообщения были хоть раз отправлены? */
bool MessagesHaveBeenSentBeforeThis = false;

/* Заменить свои префиксы цветовые на настоящие цветовые */
std::string ReplaceColorCodesToRealColors(std::string Message, bool ThatLog) {
	std::string Result = Message;
	for (const auto& color : ColorCodes) {
		size_t pos = 0;
		while ((pos = Result.find(color.first, pos)) != std::string::npos) {
			Result.replace(pos, color.first.length(), ThatLog? "" : color.second);
			if (!ThatLog) {
				pos += color.second.length();
			}
		}
	}
	return Result;
}

/* Отправить сообщение в консоль с цветами */
void CoutWithColors(std::string Message) {
	std::cout << ReplaceColorCodesToRealColors(Message,false).c_str();
}

/* Заполнить строку символами */
std::string FillString(std::string target, char symbol, int length) {
	if (target.length() >= length) {
		return target;
	}

	int fillLength = length - target.length();
	std::string filledString(fillLength, symbol);
	return filledString + target;
}

/* Получить основную часть сообщения */
std::string GetPrintMessageBaseDecorations(std::string Base, SendLogType SLT, bool ThatForLogs) {
	std::string Result = MessagesHaveBeenSentBeforeThis?"\n" : "";

	Result += ColorCodePrefix + LogTypePrefixes[SLT] + ColorCodePrefix + "_:";

	/* Время в сообщении */
	auto now = std::chrono::system_clock::now().time_since_epoch();
	auto now_forlocal = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	std::tm l;
	localtime_s(&l, &now_forlocal);

	int milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now).count() % 1000;
	int seconds      = l.tm_sec;
	int minutes      = l.tm_min;
	int hours        = l.tm_hour;

	std::string TimePart = FillString(std::to_string(hours), ' ', 2) + ":" + FillString(std::to_string(minutes), ' ', 2) + ":" + FillString(std::to_string(seconds), ' ', 2) + ":" + FillString(std::to_string(milliseconds), ' ', 3);

	Result += "[" + ColorCodePrefix + "L" + TimePart + ColorCodePrefix + "_]";

	Result += "[" + ColorCodePrefix + FillString(Base, ' ', 7) + ColorCodePrefix + "_]";

	return Result+": ";
}

/* Отправить сообщение в логи (основа) */
void PrintLogBase(std::string Base, SendLogType SLT, std::string Message) {
	std::string Result = GetPrintMessageBaseDecorations(Base, SLT, true) + Message;
}

/* Отправить сообщение в консоль (основа) */
void PrintConsoleBase(std::string Base, SendLogType SLT, std::string Message) {
	std::string Result = GetPrintMessageBaseDecorations(Base, SLT, false) + Message;
	CoutWithColors(Result);
}

/* Отправить сообщение (основа) */
void PrintBase(std::string Base,MessageType MT, SendLogType SLT, std::string Message) {
	switch (MT)
	{
		case OnlyConsole:
			PrintConsoleBase(Base, SLT, Message);
			break;
		case OnlyLog:
			PrintLogBase(Base, SLT, Message);
			break;
		default:
			PrintConsoleBase(Base, SLT, Message);
			PrintLogBase(Base, SLT, Message);
			break;
	}
	MessagesHaveBeenSentBeforeThis = true;
}

void Print(const char* message) {
	PrintBase("testipyki", Both, Info, (std::string)message);
}
void Print(std::string message) {
	Print(message.c_str());
}
void Print(double message) {
	Print(std::to_string(message));
}
void Print(bool message) {
	Print(message?"true" : "false");
}

/* Регистрация консоли */
void InstallConsole() {
	Console = GetStdHandle(STD_OUTPUT_HANDLE);
}