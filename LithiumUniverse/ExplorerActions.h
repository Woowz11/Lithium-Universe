#pragma once
#include <fstream>
#include <vector>
#include <string>

#include "nlohmann/json.hpp";

std::string FixPath(std::string Path);
void CreateFolder(std::string Path);
std::string RemoveLastFileInPath(std::string Path);
std::string AddFileToPath(std::string Path, std::string File);
void CreateFile_(std::string Path, std::string FileName, std::string Content);
void CreateFile_(std::string Path, std::string FileName);
void CreateFile_(std::string Path);
bool HasFolder(std::string Path);
bool HasFile(std::string Path);
void AddToFileLongWay(std::string FilePath, std::string AddThat);
void AddToFile(std::ofstream& File, std::string AddThat);
void ClearFileLongWay(std::string FilePath);
void ClearFile(std::ofstream& File);
std::vector<std::string> GetFolders(std::string Path);
std::vector<std::string> GetFilesIncludeSubFolders(std::string Path);
std::string GetFolderName(std::string Path);
std::string GetFileName(std::string FilePath);
std::string GetFileType(std::string FilePath);
std::string ReadFile(std::string FilePath);

nlohmann::json ConvertStringToJson(std::string Json);
nlohmann::json ReadJson(std::string FilePath);