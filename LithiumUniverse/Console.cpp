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
	{ColorCodePrefix + "_","\u001b[37m"       }, /* Белый (ресет) */
	{ColorCodePrefix + "R","\u001b[31m"       }, /* Тёмн. Красный */
	{ColorCodePrefix + "G","\u001b[38;5;10m"  }, /* Зелёный       */
	{ColorCodePrefix + "B","\u001b[34m"       }, /* Синий         */
	{ColorCodePrefix + "Y","\u001b[38;5;226m" }, /* Жёлтый        */
	{ColorCodePrefix + "A","\u001b[38;5;14m"  }, /* Голубой       */
	{ColorCodePrefix + "P","\u001b[35m"       }, /* Фиолетовый    */
	{ColorCodePrefix + "L","\u001b[38;5;8m"   }, /* Серый         */
	{ColorCodePrefix + "W","\u001b[38;5;248m" }, /* Светл. Серый  */
	{ColorCodePrefix + "F","\u001b[38;5;9m"   }, /* Красный       */
	{ColorCodePrefix + "O","\u001b[38;5;202m" }, /* Оранжевый     */
	{ColorCodePrefix + "C","\u001b[38;5;11m"  }, /* Светл. Жёлтый */
};

/* Приставки к типу сообщения (связано с SendLogType) */
const std::vector<std::string> LogTypePrefixes = {
	"WII",
	"AIM",
	"CWR",
	"OWS",
	"RER",
	"FFT",
	"RDR",
	"BDB",
	"GDG",
	"YDY"
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
std::string FillString(std::string target, char symbol, int length, bool ToRight) {
	if (target.length() >= length) {
		return target;
	}

	int fillLength = length - target.length();
	std::string filledString(fillLength, symbol);
	return ToRight? filledString + target : target + filledString;
}

/* Получить основную часть сообщения */
std::string GetPrintMessageBaseDecorations(std::string Base, SendLogType SLT, bool ThatForLogs) {
	std::string Result = MessagesHaveBeenSentBeforeThis?"\n" : "";

	std::string LogTypePrefix = LogTypePrefixes[SLT];
	std::string FirstChar(1,LogTypePrefix[0]);

	Result += ColorCodePrefix + LogTypePrefix + ColorCodePrefix + "_:";

	/* Время в сообщении */
	auto now = std::chrono::system_clock::now().time_since_epoch();
	auto now_forlocal = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	std::tm l;
	localtime_s(&l, &now_forlocal);

	int milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now).count() % 1000;
	int seconds      = l.tm_sec;
	int minutes      = l.tm_min;
	int hours        = l.tm_hour;

	std::string TimePart = FillString(std::to_string(hours), ' ', 2, true) + ":" + FillString(std::to_string(minutes), '0', 2, true) + ":" + FillString(std::to_string(seconds), '0', 2, true) + ":" + FillString(std::to_string(milliseconds), '0', 3, true);

	Result += "[" + ColorCodePrefix + FirstChar + TimePart + ColorCodePrefix + "_]";

	Result += "[" + ColorCodePrefix + FirstChar + FillString(Base, ' ', 7, false) + ColorCodePrefix + "_]";

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

/* ==== Отправка сообщений ==== */

/* Отправить обычное сообщение */
void Print(std::string Base, std::string Message) {
	PrintBase(Base, Both, SLT_Info, Message);
}

/* Отправить важное сообщение */
void PrintImportant(std::string Base, std::string Message) {
	PrintBase(Base, Both, SLT_Important, Message);
}

/* Отправить предупреждение */
void Warn(std::string Base, std::string Message) {
	PrintBase(Base, Both, SLT_Warning, Message);
}

/* Отправить серьёзное предупреждение */
void WarnSerious(std::string Base, std::string Message) {
	PrintBase(Base, Both, SLT_WarningSerious, Message);
}

/* Отправить ошибку */
void Error(std::string Base, std::string Message) {
	PrintBase(Base, Both, SLT_Error, Message);
}

/* Отправить фатальную ошибку */
void Fatal(std::string Base, std::string Message) {
	PrintBase(Base, Both, SLT_Fatal, Message);
}

/* ==== Работа с консолью ==== */

/* Регистрация консоли */
void InstallConsole() {
	Console = GetStdHandle(STD_OUTPUT_HANDLE);
}