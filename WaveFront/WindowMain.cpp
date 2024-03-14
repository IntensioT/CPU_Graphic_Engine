#ifndef UNICODE
#define UNICODE
#endif 


#include "WindowMain.h"


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

	polygons.resize(indexes.size() / 3);
	polygons = GetAllPolygons(vertexesHomo, indexes, normalIndexes, normals);
	polygonsOutp.resize(polygons.size());

	/////

	rasterizator = new Rasterizator();

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
			zCamera += 0.05f;
			rSphere += 0.05f;
		}
		else if (wheelDelta < 0)
		{
			if (zCamera > 1.f)
			{
				zCamera -= 0.05f;
				rSphere -= 0.05f;
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
		}


	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	return 0;

}

void ShowFrame(unsigned int width, unsigned int height, void* pixels, HWND hWnd)
{
	HDC hdc = GetDC(hWnd);


	HBITMAP hBitMap = CreateBitmap(width, height, 1, 8 * 4, pixels);
	//HBITMAP hBitMap = CreateCompatibleBitmap(hdc, width, height);
	/*
	Однако для повышения производительности приложения должны использовать CreateBitmap для создания монохромных растровых изображений и
	CreateCompatibleBitmap для создания цветных растровых изображений
	  [in] nPlanes
	Количество цветовых плоскостей, используемых устройством.
	  [in] nBitCount (4)
	Количество битов, необходимых для идентификации цвета одного пикселя.
	 */


	 // ��������� DC ��� �������� bit-map'�
	HDC srcHdc = CreateCompatibleDC(hdc);

	SelectObject(srcHdc, hBitMap);

	BitBlt(hdc, 0, 0, static_cast<int>(width), static_cast<int>(height), srcHdc, 0, 0, SRCCOPY);
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
	std::memset(frameBuffer, 0, sizeof(frameBuffer));

	const float MAX_DEPTH = 3.4E+38f;
	const float MIN_DEPTH = -3.4E+38f;
	for (int i = 0; i < 1080; ++i) {
		for (int j = 0; j < 1920; ++j) {
			depthBuffer[i][j] = MAX_DEPTH;
			//depthBuffer[i][j] = MIN_DEPTH;
		}
	}

	UpdateNormals();
	UpdateVectors();
}

void UpdateNormals()
{
	modelCoordSystem->SetRotateYMatrix(GetRadians(yAngleObject));
	modelCoordSystem->SetRotateXMatrix(GetRadians(xAngleObject));
	modelCoordSystem->GlobalTransformationMatrix = modelCoordSystem->RotateYMatrix * modelCoordSystem->RotateXMatrix;

	cameraGlobalCoord = { SphericalToCartesian(rSphere, phiAngleSphere, thetaAngleSphere) };
	CoordinateStruct cameraUpVect = { 0,1,0 };
	modelCoordSystem->SetCameraTransformationMatrix(cameraGlobalCoord, targetGlobalCoord, cameraUpVect);

	HomogeneousCoordinateStruct homoNormal;
	for (int i = 0; i < polygons.size(); i++)
	{
		for (int j = 0; j < vectorCount; j++)
		{
			modelCoordSystem->SetTranslationMatrix({ polygons[i].vectors[j].x, polygons[i].vectors[j].y, polygons[i].vectors[j].z });

			homoNormal = { polygons[i].vectors[j].normal.x, polygons[i].vectors[j].normal.y, polygons[i].vectors[j].normal.z, 1 };

			homoNormal *= modelCoordSystem->GlobalTransformationMatrix;
			homoNormal *= modelCoordSystem->TranslationMatrix;
			homoNormal *= modelCoordSystem->CameraTransformationMatrix;

			polygonsOutp[i].vectors[j].normal = { homoNormal.x, homoNormal.y , homoNormal.z };
			polygonsOutp[i].vectors[j].normalW = homoNormal.w;
		}
	}
}

void UpdateVectors()
{
	std::vector<Plane> clipPlanes = {
		{ {1, 0, 0}, 1 },
		{ {-1, 0, 0}, -1 },
		{ {0, 1, 0}, 1 },
		{ {0, -1, 0}, -1 },
		{ {0, 0, 1}, 1 },
		{ {0, 0, -1}, -1 }
	};


	modelCoordSystem->SetProjectionTransformationMatrix(45.0f, ((float)FrameWidth / (float)FrameHeight), zNear, zFar);

	modelCoordSystem->SetViewPortTransformationMatrix((float)FrameWidth, (float)FrameHeight, 0, 0, 0.0f, 1.0f);


	for (int i = 0; i < polygons.size(); i++)
	{
		if (!UpdatePolygons(i)) continue;
		if (!IsObjectBehindClipPlanes(i, clipPlanes))
		{
			DrawObject(i);
		}
	}

	//UpdatePolygonsAsync();
}


bool IsObjectBehindClipPlanes(int polygonIterator, const std::vector<Plane>& clipPlanes)
{
	Triangle polygon;
	HomogeneousCoordinateStruct pointHomogeneous;

	polygon = polygonsOutp[polygonIterator];

	// Normal clipping
	CoordinateStruct cameraDirection = modelCoordSystem->NormalizeVector(modelCoordSystem->SubstractVectors(cameraGlobalCoord, targetGlobalCoord));


	//float dotProduct = modelCoordSystem->DotProduct(polygon.normal, cameraGlobalCoord);

	//CoordinateStruct vect, ZAxis;

	for (int i = 0; i < 3; i++)
	{
		//vect = { polygon.vectors[i].x, polygon.vectors[i].y, polygon.vectors[i].z };
		//ZAxis = modelCoordSystem->NormalizeVector(modelCoordSystem->SubstractVectors(cameraGlobalCoord, vect));

		CoordinateStruct normal = { polygon.vectors[i].normal.x, polygon.vectors[i].normal.y, polygon.vectors[i].normal.z };

		float dotProduct = modelCoordSystem->DotProduct(normal, cameraDirection);
		/*if (dotProduct < 0)
		{
			return true;
		}*/

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

void DrawObject(int i)
{
	vectorCount = sizeof(polygonsOutp[i].vectors) / sizeof(polygonsOutp[i].vectors[0]);
	HomogeneousCoordinateStruct homoNormal0, homoNormal1, homoNormal2;

	switch (curGraphic)
	{
	case 1:
		BresenhamLineOptimised(frameBuffer, polygonsOutp[i].vectors[0], polygonsOutp[i].vectors[0 + 1], color2);
		BresenhamLineOptimised(frameBuffer, polygonsOutp[i].vectors[0 + 1], polygonsOutp[i].vectors[0 + 2], color2);
		BresenhamLineOptimised(frameBuffer, polygonsOutp[i].vectors[0 + 2], polygonsOutp[i].vectors[0], color2);
		break;
	case 2:
		for (auto j = 0; j < vectorCount; j++)
		{
			if (polygonsOutp[i].vectors[j].x > 1920 || polygonsOutp[i].vectors[j].x < 0 || polygonsOutp[i].vectors[j].y > 1080 || polygonsOutp[i].vectors[j].y < 0) return;
		}

		rasterizator->UpdateXleftAndXRight(polygonsOutp[i]);

		rasterizator->DrawLines(polygonsOutp[i], frameBuffer, depthBuffer, color);
		break;
	case 3:
		BresenhamLineOptimised(frameBuffer, polygonsOutp[i].vectors[0], polygonsOutp[i].vectors[0 + 1], color2);
		BresenhamLineOptimised(frameBuffer, polygonsOutp[i].vectors[0 + 1], polygonsOutp[i].vectors[0 + 2], color2);
		BresenhamLineOptimised(frameBuffer, polygonsOutp[i].vectors[0 + 2], polygonsOutp[i].vectors[0], color2);

		homoNormal0 = { polygonsOutp[i].vectors[0].projectedNormal.x, polygonsOutp[i].vectors[0].projectedNormal.y, polygonsOutp[i].vectors[0].projectedNormal.z, 1 };
		homoNormal1 = { polygonsOutp[i].vectors[1].projectedNormal.x, polygonsOutp[i].vectors[1].projectedNormal.y, polygonsOutp[i].vectors[1].projectedNormal.z, 1 };
		homoNormal2 = { polygonsOutp[i].vectors[2].projectedNormal.x, polygonsOutp[i].vectors[2].projectedNormal.y, polygonsOutp[i].vectors[2].projectedNormal.z, 1 };
		BresenhamLineOptimised(frameBuffer, polygonsOutp[i].vectors[0], homoNormal0, color);
		BresenhamLineOptimised(frameBuffer, polygonsOutp[i].vectors[0 + 1], homoNormal1, color);
		BresenhamLineOptimised(frameBuffer, polygonsOutp[i].vectors[0 + 2], homoNormal2, color);
		break;
	}
}

bool UpdatePolygons(int polygonIterator)
{
	Triangle polygon;
	HomogeneousCoordinateStruct pointHomogeneous, normalHomogeneous;

	for (int i = 0; i < 3; i++)
	{
		pointHomogeneous = polygons[polygonIterator].vectors[i];
		normalHomogeneous = { polygonsOutp[polygonIterator].vectors[i].normal.x, polygonsOutp[polygonIterator].vectors[i].normal.y, polygonsOutp[polygonIterator].vectors[i].normal.z,polygonsOutp[polygonIterator].vectors[i].normalW };
		


		pointHomogeneous *= modelCoordSystem->GlobalTransformationMatrix;
		pointHomogeneous *= modelCoordSystem->CameraTransformationMatrix;

		CoordinateStruct curVector = { pointHomogeneous.x,pointHomogeneous.y,pointHomogeneous.z };

		pointHomogeneous *= modelCoordSystem->ProjectionTransformationMatrix;


		//////////////////////////////////////////Clipping////////////////////////////////////////////////////////////////

		CoordinateStruct cameraDirectionOnPoint = modelCoordSystem->NormalizeVector(modelCoordSystem->SubstractVectors(cameraGlobalCoord, curVector));

		//CoordinateStruct curNormal = polygons[polygonIterator].vectors[i].normal;
		CoordinateStruct curNormal = { normalHomogeneous.x, normalHomogeneous.y, normalHomogeneous.z };

		float dotProduct = modelCoordSystem->DotProduct(curNormal, cameraDirectionOnPoint);
		/*if (dotProduct < 0)
		{
			return false;
		}*/

		if (pointHomogeneous.w < 0.4 && pointHomogeneous.w > -0.4)
		{
			return false;
		}

		pointHomogeneous *= (1 / pointHomogeneous.w);
		pointHomogeneous *= modelCoordSystem->ViewPortTransformationMatrix;


		normalHomogeneous *= modelCoordSystem->ProjectionTransformationMatrix;
		normalHomogeneous *= (1 / normalHomogeneous.w);
		normalHomogeneous *= modelCoordSystem->ViewPortTransformationMatrix;

		//pointHomogeneous.shade = 1;
		polygon.vectors[i] = pointHomogeneous;
		polygon.vectors[i].projectedNormal = { normalHomogeneous.x, normalHomogeneous.y, normalHomogeneous.z };

		///////////////////////////////// LAMBERT///////////////////////////////////////////////////////
		//CoordinateStruct normal = modelCoordSystem->NormalizeVector(polygon.vectors[i].normal);
		CoordinateStruct normal = modelCoordSystem->NormalizeVector(polygonsOutp[polygonIterator].vectors[i].normal);
		CoordinateStruct curPos = { polygon.vectors[i].x,polygon.vectors[i].y,polygon.vectors[i].z };


		CoordinateStruct lightDirection = modelCoordSystem->NormalizeVector(modelCoordSystem->SubstractVectors(lightGlobalCoord, curVector));

		float lambertTerm = (modelCoordSystem->DotProduct(normal, lightDirection) >= 0.0) ? modelCoordSystem->DotProduct(normal, lightDirection) : 0.0f;
		polygon.vectors[i].shade = lambertTerm;

		polygon.vectors[i].diffuse = { DiffuseLightColor.x * lambertTerm, DiffuseLightColor.y * lambertTerm, DiffuseLightColor.z * lambertTerm };
	}


	polygonsOutp[polygonIterator] = polygon;
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
		pointHomogeneous *= modelCoordSystem->CameraTransformationMatrix;
		pointHomogeneous *= modelCoordSystem->ProjectionTransformationMatrix;

		CoordinateStruct curVector = { pointHomogeneous.x,pointHomogeneous.y,pointHomogeneous.z };

		if (pointHomogeneous.w < 0.4 && pointHomogeneous.w > -0.4)
		{
			pointHomogeneous = { 0,0,0,1 };
		}
		else
		{
			pointHomogeneous *= (1 / pointHomogeneous.w);
			pointHomogeneous *= modelCoordSystem->ViewPortTransformationMatrix;
		}
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


void UpdatePolygonsAsync()
{
	ThreadPool pool(8);
	std::vector<std::future<Triangle>> futures;

	for (int i = 0; i < polygons.size(); i++)
	{
		futures.emplace_back(pool.enqueue([i] { return UpdatePolygonsTriangle(i); }));
	}

	for (int i = 0; i < polygons.size(); i++)
	{
		polygonsOutp[i] = futures[i].get();
	}
}