#ifndef UNICODE
#define UNICODE
#endif 

#include "WindowMain.h"
//#include "PolygonsLogic.h"


int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	ObjLoader* loader = new ObjLoader();

	modelCoordSystem = new CoordSystem({ 0.0f,0.0f,0.0f });

	vertexes = loader->GetVetrexVector();
	indexes = loader->GetIndexes();

	normalIndexes = loader->GetNormalIndexes();
	normals = loader->GetNormals();

	vertexesOutp.resize(vertexes.size() + normals.size());
	std::vector<HomogeneousCoordinateStruct> vertexesHomo;

	vertexesHomo.resize(vertexes.size());

	for (int i = 0; i < vertexes.size(); i++)
	{
		vertexesHomo[i] = { vertexes[i].x, vertexes[i].y, vertexes[i].z, 1 };
	}

	//polygons.resize(indexes.size() / 3);

	for (int i = 0; i < indexes.size(); i += 3)
	{
		vertexesHomo[indexes[i] - 1].normal = normals[normalIndexes[i] - 1];
		vertexesHomo[indexes[i + 1] - 1].normal = normals[normalIndexes[i + 1] - 1];
		vertexesHomo[indexes[i + 2] - 1].normal = normals[normalIndexes[i + 2] - 1];
		polygons.push_back({ vertexesHomo[indexes[i] - 1], vertexesHomo[indexes[i + 1] - 1], vertexesHomo[indexes[i + 2] - 1] });
	}
	//polygons = GetAllPolygons(vertexesHomo, indexes, normalIndexes, normals);
	polygonsOutp.resize(polygons.size());

	/////

	rasterizator = new Rasterizator();


	UpdateNormals();
	/////

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

	MSG msg = { };
	while (GetMessage(&msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);

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

	case WM_SIZE:
		UpdateWindowSize(hwnd);
		InvalidateRect(hwnd, NULL, TRUE);
		return 0;

	case WM_PAINT:
	{
		// Update the window title with the frame count
		std::string title = "3D Engine - FPS: " + std::to_string(fps);
		SetWindowTextA(hwnd, title.c_str());

		Render();
		ShowFrame(FrameWidth, FrameHeight, frameBuffer, hwnd);
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	case WM_LBUTTONDOWN:
	{
		// Сохраняем текущие координаты курсора как предыдущие
		prevMouseX = LOWORD(lParam);
		prevMouseY = HIWORD(lParam);

		mouseDown = true;
		return 0;
	}

	case WM_LBUTTONUP:
	{
		// Сбрасываем флаг, что кнопка мыши нажата
		mouseDown = false;
		return 0;
	}

	case WM_MOUSEMOVE:
	{
		if (mouseDown)
		{
			int currMouseX = LOWORD(lParam);
			int currMouseY = HIWORD(lParam);

			int deltaX = currMouseX - prevMouseX;
			int deltaY = currMouseY - prevMouseY;

			phiAngleSphere -= deltaX * 0.1f;
			thetaAngleSphere -= deltaY * 0.1f;

			thetaAngleSphere = (thetaAngleSphere > 175.f) ? 175.f : ((thetaAngleSphere < 5.f) ? 5.f : thetaAngleSphere);

			prevMouseX = currMouseX;
			prevMouseY = currMouseY;

			InvalidateRect(hwnd, NULL, TRUE);
		}
		return 0;
	}

	case WM_MOUSEWHEEL:
	{
		int wheelDelta = GET_WHEEL_DELTA_WPARAM(wParam);

		if (wheelDelta > 0)
		{
			zCamera += 0.5f;
			rSphere += 0.5f;
		}
		else if (wheelDelta < 0)
		{
			if (zCamera > 1.f)
			{
				zCamera -= 0.5f;
				rSphere -= 0.5f;
			}
		}

		// Перерисовать окно
		InvalidateRect(hwnd, NULL, TRUE);
		return 0;
	}


	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_LEFT:
			// Process the LEFT ARROW key. 
			yAngleObject -= 5.0f;
			InvalidateRect(hwnd, NULL, TRUE);
			return 0;

		case VK_RIGHT:
			// Process the RIGHT ARROW key. 
			yAngleObject += 5.0f;
			InvalidateRect(hwnd, NULL, TRUE);
			return 0;

		case VK_UP:
			// Process the UP ARROW key. 
			xAngleObject += 5.0f;
			InvalidateRect(hwnd, NULL, TRUE);

			return 0;

		case VK_DOWN:
			// Process the DOWN ARROW key. 
			xAngleObject -= 5.0f;
			InvalidateRect(hwnd, NULL, TRUE);
			return 0;

		case 'W':
			zCamera += 0.05f;
			rSphere += 0.05f;
			InvalidateRect(hwnd, NULL, TRUE);
			return 0;

		case 'S':
			if (zCamera > 1.f)
			{
				zCamera -= 0.05f;
				rSphere -= 0.05f;
				InvalidateRect(hwnd, NULL, TRUE);
			}
			return 0;

		case 'Z':

			phiAngleSphere += 5.f;
			InvalidateRect(hwnd, NULL, TRUE);
			return 0;

		case 'X':

			phiAngleSphere -= 5.f;
			InvalidateRect(hwnd, NULL, TRUE);

			return 0;

		case 'C':
			if (thetaAngleSphere < 175.f)
			{
				thetaAngleSphere += 5.f;
				InvalidateRect(hwnd, NULL, TRUE);
			}
			return 0;

		case 'V':
			if (thetaAngleSphere > 5.f)
			{
				thetaAngleSphere -= 5.f;
				InvalidateRect(hwnd, NULL, TRUE);
			}
			return 0;
		case '1':
			curGraphic = 1;
			return 0;
		case '2':
			curGraphic = 2;
			return 0;
		case '3':
			curGraphic = 3;
			return 0;
		case '4':
			curGraphic = 4;
			return 0;
		case '5':
			curGraphic = 5;
			return 0;
		case '6':
			curGraphic = 6;
			return 0;
		case '7':
			curGraphic = 7;
			return 0;
		case '8':
			curGraphic = 8;
			return 0;
		case '9':
			curGraphic = 9;
			return 0;
		case '0':
			curGraphic = 0;
			return 0;
		}



	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	return 0;

}

void ShowFrame(unsigned int width, unsigned int height, void* pixels, HWND hWnd)
{
	HDC hdc = GetDC(hWnd);

	//HBITMAP hBitMap = CreateBitmap(width, height, 1, 8 * 4, pixels);
	////HBITMAP hBitMap = CreateCompatibleBitmap(hdc, width, height);
	///*
	////Однако для повышения производительности приложения должны использовать CreateBitmap для создания монохромных растровых изображений и
	////CreateCompatibleBitmap для создания цветных растровых изображений
	////  [in] nPlanes
	////Количество цветовых плоскостей, используемых устройством.
	////  [in] nBitCount (4)
	////Количество битов, необходимых для идентификации цвета одного пикселя.
	//// */

	// // DC  bit-map
	//HDC srcHdc = CreateCompatibleDC(hdc);

	//SelectObject(srcHdc, hBitMap);

	//BitBlt(hdc, 0, 0, static_cast<int>(width), static_cast<int>(height), srcHdc, 0, 0, SRCCOPY);

	///////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	// Создаем битмап, совместимый с устройством
	HBITMAP hBitMap = CreateCompatibleBitmap(hdc, width, height);

	// Получаем доступ к битам битмапа
	BITMAPINFO bitmapInfo;
	ZeroMemory(&bitmapInfo, sizeof(bitmapInfo));
	bitmapInfo.bmiHeader.biSize = sizeof(bitmapInfo.bmiHeader);
	bitmapInfo.bmiHeader.biWidth = width;
	//bitmapInfo.bmiHeader.biHeight = height; 
	bitmapInfo.bmiHeader.biHeight = (height == 0) ? 0 : -static_cast<LONG>(height); // Отрицательная высота означает, что битмап будет перевернут
	bitmapInfo.bmiHeader.biPlanes = 1;
	bitmapInfo.bmiHeader.biBitCount = 32; // 32 бита на пиксель - 8 бит на канал RGBA
	bitmapInfo.bmiHeader.biCompression = BI_RGB;

	void* bits = nullptr;
	HDC srcHdc = CreateCompatibleDC(hdc);
	SelectObject(srcHdc, hBitMap);
	SetDIBits(srcHdc, hBitMap, 0, height, pixels, &bitmapInfo, DIB_RGB_COLORS);

	// Копируем битмап в целевое устройство
	BitBlt(hdc, 0, 0, width, height, srcHdc, 0, 0, SRCCOPY);
	/*[in] hdc
		Дескриптор контекста целевого устройства.
		[in] x
		Координата X в логических единицах левого верхнего угла целевого прямоугольника.
		[in] y
		Координата Y в логических единицах левого верхнего угла целевого прямоугольника.
		[in] cx
		Ширина (в логических единицах) исходного и целевого прямоугольников.
		[in] cy
		Высота (в логических единицах) исходного и целевого прямоугольников.
		[in] hdcSrc
		Дескриптор контекста исходного устройства.
		[in] x1
		Координата X в логических единицах верхнего левого угла исходного прямоугольника.
		[in] y1
		Координата Y в логических единицах верхнего левого угла исходного прямоугольника.
		[in] rop
		Код растровой операции. Эти коды определяют, как данные цвета исходного прямоугольника должны сочетаться с данными цвета для целевого прямоугольника для достижения окончательного цвета.
		*/
		///////////////////////////////////////////////////////////////////////////////////////////////////////

		// Освобождаем ресурсы
	DeleteObject(hBitMap);
	DeleteDC(srcHdc);
	ReleaseDC(hWnd, hdc);

}

void SetPoint(void* buffer, int x, int y, RGBQUAD color)
{
	if (x >= 0 && x <= (FrameWidth - 1) && y >= 0 && y <= (FrameHeight - 1))
	{
		reinterpret_cast<RGBQUAD*>(buffer)[y * FrameWidth + x] = color;
	}
}

void Render()
{
	///////////////////////////////////////////////////////////
	frameCount++;

	// Measure the elapsed time since the last frame
	float deltaTime = getElapsedTime();
	accumulatedTime += deltaTime;

	// Update the FPS every updateInterval seconds
	if (accumulatedTime >= updateInterval)
	{
		fps = static_cast<int>(frameCount / accumulatedTime);
		frameCount = 0;
		accumulatedTime -= updateInterval;
	}


	//////////////////////////////////////////////////////////

	std::memset(frameBuffer, 0, sizeof(frameBuffer));

	const float MAX_DEPTH = 3.4E+38f;
	const float MIN_DEPTH = -3.4E+38f;
	for (int i = 0; i < 1080; ++i) {
		for (int j = 0; j < 1920; ++j) {
			depthBuffer[i][j] = MAX_DEPTH;
			//depthBuffer[i][j] = MIN_DEPTH;
		}
	}
	/////////////////////////////////////////SET MATRIXES////////////////////////////////////////////////////////////////
	modelCoordSystem->SetRotateYMatrix(GetRadians(yAngleObject));
	modelCoordSystem->SetRotateXMatrix(GetRadians(xAngleObject));
	modelCoordSystem->GlobalTransformationMatrix = modelCoordSystem->RotateYMatrix * modelCoordSystem->RotateXMatrix;

	cameraGlobalCoord = { SphericalToCartesian(rSphere, phiAngleSphere, thetaAngleSphere) };
	CoordinateStruct cameraUpVect = { 0,1,0 };
	modelCoordSystem->SetCameraTransformationMatrix(cameraGlobalCoord, targetGlobalCoord, cameraUpVect);

	modelCoordSystem->SetProjectionTransformationMatrix(45.0f, ((float)FrameWidth / (float)FrameHeight), zNear, zFar);

	modelCoordSystem->SetViewPortTransformationMatrix((float)FrameWidth, (float)FrameHeight, 0, 0, 0.0f, 1.0f);
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//UpdateNormals();
	UpdateVectors();

	////////////////////////////////////TEMP//////////////////////////////////
	Triangle sus1;
	sus1.vectors[0] = { 300, 300, -3, 1 };
	sus1.vectors[0].diffuse = { 255, 0, 0};
	sus1.vectors[1] = { 300, 600, -3, 1 };
	sus1.vectors[1].diffuse = { 0, 255, 0 };
	sus1.vectors[2] = { 600, 400, -3, 1 };
	sus1.vectors[2].diffuse = { 0, 255, 0 };

	Triangle sus2;
	sus2.vectors[0] = { 600, 300, -3, 1 };
	sus2.vectors[0].diffuse = { 255, 255, 255 };
	sus2.vectors[2] = { 600, 600, -3, 1 };
	sus2.vectors[1].diffuse = { 255, 255, 255 };
	sus2.vectors[1] = { 300, 400, -3, 1 };
	sus2.vectors[2].diffuse = { 255, 255, 255 };


	//rasterizator->DrawPolygonBarycentric(sus1, frameBuffer, depthBuffer, color);
	rasterizator->DrawPolygonBarycentric(sus2, frameBuffer, depthBuffer, color);

	////////////////////////////////////////////////////////////////////////////

	/*for (int i = 0; i < polygonsOutp.size(); i++)
	{
		DrawTriangle(polygonsOutp[i]);
	}*/

	/*Concurrency::parallel_for(0, static_cast<int>(polygonsOutp.size()), [&](int i)
		{
			DrawTriangle(polygonsOutp[i]);
		});*/



	//UpdatePolygonsAsync();
}

void UpdateNormals()
{

	HomogeneousCoordinateStruct homoNormal, normalSum, vertexSum;
	for (int i = 0; i < polygons.size(); i++)
	{
		normalSum = { 0,0,0,0 };
		vertexSum = { 0,0,0,0 };
		for (int j = 0; j < (1 / vectorCount); j++)
		{

			homoNormal = { polygons[i].vectors[j].normal.x, polygons[i].vectors[j].normal.y, polygons[i].vectors[j].normal.z, 0 };

			//homoNormal *= modelCoordSystem->GlobalTransformationMatrix;

			polygonsOutp[i].vectors[j].normal = { homoNormal.x, homoNormal.y , homoNormal.z };
			polygonsOutp[i].vectors[j].normalW = homoNormal.w;
			normalSum = normalSum + homoNormal;

			// For center of polygon
			vertexSum = vertexSum + polygons[i].vectors[j];
		}
		vertexSum *= vectorCount;
		normalSum *= vectorCount;
		polygonsOutp[i].polygonNormal = modelCoordSystem->NormalizeVector(normalSum);
		polygonsOutp[i].polygonCenter = vertexSum;

		polygons[i].isOnScreen = true;
		polygonsOutp[i].isOnScreen = true;
	}
}

void UpdateVectors()
{
	//omp_set_num_threads(6);
	// Цикл будет выполняться в g_nNumberOfThreads потоков.
	// Параметры цикла будут автоматически распределены между потоками.
	//#pragma omp parallel for
	for (int i = 0; i < polygons.size(); i++)
	{
		if (!ClipFacePolygons(i)) continue;
		if (!UpdatePolygons(i)) continue;
	}
}
//void UpdateVectors()
//{
//	for (int i = 0; i < polygons.size(); i++)
//	{
//		if (!ClipFacePolygons(i)) continue;
//		if (!UpdatePolygons(i)) continue;
//		//if (!IsObjectBehindClipPlanes(i, clipPlanes))
//		//{
//		//DrawObject(i);
//		//}
//	}
//}



bool IsObjectBehindClipPlanes(int polygonIterator, const std::vector<Plane>& clipPlanes)
{
	Triangle polygon;
	HomogeneousCoordinateStruct pointHomogeneous;

	polygon = polygonsOutp[polygonIterator];

	for (int i = 0; i < 3; i++)
	{
		pointHomogeneous = polygon.vectors[i];
		bool isVertexBehindClipPlanes = true;

		for (const Plane& clipPlane : clipPlanes) {
			if (!modelCoordSystem->IsVertexBehindClipPlane(pointHomogeneous, clipPlane)) {
				isVertexBehindClipPlanes = false;
				break;
			}
		}
		if (!isVertexBehindClipPlanes) {
			return false;
		}
	}
	return true;
}

Triangle CalculateLambertTermAndShade(int polygonIterator, int vectorIterator, CoordinateStruct curVector, Triangle inputPolygon)
{
	Triangle polygon = inputPolygon;

	CoordinateStruct curNormal = polygonsOutp[polygonIterator].vectors[vectorIterator].normal;

	CoordinateStruct lightDirection = modelCoordSystem->NormalizeVector(modelCoordSystem->SubstractVectors(lightGlobalCoord, curVector));

	float lambertTerm = (modelCoordSystem->DotProduct(curNormal, lightDirection) >= 0.0) ? modelCoordSystem->DotProduct(curNormal, lightDirection) : 0.0f;
	polygon.vectors[vectorIterator].shade = lambertTerm;

	polygon.vectors[vectorIterator].diffuse = { DiffuseLightColor.x * lambertTerm, DiffuseLightColor.y * lambertTerm, DiffuseLightColor.z * lambertTerm };

	return polygon;
}

void DrawObject(int i)
{
	if (polygonsOutp[i].isOnScreen == false) return;
	//vectorCount = 1 / (sizeof(polygonsOutp[i].vectors) / sizeof(polygonsOutp[i].vectors[0]));
	HomogeneousCoordinateStruct homoNormal0, homoNormal1, homoNormal2;

	switch (curGraphic)
	{
	case 1:
		polygonOutputMutex.lock();
		BresenhamLineOptimised(frameBuffer, polygonsOutp[i].vectors[0], polygonsOutp[i].vectors[0 + 1], color2);
		BresenhamLineOptimised(frameBuffer, polygonsOutp[i].vectors[0 + 1], polygonsOutp[i].vectors[0 + 2], color2);
		BresenhamLineOptimised(frameBuffer, polygonsOutp[i].vectors[0 + 2], polygonsOutp[i].vectors[0], color2);
		polygonOutputMutex.unlock();
		break;
	case 2:
		polygonOutputMutex.lock();
		for (int j = 0; j < (1 / vectorCount); j++)
		{
			if (polygonsOutp[i].vectors[j].x > 1920 || polygonsOutp[i].vectors[j].x < 0 || polygonsOutp[i].vectors[j].y > 1080 || polygonsOutp[i].vectors[j].y < 0) return;
		}

		rasterizator->UpdateXleftAndXRight(polygonsOutp[i]);

		rasterizator->DrawLines(polygonsOutp[i], frameBuffer, depthBuffer, color);
		polygonOutputMutex.unlock();
		break;
	case 3:
		polygonOutputMutex.lock();
		BresenhamLineOptimised(frameBuffer, polygonsOutp[i].vectors[0], polygonsOutp[i].vectors[0 + 1], color2);
		BresenhamLineOptimised(frameBuffer, polygonsOutp[i].vectors[0 + 1], polygonsOutp[i].vectors[0 + 2], color2);
		BresenhamLineOptimised(frameBuffer, polygonsOutp[i].vectors[0 + 2], polygonsOutp[i].vectors[0], color2);

		homoNormal0 = { polygonsOutp[i].vectors[0].projectedNormal.x, polygonsOutp[i].vectors[0].projectedNormal.y, polygonsOutp[i].vectors[0].projectedNormal.z, 1 };
		homoNormal1 = { polygonsOutp[i].vectors[1].projectedNormal.x, polygonsOutp[i].vectors[1].projectedNormal.y, polygonsOutp[i].vectors[1].projectedNormal.z, 1 };
		homoNormal2 = { polygonsOutp[i].vectors[2].projectedNormal.x, polygonsOutp[i].vectors[2].projectedNormal.y, polygonsOutp[i].vectors[2].projectedNormal.z, 1 };
		BresenhamLineOptimised(frameBuffer, polygonsOutp[i].vectors[0], homoNormal0, color);
		BresenhamLineOptimised(frameBuffer, polygonsOutp[i].vectors[0 + 1], homoNormal1, color);
		BresenhamLineOptimised(frameBuffer, polygonsOutp[i].vectors[0 + 2], homoNormal2, color);
		polygonOutputMutex.unlock();
		break;
	case 4:


		for (int j = 0; j < (1 / vectorCount); j++)
		{
			if (polygonsOutp[i].vectors[j].x > 1920 || polygonsOutp[i].vectors[j].x < 0 || polygonsOutp[i].vectors[j].y > 1080 || polygonsOutp[i].vectors[j].y < 0) return;
		}

		rasterizator->DrawPolygon(polygonsOutp[i], frameBuffer, depthBuffer, color);

		break;
	}
}

void DrawTriangle(Triangle& triangle)
{
	if (triangle.isOnScreen == false) return;
	int curVectorCount = (1 / vectorCount);
	RectangleStruct bbox;
	HomogeneousCoordinateStruct homoNormal0, homoNormal1, homoNormal2;

	switch (curGraphic)
	{
	case 1:
		polygonOutputMutex.lock();
		BresenhamLineOptimised(frameBuffer, triangle.vectors[0], triangle.vectors[0 + 1], color2);
		BresenhamLineOptimised(frameBuffer, triangle.vectors[0 + 1], triangle.vectors[0 + 2], color2);
		BresenhamLineOptimised(frameBuffer, triangle.vectors[0 + 2], triangle.vectors[0], color2);
		polygonOutputMutex.unlock();
		break;
	case 2:
		bbox = FindTriangleBoundingRectangle2D(triangle);
		if (bbox.left > 1920 || bbox.right < 0 || bbox.top > 1080 || bbox.bottom < 0) return;


		polygonOutputMutex.lock();


		rasterizator->UpdateXleftAndXRight(triangle);

		rasterizator->DrawLines(triangle, frameBuffer, depthBuffer, color);
		polygonOutputMutex.unlock();
		break;
	case 3:
		polygonOutputMutex.lock();
		BresenhamLineOptimised(frameBuffer, triangle.vectors[0], triangle.vectors[0 + 1], color2);
		BresenhamLineOptimised(frameBuffer, triangle.vectors[0 + 1], triangle.vectors[0 + 2], color2);
		BresenhamLineOptimised(frameBuffer, triangle.vectors[0 + 2], triangle.vectors[0], color2);

		homoNormal0 = { triangle.vectors[0].projectedNormal.x, triangle.vectors[0].projectedNormal.y, triangle.vectors[0].projectedNormal.z, 1 };
		homoNormal1 = { triangle.vectors[1].projectedNormal.x, triangle.vectors[1].projectedNormal.y, triangle.vectors[1].projectedNormal.z, 1 };
		homoNormal2 = { triangle.vectors[2].projectedNormal.x, triangle.vectors[2].projectedNormal.y, triangle.vectors[2].projectedNormal.z, 1 };
		BresenhamLineOptimised(frameBuffer, triangle.vectors[0], homoNormal0, color);
		BresenhamLineOptimised(frameBuffer, triangle.vectors[0 + 1], homoNormal1, color);
		BresenhamLineOptimised(frameBuffer, triangle.vectors[0 + 2], homoNormal2, color);
		polygonOutputMutex.unlock();
		break;
	case 4:
		bbox = FindTriangleBoundingRectangle2D(triangle);
		if (bbox.left > 1920 || bbox.right < 0 || bbox.top > 1080 || bbox.bottom < 0) return;


		rasterizator->DrawPolygonBarycentric(triangle, frameBuffer, depthBuffer, color);

		break;

	}
}

bool ClipFacePolygons(int polygonIterator)
{
	HomogeneousCoordinateStruct polygonCenter, polygonNormal;

	polygonCenter = polygonsOutp[polygonIterator].polygonCenter;
	polygonNormal = polygonsOutp[polygonIterator].polygonNormal;

	polygonCenter *= modelCoordSystem->GlobalTransformationMatrix;
	polygonNormal *= modelCoordSystem->GlobalTransformationMatrix;


	CoordinateStruct curPolygonCenter = polygonCenter.toCoordinateStruct();
	CoordinateStruct curCameraPos = { 0,0,0 };
	CoordinateStruct cameraDirectionOnPoint = modelCoordSystem->NormalizeVector(modelCoordSystem->SubstractVectors(cameraGlobalCoord, curPolygonCenter));

	float dotProduct = modelCoordSystem->DotProduct(polygonNormal.toCoordinateStruct(), cameraDirectionOnPoint);
	/*if (dotProduct < 0)
	{
		polygonsOutp[polygonIterator].isOnScreen = false;
		return false;
	}*/
	polygonsOutp[polygonIterator].isOnScreen = true;
	return true;
}



bool UpdatePolygons(int polygonIterator)
{
	sus += 50.0f;
	while (sus >= 255.0f)
	{
		sus -= 255.0f;
	}

	Triangle polygon = polygonsOutp[polygonIterator];
	HomogeneousCoordinateStruct pointHomogeneous, normalHomogeneous;

	for (int i = 0; i < (1 / vectorCount); i++)
	{
		polygonsMutex.lock();
		pointHomogeneous = polygons[polygonIterator].vectors[i];
		polygonsMutex.unlock();

		polygonOutputMutex.lock();
		normalHomogeneous = { polygonsOutp[polygonIterator].vectors[i].normal.x, polygonsOutp[polygonIterator].vectors[i].normal.y, polygonsOutp[polygonIterator].vectors[i].normal.z, 1 };
		polygonOutputMutex.unlock();

		pointHomogeneous *= modelCoordSystem->GlobalTransformationMatrix;
		CoordinateStruct curVector = { pointHomogeneous.x,pointHomogeneous.y,pointHomogeneous.z };
		modelCoordSystem->SetTranslationMatrix(pointHomogeneous.toCoordinateStruct());

		pointHomogeneous *= modelCoordSystem->CameraTransformationMatrix;
		pointHomogeneous *= modelCoordSystem->ProjectionTransformationMatrix;

		if (pointHomogeneous.w < 0.4 && pointHomogeneous.w > -0.4)
		{
			//#pragma omp atomic
			polygonsOutp[polygonIterator].isOnScreen = false;
			return false;
		}

		pointHomogeneous *= (1 / pointHomogeneous.w);
		pointHomogeneous *= modelCoordSystem->ViewPortTransformationMatrix;

		//////////////////////////////Normals Showcase//////////////////////////////////
		if (curGraphic == 3)
		{
			polygonOutputMutex.lock();
			normalHomogeneous = modelCoordSystem->CalculateNormalProjections({ polygonsOutp[polygonIterator].vectors[i].normal.x,
																				polygonsOutp[polygonIterator].vectors[i].normal.y,
																				polygonsOutp[polygonIterator].vectors[i].normal.z, 1 });
			polygonOutputMutex.unlock();
		}
		///////////////////////////////////////////////////////////////////////////////

		polygon.vectors[i] = pointHomogeneous;
		polygon.vectors[i].projectedNormal = { normalHomogeneous.x, normalHomogeneous.y, normalHomogeneous.z };
		///////////////////////////////// LAMBERT///////////////////////////////////////////////////////
		polygon = CalculateLambertTermAndShade(polygonIterator, i, curVector, polygon);
		//polygon.vectors[i].diffuse = { sus, sus+30, sus+10 };
	}
	polygonOutputMutex.lock();
	polygonsOutp[polygonIterator] = polygon;
	polygonOutputMutex.unlock();
	return true;
}


Triangle UpdatePolygonsTriangle(int polygonIterator)
{
	Triangle polygon;
	HomogeneousCoordinateStruct pointHomogeneous;
	for (int i = 0; i < 3; i++)
	{
		pointHomogeneous = polygons[polygonIterator].vectors[i];

		pointHomogeneous *= modelCoordSystem->GlobalTransformationMatrix;
		CoordinateStruct curVector = { pointHomogeneous.x,pointHomogeneous.y,pointHomogeneous.z };
		modelCoordSystem->SetTranslationMatrix(pointHomogeneous.toCoordinateStruct());
		pointHomogeneous *= modelCoordSystem->CameraTransformationMatrix;
		pointHomogeneous *= modelCoordSystem->ProjectionTransformationMatrix;

		if (pointHomogeneous.w < 0.4 && pointHomogeneous.w > -0.4)
		{
			pointHomogeneous = { 0,0,0,1 };
			//return false;
		}

		pointHomogeneous *= (1 / pointHomogeneous.w);
		pointHomogeneous *= modelCoordSystem->ViewPortTransformationMatrix;

		pointHomogeneous.shade = 1;
		polygon.vectors[i] = pointHomogeneous;
		polygon.vectors[i].normal = polygons[polygonIterator].vectors[i].normal;

		///////////////////////////////// LAMBERT///////////////////////////////////////////////////////
		CoordinateStruct normal = modelCoordSystem->NormalizeVector(polygon.vectors[i].normal);
		// получаем вектор направления света
		CoordinateStruct curPos = { polygon.vectors[i].x,polygon.vectors[i].y,polygon.vectors[i].z };
		CoordinateStruct lightDirection = modelCoordSystem->NormalizeVector(modelCoordSystem->SubstractVectors(lightGlobalCoord, curVector));

		// получаем скалярное произведение векторов нормали и направления света
		float lambertTerm = (modelCoordSystem->DotProduct(normal, lightDirection) >= 0.0) ? modelCoordSystem->DotProduct(normal, lightDirection) : 0.0f;
		polygon.vectors[i].shade = lambertTerm;

		polygon.vectors[i].diffuse = { DiffuseLightColor.x * lambertTerm, DiffuseLightColor.y * lambertTerm, DiffuseLightColor.z * lambertTerm };
	}
	return polygon;
}

void UpdateWindowSize(HWND hWnd)
{
	RECT clientRect;
	GetClientRect(hWnd, &clientRect);

	FrameHeight = clientRect.bottom;
	FrameWidth = clientRect.right;
}

void BresenhamLineOptimised(void* buffer, HomogeneousCoordinateStruct vectorA, HomogeneousCoordinateStruct vectorB, RGBQUAD color)
{
	int x1 = (int)(vectorA.x);
	int y1 = (int)(vectorA.y);
	int x2 = (int)(vectorB.x);
	int y2 = (int)(vectorB.y);

	int dx = x2 - x1;
	int dy = y2 - y1;
	int w = std::abs(dx);
	int h = std::abs(dy);
	int l = max(w, h);

	int sign = (dx >= 0) ? 1 : -1;
	int mat11 = sign;
	int mat12 = 0;
	int mat21 = 0;
	sign = (dy >= 0) ? 1 : -1;
	int mat22 = sign;

	if (w < h)
	{
		int temp = mat11;
		mat11 = mat12;
		mat12 = temp;

		temp = mat21;
		mat21 = mat22;
		mat22 = temp;
	}
	int y = 0;
	int error = 0;
	int errorDec = 2 * l;
	int errorInc = 2 * min(w, h);

	for (int x = 0; x <= l; x++)
	{
		int xt = x1 + (mat11 * x) + (mat12 * y);
		int yt = y1 + (mat21 * x) + (mat22 * y);
		SetPoint(buffer, xt, yt, color);

		if ((error += errorInc) > l)
		{
			error -= errorDec;
			y++;
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


//void UpdatePolygonsAsync()
//{
//	ThreadPool pool(8);
//	std::vector<std::future<Triangle>> futures;
//
//	for (int i = 0; i < polygons.size(); i++)
//	{
//		futures.emplace_back(pool.enqueue([i] { return UpdatePolygonsTriangle(i); }));
//	}
//
//	for (int i = 0; i < polygons.size(); i++)
//	{
//		polygonsOutp[i] = futures[i].get();
//	}
//}

//void UpdatePolygonsAsync()
//{
//	ThreadPool pool(8);
//	std::vector<std::future<void>> futures;
//
//	for (int i = 0; i < polygons.size(); i++)
//	{
//		futures.emplace_back(pool.enqueue([i] { return UpdateVectors(i); }));
//	}
//
//	for (int i = 0; i < polygons.size(); i++)
//	{
//		futures[i].get();
//	}
//}

void DrawPolygonsAsync()
{
	ThreadPool pool(8);
	std::vector<std::future<void>> futures;

	for (int i = 0; i < polygonsOutp.size(); i++)
	{
		futures.emplace_back(pool.enqueue([i] { return DrawObject(i); }));
	}

	for (int i = 0; i < polygons.size(); i++)
	{
		futures[i].get();
	}
}

double getElapsedTime()
{
	static auto lastTime = std::chrono::high_resolution_clock::now();
	auto currentTime = std::chrono::high_resolution_clock::now();
	auto elapsedTime = std::chrono::duration_cast<std::chrono::duration<double>>(currentTime - lastTime);
	lastTime = currentTime;
	return elapsedTime.count();
}