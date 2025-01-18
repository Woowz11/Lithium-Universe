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

/* Начало текстового кода */
const std::string CodePrefix = "$$";

/* Проверить текстовые кода */
const bool DebugCodePrefixs = false;

/* Массив текстовых кодов */
const std::map<std::string, WORD> TextCodes = {
	{CodePrefix + "_", FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE                       }, /* Дефолтный       */

	{CodePrefix + "W", FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY}, /* Белый           */
	{CodePrefix + "R", FOREGROUND_RED | FOREGROUND_INTENSITY                                     }, /* Красный         */
	{CodePrefix + "G", FOREGROUND_GREEN | FOREGROUND_INTENSITY                                   }, /* Зелёный         */
	{CodePrefix + "B", FOREGROUND_BLUE | FOREGROUND_INTENSITY                                    }, /* Синий           */
	{CodePrefix + "Y", FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY                  }, /* Жёлтый          */
	{CodePrefix + "P", FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY                   }, /* Фиолетовый      */
	{CodePrefix + "A", FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY                 }, /* Голубой         */
	{CodePrefix + "w", FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE                       }, /* Белый (т.)      */
	{CodePrefix + "r", FOREGROUND_RED                                                            }, /* Красный (т.)    */
	{CodePrefix + "g", FOREGROUND_GREEN                                                          }, /* Зелёный (т.)    */
	{CodePrefix + "b", FOREGROUND_BLUE                                                           }, /* Синий (т.)      */
	{CodePrefix + "y", FOREGROUND_RED | FOREGROUND_GREEN                                         }, /* Жёлтый (т.)     */
	{CodePrefix + "p", FOREGROUND_RED | FOREGROUND_BLUE                                          }, /* Фиолетовый (т.) */
	{CodePrefix + "a", FOREGROUND_GREEN | FOREGROUND_BLUE                                        }, /* Голубой (т.)    */
};

/* Приставки к типу сообщения (связано с SendLogType) */
const std::vector<std::string> LogTypePrefixes = {
	"_ I",
	"AIM",
	"y W",
	"YWS",
	"R E",
	"r F",
	"W D",
	"rDR",
	"GDG",
	"BDB",
	"yDY"
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

/* Удалить коды из текста */
std::string RemoveTextCodes(std::string Message) {
	std::string Result = Message;
	for (const auto& color : TextCodes) {
		size_t pos = 0;
		while ((pos = Result.find(color.first, pos)) != std::string::npos) {
			Result.replace(pos, color.first.length(), "");
		}
	}
	return Result;
}

/* Разделить строку на коды и текст */
std::vector<std::string> ExtractCodesAndText(const std::string& input) {
	std::vector<std::string> result;
	std::string currentText;
	bool inCode = false;
	std::string currentCode;

	for (size_t i = 0; i < input.size(); ++i) {
		char ch = input[i];

		if (ch == '$' && i + 1 < input.size() && input[i + 1] == '$') {
			if (!currentText.empty()) {
				result.push_back(currentText);
				currentText.clear();
			}

			inCode = true;
			currentCode = CodePrefix;
			i++;
		}
		else if (inCode) {
			currentCode += ch;
			result.push_back(currentCode);
			inCode = false;
		}
		else {
			currentText += ch;
		}
	}

	if (!currentText.empty()) {
		result.push_back(currentText);
	}

	return result;
}


/* Отправить сообщение в консоль с кодами */
void CoutWithCodes(std::string Message) {
	std::vector<std::string> Extracted = ExtractCodesAndText(Message);
	for (const auto& p : Extracted) {
		if (TextCodes.find(p) != TextCodes.end()) {
			#ifdef NDEBUG
			#else
			if (DebugCodePrefixs) {
				std::cout << "{" << p << "}";
			}
			#endif
			SetConsoleTextAttribute(Console, TextCodes.at(p));
		}
		else {
			std::cout << p;
		}
	}
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

	Result = 
		std::to_string(year)                              + "-" +
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

	Result += CodePrefix + LogTypePrefix + CodePrefix + "_:";

	/* Время в сообщении */
	auto now = std::chrono::system_clock::now().time_since_epoch();
	auto now_forlocal = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	std::tm l;
	localtime_s(&l, &now_forlocal);

	int milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now).count() % 1000;
	int seconds      = l.tm_sec;
	int minutes      = l.tm_min;
	int hours        = l.tm_hour;

	std::string TimePart = 
		FillString(std::to_string(hours       ), ' ', 2, true) + ":" + 
		FillString(std::to_string(minutes     ), '0', 2, true) + ":" + 
		FillString(std::to_string(seconds     ), '0', 2, true) + ":" + 
		FillString(std::to_string(milliseconds), '0', 3, true);

	Result += "[" + CodePrefix + FirstChar + TimePart + CodePrefix + "_]";

	Result += "[" + CodePrefix + FirstChar + FillString(Base, ' ', 7, false) + CodePrefix + "_]";

	return Result + ": ";
}

/* Отправить сообщение в логи (основа) */
void PrintLogBase(std::string Base, SendLogType SLT, std::string Message) {
	if (!LogFileCorrupted) {
		if (HasFile(CurrentLogPath)) {
			std::string Decor = GetPrintMessageBaseDecorations(Base, SLT, true);
			std::string Result = RemoveTextCodes(Decor + ReplaceCharsToString(Message, '\n', Decor));
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
	std::string Decor = GetPrintMessageBaseDecorations(Base, SLT, false);
	std::string Result = Decor + ReplaceCharsToString(Message,'\n',Decor);
	CoutWithCodes(Result);
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

/* Отправить обычное сообщение (кроме логов) */
void PrintFast(std::string Base, std::string Message) {
	PrintBase(Base, OnlyConsole, SLT_Info, Message);
}

/* Отправить очень успрощённое сообщение (очень быстрое) */
void PrintVeryFast(std::string Message) {
	std::cout << Message << std::endl;
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

/* Отправить отладочное сообщение */
void PrintDebug(std::string Base, std::string Message) {
#ifdef NDEBUG
#else
	PrintBase(Base, OnlyConsole, SLT_Debug, Message);
#endif
}

/* Отправить отладочное сообщение (красное) */
void PrintDebugR(std::string Base, std::string Message) {
#ifdef NDEBUG
#else
	PrintBase(Base, OnlyConsole, SLT_DebugRed, Message);
#endif
}

/* Отправить отладочное сообщение (зелёное) */
void PrintDebugG(std::string Base, std::string Message) {
#ifdef NDEBUG
#else
	PrintBase(Base, OnlyConsole, SLT_DebugGreen, Message);
#endif
}

/* Отправить отладочное сообщение (синее) */
void PrintDebugB(std::string Base, std::string Message) {
#ifdef NDEBUG
#else
	PrintBase(Base, OnlyConsole, SLT_DebugBlue, Message);
#endif
}

/* Отправить отладочное сообщение (жёлтое) */
void PrintDebugY(std::string Base, std::string Message) {
#ifdef NDEBUG
#else
	PrintBase(Base, OnlyConsole, SLT_DebugYellow, Message);
#endif
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