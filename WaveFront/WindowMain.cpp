#ifndef UNICODE
#define UNICODE
#endif 

#include <windows.h>
#include "CoordSystem.h"
#include "TestGLM.h"



//#define FrameHeight 720
//#define FrameWidth 960
#define FrameHeight 720
#define FrameWidth 1280

RGBQUAD frameBuffer[FrameHeight][FrameWidth];

std::vector<CoordinateStruct> vertexes;
std::vector<int> indexes;

CoordSystem* modelCoordSystem;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void ShowFrame(unsigned int width, unsigned int height, void* pixels, HWND hWnd);
void SetPoint(void* buffer, int x, int y, RGBQUAD color = { 0,0,0,0 });
void SetLine(void* buffer, int x0, int y0, int x1, int y1, RGBQUAD color);
void plotLineWithErr(void* buffer, int x0, int y0, int x1, int y1, RGBQUAD color);
void plotLine(void* buffer, int x0, int y0, int x1, int y1, RGBQUAD color);
void Render();
void ReformatVertexes();
void FrameMove(float timeDelta);




int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{

	ObjLoader* loader = new ObjLoader();
	//CoordSystem* coord = new CoordSystem({ 10,20,20 });
	//CoordSystem* ViewCoord = new CoordSystem({ -3.0f,0.0f,0.0f });
	modelCoordSystem = new CoordSystem({ 0.0f,0.0f,0.0f });

	//std::string res = loader->GetOutput();
	vertexes = loader->GetVetrexVector();
	indexes = loader->GetIndexes();


	////


	modelCoordSystem->SetProjectionTransformationMatrix(45.0f, ((float)FrameWidth / (float)FrameHeight), 10.0f, 100.0f);


	CoordinateStruct cameraGlobalCoord = { 0.05f, 0.8f,-2.0f };
	CoordinateStruct targetGlobalCoord = { 0,1.0f,1 };
	CoordinateStruct cameraUpVect = { 0,-1,0 };

	modelCoordSystem->SetCameraTransformationMatrix(cameraGlobalCoord, targetGlobalCoord, cameraUpVect);

	modelCoordSystem->SetViewPortTransformationMatrix((float)FrameWidth, (float)FrameHeight, 0, 0, 0.0f, 1.0f);

	
	/////

	ReformatVertexes();

	// Register the window class.
	const wchar_t CLASS_NAME[] = L"Sample Window Class";

	WNDCLASS wc = { };

	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;

	RegisterClass(&wc);

	// Create the window.

	HWND hwnd = CreateWindowEx(
		0,                              // Optional window styles.
		CLASS_NAME,                     // Window class
		L"Simple 3d graphics",    // Window text
		WS_OVERLAPPEDWINDOW,            // Window style

		// Size and position
		CW_USEDEFAULT, CW_USEDEFAULT, FrameWidth, FrameHeight,

		NULL,       // Parent window    
		NULL,       // Menu
		hInstance,  // Instance handle
		NULL        // Additional application data
	);

	if (hwnd == NULL)
	{
		return 0;
	}

	ShowWindow(hwnd, nCmdShow);

	// Run the message loop.
	float Start = GetTickCount();


	MSG msg = { };
	while (GetMessage(&msg, NULL, 0, 0) > 0)
	{
		float End = GetTickCount();


		TranslateMessage(&msg);
		DispatchMessage(&msg);

		FrameMove(End - Start);
		Render();
		Start = End;


		Render();
	}



	return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_PAINT:
	{
		ShowFrame(FrameWidth, FrameHeight, frameBuffer, hwnd);
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	return 0;

}

void ShowFrame(unsigned int width, unsigned int height, void* pixels, HWND hWnd)
{
	HBITMAP hBitMap = CreateBitmap(width, height, 1, 8 * 4, pixels);  //CreateCompatibleBitmap  с цветным будет быстрее 
	/*
	  [in] nPlanes
	  Количество цветовых плоскостей, используемых устройством.
	  [in] nBitCount (4 байта)
	  Количество битов, необходимых для идентификации цвета одного пикселя.
	 */

	HDC hdc = GetDC(hWnd);

	// Временный DC для переноса bit-map'а
	HDC srcHdc = CreateCompatibleDC(hdc);

	SelectObject(srcHdc, hBitMap);

	BitBlt(hdc, 0, 0, static_cast<int>(width), static_cast<int>(height), srcHdc, 0, 0, SRCCOPY);
	/*[in] hdc
		Дескриптор контекста целевого устройства.
		[in] x
		Координата X в логических единицах верхнего левого угла прямоугольника назначения.
		[in] y
		Координата Y в логических единицах верхнего левого угла прямоугольника назначения.
		[in] cx
		Ширина(в логических единицах) исходного и целевого прямоугольников.
		[in] cy
		Высота(в логических единицах) исходного и целевого прямоугольников.
		[in] hdcSrc
		Дескриптор контекста исходного устройства.
		[in] x1
		Координата X в логических единицах верхнего левого угла исходного прямоугольника.
		[in] y1
		Координата Y в логических единицах верхнего левого угла исходного прямоугольника.
		[in] rop
		Код растровой операции.Эти коды определяют, как данные цвета исходного прямоугольника должны сочетаться с данными цвета для прямоугольника назначения для достижения окончательного цвета.*/


	DeleteObject(hBitMap);
	DeleteDC(srcHdc);
	ReleaseDC(hWnd, hdc);
}

void SetPoint(void* buffer, int x, int y, RGBQUAD color)
{
	if (x >= 0 && x <= static_cast<int>(FrameWidth - 1) && y >= 0 && y <= static_cast<int>(FrameHeight - 1))
	{
		reinterpret_cast<RGBQUAD*>(buffer)[y * FrameWidth + x] = color;
	}
}

void Render()
{
	//ReformatVertexes();

	for (int i = 0; i < indexes.size(); i += 3)
	{
		plotLine(frameBuffer, vertexes[indexes[i] - 1].x, vertexes[indexes[i] - 1].y, vertexes[indexes[i + 1] - 1].x, vertexes[indexes[i + 1] - 1].y, { 255,255,255,0 });
		plotLine(frameBuffer, vertexes[indexes[i + 1] - 1].x, vertexes[indexes[i + 1] - 1].y, vertexes[indexes[i + 2] - 1].x, vertexes[indexes[i + 2] - 1].y, { 255,255,255,0 });
		plotLine(frameBuffer, vertexes[indexes[i + 2] - 1].x, vertexes[indexes[i + 2] - 1].y, vertexes[indexes[i] - 1].x, vertexes[indexes[i] - 1].y, { 255,255,255,0 });
	}
}

void ReformatVertexes()
{
	HomogeneousCoordinateStruct pointHomogeneous;
	for (int i = 0; i < vertexes.size(); i++)
		//for (int i = 0; i < v2.size(); i++)
	{
		//pointHomogeneous = { v2[i].x, v2[i].y, v2[i].z, 1.0f };
		pointHomogeneous = { vertexes[i].x, vertexes[i].y, vertexes[i].z, 1.0f };

		// row multiply
		pointHomogeneous *= *modelCoordSystem->GlobalTransformationMatrix;
		pointHomogeneous *= *modelCoordSystem->CameraTransformationMatrix;
		pointHomogeneous *= *modelCoordSystem->ProjectionTransformationMatrix;
		pointHomogeneous *= (1 / pointHomogeneous.w);
		pointHomogeneous *= *modelCoordSystem->ViewPortTransformationMatrix;

		/*pointHomogeneous = modelCoordSystem->ColumnMultMatVect(modelCoordSystem->ProjectionTransformationMatrix,
			modelCoordSystem->ColumnMultMatVect(modelCoordSystem->CameraTransformationMatrix,
				modelCoordSystem->ColumnMultMatVect(modelCoordSystem->GlobalTransformationMatrix, pointHomogeneous)));

		pointHomogeneous *= (1 / pointHomogeneous.w);
		pointHomogeneous = modelCoordSystem->ColumnMultMatVect(modelCoordSystem->ViewPortTransformationMatrix, pointHomogeneous);*/

		vertexes[i].x = pointHomogeneous.x;
		vertexes[i].y = pointHomogeneous.y;
		vertexes[i].z = pointHomogeneous.z;

	}
}

void FrameMove(float timeDelta)
{
	static float angle = 0.5f;
	modelCoordSystem->SetRotationMatrixOptimized(angle, { 0,1,0 });

	//modelCoordSystem->GlobalTransformationMatrix = modelCoordSystem->RotationMatrix * modelCoordSystem->GlobalTransformationMatrix;

	CoordinateStruct angleAxis = { 0,1,0 };

	float scaleX = 0.0f;
	float scaleY = 0.0f;
	float scaleZ = 0.0f;

	CoordinateStruct scaling = { scaleX,scaleY,scaleZ };

	float translationX = 0.0f;
	float translationY = 0.0f;
	float translationZ = 0.0f;

	CoordinateStruct translation = { translationX,translationY,translationZ };



	//modelCoordSystem->GlobalTransformationMatrix = modelCoordSystem->TransformMatrix(modelCoordSystem->GlobalTransformationMatrix, scaling, angle, angleAxis, translation);

	angle += 0.5f;
}

void SetLine(void* buffer, int x0, int y0, int x1, int y1, RGBQUAD color)
{
	int deltaX = x1 - x0;
	int deltaY = y1 - y0;
	int absDeltaX = std::abs(deltaX);
	int absDeltaY = std::abs(deltaY);

	int accretion = 0;

	if (absDeltaX >= absDeltaY) // angle >45 ? 
	{
		int y = y0;
		int direction = deltaY != 0 ? (deltaY > 0 ? 1 : -1) : 0; //up or down
		for (int x = x0; deltaX > 0 ? x <= x1 : x >= x1; deltaX > 0 ? x++ : x--) //left or right
		{
			SetPoint(buffer, x, y, color);

			accretion += absDeltaY;

			if (accretion >= absDeltaX)
			{
				accretion -= absDeltaX;
				y += direction;
			}
		}
	}
	else
	{
		int x = x0;
		int direction = deltaX != 0 ? (deltaX > 0 ? 1 : -1) : 0;
		for (int y = y0; deltaY > 0 ? y <= y1 : y >= y1; deltaY > 0 ? y++ : y--)
		{
			SetPoint(buffer, x, y, color);

			accretion += absDeltaX;

			if (accretion >= absDeltaY)
			{
				accretion -= absDeltaY;
				x += direction;
			}
		}
	}
}

void plotLineLow(void* buffer, int x0, int y0, int x1, int y1, RGBQUAD color)
{
	int dx = x1 - x0;
	int dy = y1 - y0;
	int yi = 1;

	if (dy < 0) {
		yi = -1;
		dy = -dy;
	}

	int D = (2 * dy) - dx;
	int y = y0;

	for (int x = x0; x <= x1; x++) {
		SetPoint(buffer, x, y, color);

		if (D > 0) {
			y += yi;
			D += (2 * (dy - dx));
		}
		else {
			D += (2 * dy);
		}
	}
}

void plotLineHigh(void* buffer, int x0, int y0, int x1, int y1, RGBQUAD color)
{
	int dx = x1 - x0;
	int dy = y1 - y0;
	int xi = 1;

	if (dx < 0) {
		xi = -1;
		dx = -dx;
	}

	int D = (2 * dx) - dy;
	int x = x0;

	for (int y = y0; y <= y1; y++) {
		SetPoint(buffer, x, y, color);

		if (D > 0) {
			x += xi;
			D += (2 * (dx - dy));
		}
		else {
			D += (2 * dx);
		}
	}
}

void plotLine(void* buffer, int x0, int y0, int x1, int y1, RGBQUAD color)
{
	if (std::abs(y1 - y0) < std::abs(x1 - x0)) {
		if (x0 > x1) {
			plotLineLow(buffer, x1, y1, x0, y0, color);
		}
		else {
			plotLineLow(buffer, x0, y0, x1, y1, color);
		}
	}
	else {
		if (y0 > y1) {
			plotLineHigh(buffer, x1, y1, x0, y0, color);
		}
		else {
			plotLineHigh(buffer, x0, y0, x1, y1, color);
		}
	}
}

void plotLineWithErr(void* buffer, int x0, int y0, int x1, int y1, RGBQUAD color)
{
	int dx = std::abs(x1 - x0);
	int sx = x0 < x1 ? 1 : -1;
	int dy = -std::abs(y1 - y0);
	int sy = y0 < y1 ? 1 : -1;
	int error = dx + dy;

	while (true) {
		SetPoint(buffer, x0, y0, color);

		if (x0 == x1 && y0 == y1)
			break;

		int errorx2 = 2 * error;

		if (errorx2 >= dy) {
			if (x0 == x1)
				break;

			error += dy;
			x0 += sx;
		}

		if (errorx2 <= dx) {
			if (y0 == y1)
				break;

			error += dx;
			y0 += sy;
		}
	}
}