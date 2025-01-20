#include <filesystem>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "nlohmann/json.hpp";
using json = nlohmann::json;

#include "StringActions.h";
#include "Console.h";

/* Скрипт позволяет управлять проводником (создавать папки, файлы, читать и записывать) */

/* Пути указываются чертой / а не \ */

const std::string Base_EA = "ExplorerActions";

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
		Warn(Base_EA,"Cannot remove last element from path because it was not found! RemoveLastFileInPath(\""+Path+"\");");
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
	std::ofstream file(FixPath(Path), std::ios::app);
	file.close();
}

/* Файл существует по пути? */
bool HasFile(std::string Path) {
	std::ifstream f((FixPath(Path)).c_str());
	return f.good();
}

/* Папка существует по пути? */
bool HasFolder(std::string Path) {
	return std::filesystem::exists(FixPath(Path)) && std::filesystem::is_directory(FixPath(Path));
}

/* Добавить текст в файл (длинным путём) */
void AddToFileLongWay(std::string FilePath, std::string AddThat) {
	if (HasFile(FilePath)) {
		std::ofstream File(FixPath(FilePath), std::ios::app);
		File << AddThat;
		File.close();
	}
	else {
		ErrorFromLog(Base_EA, "It is not possible to add data to the file because file does not exist! AddToFileLongWay(\"" + FilePath + "\",\"" + AddThat + "\");");
	}
}

/* Добавить текст в файл */
void AddToFile(std::ofstream& File, std::string AddThat) {
	if (File.is_open()) {
		File << AddThat;
	}
	else {
		ErrorFromLog(Base_EA, "It is not possible to add data to the file because it is not open! AddToFile(?,\"" + AddThat + "\");");
	}
}

/* Очистить файл (длинным путём) */
void ClearFileLongWay(std::string FilePath) {
	if (HasFile(FilePath)) {
		std::ofstream File(FixPath(FilePath));
		File.write("", 0);
		File.close();
	}
	else {
		ErrorFromLog(Base_EA, "The file cannot be cleared because file does not exist! ClearFileLongWay(\"" + FilePath + "\");");
	}
}

/* Очистить файл */
void ClearFile(std::ofstream& File) {
	if (File.is_open()) {
		File.write("",0);
	}
	else {
		ErrorFromLog(Base_EA, "The file cannot be cleared because it is not open! ClearFile(?);");
	}
}

/* Получить массив папок внутри папки */
std::vector<std::string> GetFolders(std::string Path) {
	std::vector<std::string> Result = {};
	if (HasFolder(Path)) {
		for (const auto& e : std::filesystem::directory_iterator(Path)) {
			if (std::filesystem::is_directory(e)) {
				Result.push_back(FixPath(e.path().string()));
			}
		}
	}
	else {
		ErrorFromLog(Base_EA, "Unable to get folders because main folder does not exist! GetFolders(\"" + Path + "\");");
	}
	return Result;
}

/* Получить массив файлов внутри папки и подпапок */
std::vector<std::string> GetFilesIncludeSubFolders(std::string Path) {
	std::vector<std::string> Result = {};
	if (HasFolder(Path)) {
		for (const auto& e : std::filesystem::recursive_directory_iterator(Path)) {
			if (std::filesystem::is_regular_file(e)) {
				Result.push_back(FixPath(e.path().string()));
			}
		}
	}
	else {
		ErrorFromLog(Base_EA, "Unable to get files because main folder does not exist! GetFilesIncludeSubFolders(\"" + Path + "\");");
	}
	return Result;
}

/* Получить расширение файла */
std::string GetFileType(std::string FilePath) {
	if (HasFile(FilePath)) {
		std::filesystem::path path(FilePath);
		std::string extension = path.extension().string();
		if (extension == "" || extension == ".") { return ""; }
		return RemoveFirstSymbol(extension);
	}
	else {
		ErrorFromLog(Base_EA, "Unable to determine file type because file does not exist! GetFileType(\"" + FilePath + "\");");
	}
	return "";
}

/* Прочесть файл */
std::string ReadFile(std::string FilePath) {
	if (HasFile(FilePath)) {
		std::ifstream file(FilePath);
		std::stringstream buffer;
		buffer << file.rdbuf();
		return buffer.str();
	}
	else {
		Error(Base_EA, "The file cannot be read because it does not exist! ReadFile(\"" + FilePath + "\");");
	}
	return "";
}

/* Получить название файла */
std::string GetFileName(std::string FilePath) {
	if (HasFile(FilePath)) {
		std::filesystem::path path(FilePath);
		return path.stem().string();
	}
	else {
		ErrorFromLog(Base_EA, "Unable to get file name because file does not exist! GetFileName(\"" + FilePath + "\");");
	}
	return "";
}

/* ==== JSON ==== */

json ErrorJson = { {"Error", true} };

/* Конвертировать строку в Json */
json ConvertStringToJson(std::string Json) {
	try {
		return json::parse(Json);
	}
	catch (const json::parse_error& e) {
		Error("JSON", "Failed to convert std::string to JSON! ConvertStringToJson(R\"(\n" + Json + "\n)\");");
		Error("JSON", e.what());
	}
	return ErrorJson;
}

/* Прочитать JSON файл */
json ReadJson(std::string FilePath) {
	if (HasFile(FilePath)) {
		return ConvertStringToJson(ReadFile(FilePath));
	}
	else {
		Error("JSON", "Unable to read JSON file because it does not exist! ReadJson(\"" + FilePath + "\");");
	}
	return ErrorJson;
}