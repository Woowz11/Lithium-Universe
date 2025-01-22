#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_RADIANS
#include <GLM/glm.hpp>

#include <vector>
#include <box2d/box2d.h>

#include "GameObject.h";

extern std::vector<GameObject>  Scene;
extern std::vector<b2BodyId> Bodies;

glm::vec2 BVec2ToVec2(b2Vec2 v);
b2Vec2 Vec2ToBVec2(glm::vec2 v);
b2Rot Makeb2Rot(float o);
float MakeOrientation(b2Rot r);
GameObject& GetGameObjectFromBody(b2BodyId b);
b2BodyId GetBody(int i);
GameObject& GetGameObject(int i, std::string message);
bool CheckOutSceneIndex(int i);

int CreateGameObject(std::string Name, RO_Type ObjectType);
void DeleteGameObject(int i, bool IgnoreError);
void DeleteGameObject(int i);

void SetGameObjectDontDelete(int i, bool b);
void SetGameObjectResize(int i, bool b);
void SetGameObjectLayer(int i, float l);
void SetGameObjectText(int i, std::u32string t);
void SetGameObjectFont(int i, int f);
void SetGameObjectShader(int i, int s);
void SetGameObjectTexture(int i, int t);
void SetGameObjectActive(int i, bool b);
void SetGameObjectCollider(int i, ColliderType CT);
void SetGameObjectSelectable(int i, bool b);
void SetGameObjectStatic(int i, bool b);
void SetGameObjectSize(int i, glm::vec2 s);
void SetGameObjectRenderable(int i, bool b);
void SetGameObjectColor(int i, glm::vec4 c);
void SetGameObjectPosition(int i, glm::vec2 p);
void SetGameObjectOrientation(int i, float r);
void SetGameObjectTransform(int i, glm::vec2 p, float r);

glm::vec2 GetGameObjectPosition(int i);
float GetGameObjectOrientation(int i);
glm::vec4 GetGameObjectColor(int i);
glm::vec2 GetGameObjectSize(int i);
int GetGameObjectFromPoint(glm::vec2 PointPos);