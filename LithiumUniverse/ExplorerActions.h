#pragma once
#include <fstream>
#include <vector>
#include <string>

#include "nlohmann/json.hpp";

std::string FixPath(const std::string Path);
void CreateFolder(const std::string Path);
std::string RemoveLastFileInPath(const std::string Path);
std::string AddFileToPath(const std::string Path, const std::string File);
void CreateFile_(const std::string Path, const std::string FileName, const std::string Content);
void CreateFile_(const std::string Path, const std::string FileName);
void CreateFileLongWay(const std::string Path);
bool HasFolder(const std::string Path);
bool HasFile(const std::string Path);
void AddToFileLongWay(const std::string FilePath, const std::string AddThat);
void AddToFile(std::ofstream& File, const std::string AddThat);
void ClearFileLongWay(const std::string FilePath);
void ClearFile(std::ofstream& File);
std::vector<std::string> GetFolders(const std::string Path);
std::vector<std::string> GetFilesIncludeSubFolders(const std::string Path);
std::string GetFolderName(const std::string Path);
std::string GetFileName(const std::string FilePath);
std::string GetFileType(const std::string FilePath);
std::string ReadFile(const std::string FilePath);
std::string GetFirstElementFromPath(const std::string Path);

nlohmann::json ConvertStringToJson(const std::string Json);
nlohmann::json ReadJson(const std::string FilePath);