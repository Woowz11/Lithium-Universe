#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_RADIANS
#include <GLM/glm.hpp>

#include <vector>
#include <box2d/box2d.h>

#include "GameObject.h";

extern std::vector<GameObject>  Scene;
extern std::vector<b2BodyId> Bodies;

glm::vec2 BVec2ToVec2(const b2Vec2 v);
b2Vec2 Vec2ToBVec2(const glm::vec2 v);
b2Rot Makeb2Rot(const float o);
float MakeOrientation(const b2Rot r);
GameObject& GetGameObjectFromBody(const b2BodyId b);
b2BodyId GetBody(const int i);
GameObject& GetGameObject(const int i, const std::string message);
bool CheckOutSceneIndex(const int i);

int CreateGameObject(const std::string Name, const RO_Type ObjectType, bool Modded);
int CreateGameObject(const std::string Name, const RO_Type ObjectType);
int CloneGameObject(const int i);
void DeleteGameObject(const int i, bool IgnoreError);
void DeleteGameObject(const int i);

void SetGameObjectDontDelete(const int i, const bool b);
void SetGameObjectResize(const int i, const bool b);
void SetGameObjectLayer(const int i, const float l);
void SetGameObjectText(const int i, const std::string t);
void SetGameObjectFont(const int i, const int f);
void SetGameObjectShader(const int i, const int s);
void SetGameObjectTexture(const int i, const int t);
void SetGameObjectActive(const int i, const bool b);
void SetGameObjectCollider(const int i, const ColliderType CT);
void SetGameObjectSelectable(const int i, const bool b);
void SetGameObjectStatic(const int i, const bool b);
void SetGameObjectSize(const int i, const glm::vec2 s);
void SetGameObjectRenderable(const int i, const bool b);
void SetGameObjectColor(const int i, const glm::vec4 c);
void SetGameObjectPosition(const int i, const glm::vec2 p);
void SetGameObjectOrientation(const int i, const float r);
void SetGameObjectTransform(const int i, glm::vec2 p, const float r);

glm::vec2 GetGameObjectPosition(const int i);
float GetGameObjectOrientation(const int i);
glm::vec4 GetGameObjectColor(const int i);
glm::vec2 GetGameObjectSize(const int i);
int GetGameObjectFromPoint(const glm::vec2 PointPos);