#include "TestGLM.h"

void doMagic(glm::vec4 inp)
{
	

	// ������� �������� : 45� ���� ������. 4:3 �����������, ��������� ���� : 0.1 ������ <-> 100 ������
	glm::mat4 Projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
	// ������� ������
	glm::mat4 View = glm::lookAt(
		glm::vec3(4, 3, 3), // ������� �  (4,3,3)������� ���������
		glm::vec3(0, 0, 0), // � ������� � ����� ������
		glm::vec3(0, 1, 0)  // ���� ������ ������� �����
	);
	// ������� ������ � ��������� �������. ������ ��������� � ������ ������� ���������
	glm::mat4 Model = glm::mat4(1.0f);  // ���������� ���� �������� ��� ������ ������!
	// ���� ��� : �������� ��� ���� ��� �������
	glm::mat4 MVP = Projection * View * Model;

	glm::vec4 res = MVP * inp;

	// ������� ������� ��������
	//glViewport(0, 0, screenWidth, screenHeight);
	

	// ��������� ������� �������� � MVP-�������
	//glm::mat4 MVP_with_viewport = viewportMatrix * MVP;
}
