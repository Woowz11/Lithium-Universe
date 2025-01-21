#include <vector>

#include "Shader.h";

std::vector<Shader> Shaders = {};

/* Информация о uniform's

Projection    (mat4     ) = Проекция (от камеры)
Position      (mat4     ) = Позиция объекта
LinePosition  (mat4     ) = Позиция начала и конца линии
Random        (float    ) = Случайное дробное число от 0 до 1
Random1       (float    ) = Случайное дробное число от 0 до 1 (у каждого объекта своё)
Random2       (float    ) = Случайное дробное число от 0 до 1 (у каждого объекта своё)
Random3       (float    ) = Случайное дробное число от 0 до 1 (у каждого объекта своё)
ScreenSize    (vec2     ) = Текущий размер экрана
MousePosition (vec2     ) = Позиция мыши в пикселях
Texture       (sampler2D) = Текстура
Time          (float    ) = Прошедшее время с запуска приложения
DeltaTime     (float    ) = Размягчение зависящие от FPS
DebugRender   (bool     ) = Включен отладочный рендер?
Sleeping      (bool     ) = Физическое тело объекта спит?

Информация о location's
[0] PolygonPosition (vec3) = Позиция полигона
[1] TextureUV       (vec2) = Развёртка текстуры

*/