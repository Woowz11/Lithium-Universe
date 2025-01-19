#pragma once
#include <fstream>
#include <string>

std::string FixPath(std::string Path);
void CreateFolder(std::string Path);
std::string RemoveLastFileInPath(std::string Path);
std::string AddFileToPath(std::string Path, std::string File);
void CreateFile_(std::string Path, std::string FileName, std::string Content);
void CreateFile_(std::string Path, std::string FileName);
void CreateFile_(std::string Path);
bool HasFile(std::string Path);
void AddToFileLongWay(std::string FilePath, std::string AddThat);
void AddToFile(std::ofstream& File, std::string AddThat);
void ClearFileLongWay(std::string FilePath);
void ClearFile(std::ofstream& File);