#include <vector>

#include "Shader.h";

std::vector<Shader> Shaders = {};

/* Информация о uniform's

ID                (int      ) = Айди объекта
Texture           (sampler2D) = Текстура
TextureSize       (vec2     ) = Размер текстуры
Position          (vec2     ) = Позиция объекта
CameraPosition    (vec2     ) = Позиция камеры
CameraOrientation (float    ) = Поворот камеры
CameraZoom        (float    ) = Зум камеры
Random            (float    ) = Случайное дробное число от 0 до 1
LocalRandom       (float    ) = Случайное дробное число от 0 до 1 (у каждого объекта свой)
ScreenSize        (vec2     ) = Текущий размер экрана
ScreenStartSize   (vec2     ) = Размер экрана при запуске игры
MousePosition     (vec2     ) = Позиция мыши в пикселях
Time              (float    ) = Прошедшее время с запуска приложения
DeltaTime         (float    ) = Размягчение зависящие от FPS
DebugRender       (bool     ) = Включен отладочный рендер?
Static            (bool     ) = Объект статичный?
Physical          (bool     ) = Объект физичный?
Interface         (bool     ) = Объект является интерфейсом?
Resize            (bool     ) = Объект меняет размер от размера экрана
Sleeping          (bool     ) = Физическое тело объекта спит?

Информация о location's
[0] PointPosition (vec3) = Позиция точки
[1] TextureUV     (vec2) = Развёртка текстуры в точке
[2] PointID       (int ) = Айди точки

*/