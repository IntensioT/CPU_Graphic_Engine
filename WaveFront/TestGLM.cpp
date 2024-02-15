#include "TestGLM.h"

void doMagic(glm::vec4 inp)
{
	

	// Матрица проекции : 45° Угол обзора. 4:3 соотношение, дальность вида : 0.1 единиц <-> 100 единиц
	glm::mat4 Projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
	// Матрица камеры
	glm::mat4 View = glm::lookAt(
		glm::vec3(4, 3, 3), // Позиция в  (4,3,3)мировых координат
		glm::vec3(0, 0, 0), // И смотрит в центр экрана
		glm::vec3(0, 1, 0)  // Верх камеры смотрит вверх
	);
	// Матрица модели – единичная матрица. Модель находится в центре мировых координат
	glm::mat4 Model = glm::mat4(1.0f);  // Выставляем свое значение для каждой модели!
	// НАША МВП : Умножаем все наши три матрицы
	glm::mat4 MVP = Projection * View * Model;

	glm::vec4 res = MVP * inp;

	// Создаем матрицу вьюпорта
	//glViewport(0, 0, screenWidth, screenHeight);
	

	// Применяем матрицу вьюпорта к MVP-матрице
	//glm::mat4 MVP_with_viewport = viewportMatrix * MVP;
}
