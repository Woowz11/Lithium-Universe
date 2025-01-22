#include <algorithm>
#include <string>
#include <vector>
#include <map>

#include "nlohmann/json.hpp";
using json = nlohmann::json;

#include "ExplorerActions.h";
#include "GlobalRender.h";
#include "GameResource.h";
#include "DebugGetter.h";
#include "GlobalMods.h";
#include "GameData.h";
#include "FontChar.h";
#include "Console.h";
#include "Texture.h";
#include "GameMod.h";
#include "Shader.h";
#include "Font.h";

/* Айди игровых ресурсов */
const std::string BaseID              = "Base";

/* Все ресурсы игры */
std::vector<GameResource> Resources = {};

/* Пути ресурсов */
std::vector<std::string> F_Other    = {};
std::vector<std::string> F_Shaders  = {};
std::vector<std::string> F_Textures = {};
std::vector<std::string> F_Fonts    = {};
std::vector<std::string> F_Sounds   = {};

/* Путь до текстуры ошибки */
std::string ErrorTexturePath = "";
/* Путь до шейдера ошибки */
std::string ErrorShaderPath = "";

/* Получить ресурс по полному пути */
GameResource GetResourceDebug(const std::string FullPath) {
	auto it = std::find_if(Resources.begin(), Resources.end(),
		[&FullPath](const GameResource& GR) { return GR.FullPath == FullPath; });

	if (it != Resources.end()) {
		return Resources[std::distance(Resources.begin(), it)];
	}
	else {
		Error("RES", "Resource not found at path! GetResourceDebug(\"" + FullPath + "\");");
		return GetResourceDebug(ErrorTexturePath);
	}
}

/* Получить ресурс по пути */
GameResource GetResource(const std::string Base, const std::string Path) {
	return GetResourceDebug(GamePath + "/" + (Base == BaseID ? "Resources" : "Mods/" + Base) + "/" + Path);
}

/* Получить айди ресурса */
int GetResourceID(const int ID, const int IfNotFound) {
#ifdef NDEBUG
	return ID;
#else
	if (ID >= 0 && ID < Resources.size()) {
		GameResource GR = Resources[ID];
		if (!GR.Deleted) {
			return ID;
		}
		else {
			Error("RES", "Unable to get resource because it has been deleted! GetResourceID(" + std::to_string(ID) + "," + std::to_string(IfNotFound) + ");");
		}
	}
	else {
		Error("RES", "Unable to get resource because it is not in the array of all resources! GetResourceID(" + std::to_string(ID) + "," + std::to_string(IfNotFound) + ");");
	}
	return IfNotFound;
#endif
}
int GetResourceID(const int ID) {
	return GetResourceID(ID, GetResourceDebug(ErrorTexturePath).ID);
}

/* Получить айди ассета ресурса */
int GetResourceAssetID(const int ID, const int IfNotFound) {
	return Resources[GetResourceID(ID, IfNotFound)].AssetID;
}
int GetResourceAssetID(const int ID) {
	return GetResourceAssetID(ID, GetResourceDebug(ErrorTexturePath).ID);
}

/* Конвертировать путь по типу "Base:Shaders/Default.vert" в полный путь */
std::string ComplexToFullPath(const std::string ComplexPath) {
	std::string CP = ComplexPath;
	size_t pos = CP.find(':');
	if (pos == std::string::npos) {
		Error("RES", "Failed to convert path [" + ComplexPath + "] to full path! ComplexToFullPath(\"" + ComplexPath + "\");");
		return "";
	}
	else {
		std::string Base = CP.substr(0, pos);
		std::string Path = CP.substr(pos + 1);
		return FixPath(GamePath + "/" + (Base == BaseID ? "Resources" : "Mods/" + Base) + "/" + Path);
	}
}

/* Конвертировать полный путь в не полный путь */
std::string FullPathToComplex(const std::string FullPath) {
	std::string EndPath = ReplaceStringToString(FixPath(FullPath),GamePath+"/","");
	std::string Base = GetFirstElementFromPath(EndPath);
	if (Base == "Mods") {
		EndPath = ReplaceStringToString(EndPath, Base + "/", "");
		Base = GetFirstElementFromPath(EndPath);
	}
	EndPath = ReplaceStringToString(EndPath, Base + "/", "");
	if (Base == "Resources") { Base = BaseID; }
	return Base + ":" + EndPath;
}

/* Получить базу по пути */
std::string GetBaseFromPath(const std::string Path) {
	std::string P = Path;
	size_t pos = P.find(':');
	if (pos == std::string::npos) {
		Error("RES", "Failed to get base from path [" + Path + "]! GetBaseFromPath(\"" + Path + "\");");
		return "";
	}
	else {
		return P.substr(0, pos);
	}
}

/* Определить ресурс по типу */
void DefineResource(const std::string Path) {
	std::string type = GetFileType(Path);

	if (type == "png") {
		F_Textures.push_back(Path);
	}
	else if (type == "lu_shader") {
		F_Shaders.push_back(Path);
	}
	else if (type == "lu_font") {
		F_Fonts.push_back(Path);
	}
	else {
		F_Other.push_back(Path);
	}
}

/* Создать ресурс или взять существующий */
void CreateNewGameResourceOrSkip(const std::string FullPath_, const GameResourceType Type_, const int ID_, const int AssetID_) {
	auto it = std::find_if(Resources.begin(), Resources.end(),
		[&FullPath_](const GameResource& GR) { return GR.FullPath == FullPath_; });

	if (it != Resources.end()) {
		GameResource& GR = Resources[std::distance(Resources.begin(), it)];
		GR.Deleted = false;
		GR.AssetID = AssetID_;
	}
	else {
		GameResource GR = GameResource(FullPath_, Type_, ID_, AssetID_);
		if (FullPath_ == ErrorTexturePath || FullPath_ == ErrorShaderPath) {
			GR.ErrorResource = true;
		}
		Resources.push_back(GR);
	}
}

/* Получить все ресурсы игры */
void GetAllFilesInGamePath() {
	F_Shaders  = {};
	F_Fonts    = {};
	F_Textures = {};
	F_Sounds   = {};
	F_Other    = {};

	std::string ResourcesPath = GamePath + "/Resources";
	std::string ModsPath      = GamePath + "/Mods";

	std::vector<std::string> R_Resources = GetFilesIncludeSubFolders(ResourcesPath);
	std::vector<std::string> R_Mods      = GetFilesIncludeSubFolders(ModsPath);

	for (std::string r : R_Resources) {
		DefineResource(r);
	}

	for (std::string r : R_Mods) {
		DefineResource(r);
	}
}

void UpdateR_Other() {
	for (const std::string r : F_Other) {
		CreateNewGameResourceOrSkip(r, GR_Other, Resources.size(), -1);
	}
	Print("RES", "Other loaded!");
}

void RemoveR_Shaders() {
	for (Shader S : Shaders) {
		S.DeleteShader();
	}
	Shaders = {};
}

void UpdateR_Shaders() {
	ErrorShaderPath = GamePath + "/Resources/Shaders/Error.lu_shader";
	std::string Target = ErrorShaderPath;
	std::sort(F_Shaders.begin(), F_Shaders.end(), [&Target](const std::string& a, const std::string& b) {
		if (a == Target) return true;
		if (b == Target) return false;
		return false;
	});

	int ErrorShaderID = -1;

	for (const std::string r : F_Shaders) {
		int ID = -1;

		std::string Vertex = "";
		std::string Fragment = "";

		json ShaderInfo = ReadJson(r);
		if (ShaderInfo["Error"] == true) {
			Error("SHADER", "Unable to load shader [$$Y" + r + "$$_] because its JSON file contains an error! UpdateR_Shaders();");
		}
		else {
			if (ShaderInfo.contains("Vertex") && ShaderInfo.contains("Fragment")) {
				if (ShaderInfo["Vertex"].is_string() && ShaderInfo["Fragment"].is_string()) {
					std::string VertexPath = ComplexToFullPath(ShaderInfo["Vertex"]);
					std::string FragmentPath = ComplexToFullPath(ShaderInfo["Fragment"]);

					if (VertexPath == "" || FragmentPath == "") {
						Error("SHADER", "Unable to load shader [$$Y" + r + "$$_] because paths to Vertex or Fragment shaders are incorrect! UpdateR_Shaders();");
					}
					else {
						if (HasFile(VertexPath)) {
							Vertex = ReadFile(VertexPath);
						}
						if (HasFile(FragmentPath)) {
							Fragment = ReadFile(FragmentPath);
						}
					}

					if (!(HasFile(VertexPath) && HasFile(FragmentPath))) {
						Error("SHADER", "Unable to load shader [$$Y" + r + "$$_] because Vertex or Fragment shader files were not found! UpdateR_Shaders();");
					}
				}
				else {
					Error("SHADER", "Unable to load shader [$$Y" + r + "$$_] because Vertex or Fragment shader files are not paths! UpdateR_Shaders();");
				}
			}
			else {
				Error("SHADER", "Unable to load shader [$$Y" + r + "$$_] because the Vertex or Fragment field was not found in the file $$Y" + GetFileName(r) + ".lu_shader$$_! UpdateR_Shaders();");
			}
		}

		Shader S = Shader(r, Vertex, Fragment, ErrorShaderID);
		ID = Shaders.size();
		if (ErrorShaderID == -1) {
			ErrorShaderID = S.ID;
		}
		Shaders.push_back(S);

		CreateNewGameResourceOrSkip(r, GR_Shader, Resources.size(), ID);
	}
	ReloadShaderTime();
	Print("RES", "Shaders loaded!");
}

void RemoveR_Textures() {
	for (Texture T : Texturies) {
		DeleteTexture(T);
	}
	Texturies = {};
}

void UpdateR_Textures() {
	ErrorTexturePath = GamePath + "/Resources/Textures/Error/NotFound.png";
	std::string Target = ErrorTexturePath;
	std::sort(F_Textures.begin(), F_Textures.end(), [&Target](const std::string& a, const std::string& b) {
		if (a == Target) return true;
		if (b == Target) return false;
		return false;
	});

	for (const std::string r : F_Textures) {
		Texture T = CreateTexture(r, "Test");
		T.Path = FullPathToComplex(r);
		Texturies.push_back(T);
		CreateNewGameResourceOrSkip(r, GR_Texture, Resources.size(), Texturies.size()-1);
	}
	Print("RES", "Textures loaded!");
}

void RemoveR_Fonts() {
	Fonts = {};
}

void UpdateR_Fonts() {
	for (const std::string r : F_Fonts) {
		json FontInfo = ReadJson(r);
		if (FontInfo["Error"] == true) {
			Error("FONT", "Unable to load font [$$Y" + r + "$$_] because its JSON file contains an error! UpdateR_Fonts();");
		}
		else {
			Font F = CreateFont(r, FontInfo);
			if (F.FullPath != "") {
				Fonts.push_back(F);
				CreateNewGameResourceOrSkip(r, GR_Font, Resources.size(), Fonts.size() - 1);
			}
		}
	}
	Print("RES", "Fonts loaded!");
}

void UpdateR_Sounds() {
	for (const std::string r : F_Sounds) {

	}
	Print("RES", "Sounds loaded!");
}

void UpdateResources() {
	std::string VanillaTexturesFolder = AddFileToPath(AddFileToPath(GamePath, "Resources"), "Textures");
	CreateFolder(VanillaTexturesFolder);

	Print("RES", "Loading resources has started...");

	ClearDebug();

	RemoveR_Fonts();
	RemoveR_Textures();
	RemoveR_Shaders();

	StartDebug();
	GetAllFilesInGamePath();
	for (auto p : Resources) {
		p.Deleted = true;
	}
	CheckMods();
	UpdateR_Other();
	UpdateR_Shaders();
	UpdateR_Textures();
	UpdateR_Fonts();
	UpdateR_Sounds();
	FinishLoadingResources();
	Print("RES", "Resources updated!");
}