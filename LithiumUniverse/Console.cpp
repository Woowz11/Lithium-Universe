#include <Windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <vector>
#include <ctime>
#include <map>

#include "ExplorerActions.h";
#include "StringActions.h";
#include "Console.h";

/* Скрипт [Вывод сообщений в консоль и логирование их] */

/* Начало цветового кода */
const std::string ColorCodePrefix = "$$";

/* Массив цветовых кодов */
const std::map<std::string, std::string> ColorCodes = {
	{ColorCodePrefix + "_","\u001b[37m"       }, /* Белый (ресет) */
	{ColorCodePrefix + "R","\u001b[31m"       }, /* Тёмн. Красный */
	{ColorCodePrefix + "G","\u001b[38;5;10m"  }, /* Зелёный       */
	{ColorCodePrefix + "B","\u001b[38;5;4m"   }, /* Синий         */
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
	"W I",
	"AIM",
	"C W",
	"OWS",
	"R E",
	"F F",
	"RDR",
	"BDG",
	"GDB",
	"YDY"
};

/* Консоль */
HANDLE Console;

/* Папка с логами */
std::string LogsPath;

/* Текущий лог */
std::string CurrentLogName;

/* Позиция текущего лога */
std::string CurrentLogPath;

/* Лог файл */
std::ofstream LogFile;

/* Лог файл удалён или сломан? */
bool LogFileCorrupted = false;

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

/* Создать название лог файлу */
std::string GenerateLogFileName() {
	std::string Result;

	auto now_forlocal = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	std::tm l;
	localtime_s(&l, &now_forlocal);

	int seconds = l.tm_sec;
	int minutes = l.tm_min;
	int hours   = l.tm_hour;
	int days    = l.tm_mday;
	int month   = l.tm_mon+1;
	int year    = l.tm_year+1900;

	Result = std::to_string(year)                         + "-" +
		FillString(std::to_string(month),   '0', 2, true) + "-" +
		FillString(std::to_string(days),    '0', 2, true) + "-" +
		FillString(std::to_string(hours),   '0', 2, true) + "-" +
		FillString(std::to_string(minutes), '0', 2, true) + "-" +
		FillString(std::to_string(seconds), '0', 2, true);

	if (HasFile(LogsPath + "/" + Result + ".log")) {
		Result += " (CALMDOWN!)";
	}

	return Result + ".log";
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
	std::string Result = ReplaceColorCodesToRealColors(GetPrintMessageBaseDecorations(Base, SLT, true) + Message, true);
	if (!LogFileCorrupted) {
		if (HasFile(CurrentLogPath)) {
			AddToFile(LogFile, Result);
		}
		else {
			if (!LogFileCorrupted) {
				LogFileCorrupted = true;
				ErrorFromLog("LOGGER", "The current log file has been corrupted! Possibly deleted! Path: " + CurrentLogPath);
			}
		}
	}
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

/* Отправить ошибку (специально для логов) */
void ErrorFromLog(std::string Base, std::string Message) {
	PrintBase(Base, OnlyConsole, SLT_Error, Message);
}

/* Отправить фатальную ошибку */
void Fatal(std::string Base, std::string Message) {
	PrintBase(Base, Both, SLT_Fatal, Message);
}

/* ==== Работа с консолью ==== */

/* Очистка консоли */
void CloseConsole() {
	LogFile.close();
}

/* Регистрация консоли */
void InstallConsole(std::string GamePath) {
	Console = GetStdHandle(STD_OUTPUT_HANDLE);

	LogsPath = AddFileToPath(GamePath, "Logs");

	CreateFolder(LogsPath);

	CurrentLogName = GenerateLogFileName();
	CurrentLogPath = LogsPath + "/" + CurrentLogName;

	CreateFile_(CurrentLogPath);

	LogFile.open(CurrentLogPath);
}