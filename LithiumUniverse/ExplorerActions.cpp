#include <filesystem>
#include <iostream>
#include <fstream>
#include <string>

#include "StringActions.h";
#include "Console.h";

/* Скрипт позволяет управлять проводником (создавать папки, файлы, читать и записывать) */

/* Пути указываются чертой / а не \ */

const std::string Base = "ExplorerActions";

/* Исправить чёрточки в пути */
std::string FixPath(std::string Path) {
	std::string Result = Trim(Path);
	Result = ReplaceCharsToChars(Result, '\\', '/');
	if (GetLastSymbol(Result) == '/') {
		Result = RemoveLastSymbol(Result);
	}
	return Result;
}

/* Удалить последний файл из пути */
std::string RemoveLastFileInPath(std::string Path) {
	size_t found = Path.find_last_of("/\\");
	if (found) {
		std::string Result = Path.substr(0, found);
		return Result;
	}
	else {
		Warn(Base,"Cannot remove last element from path because it was not found! RemoveLastFileInPath(\""+Path+"\");");
		return Path;
	}
}

/* Добавить файл в конец пути */
std::string AddFileToPath(std::string Path, std::string File) {
	return FixPath(Path) + "/" + File;
}

/* Создать папку по указаному пути */
void CreateFolder(std::string Path) {
	namespace fs = std::filesystem;
	fs::create_directories(Path);
}

/* Создать файл */
void CreateFile_(std::string Path, std::string FileName, std::string Content) {
	std::ofstream file(FixPath(Path) + "/" + FileName);
	file << Content;
	file.close();
}
void CreateFile_(std::string Path, std::string FileName) {
	CreateFile_(Path, FileName, "");
}
void CreateFile_(std::string Path) {
	std::ofstream file(FixPath(Path));
	file.close();
}

/* Файл существует по пути? */
bool HasFile(std::string Path) {
	std::ifstream f((FixPath(Path)).c_str());
	return f.good();
}

/* Добавить текст в файл */
void AddToFile(std::ofstream& File, std::string AddThat) {
	if (File.is_open()) {
		File << AddThat;
	}
	else {
		ErrorFromLog(Base,"It is not possible to add data to the file because it is not open! AddToFile(?,\""+AddThat+"\");");
	}
}