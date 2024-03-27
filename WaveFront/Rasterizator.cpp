#include "Rasterizator.h"

void Rasterizator::SortYPoints(Triangle& polygon)
{
	// ���������� ����� ���, ��� y0 <= y1 <= y2
	if (polygon.vectors[1].y < polygon.vectors[0].y) std::swap(polygon.vectors[1], polygon.vectors[0]);
	if (polygon.vectors[2].y < polygon.vectors[0].y) std::swap(polygon.vectors[2], polygon.vectors[0]);
	if (polygon.vectors[2].y < polygon.vectors[1].y) std::swap(polygon.vectors[2], polygon.vectors[1]);
}

Triangle Rasterizator::correctOrdinary(Triangle polygon)
{
	float y0 = polygon.vectors[0].y;
	float y1 = polygon.vectors[1].y;
	float y2 = polygon.vectors[2].y;

	if (y1 < y0)
	{
		SwapVectorsInPolygon(polygon, 1, 0);
	}
	if (y2 < y0)
	{
		SwapVectorsInPolygon(polygon, 2, 0);
	}
	if (y2 < y1)
	{
		SwapVectorsInPolygon(polygon, 2, 1);
	}

	return Triangle();
}

void Rasterizator::getXleftAndRight(Triangle polygon)
{
	isLeft = 0;
	// ���������� ��������� x ���� ������������
	std::vector<float> x01 = Interpolate(polygon.vectors[0].y, polygon.vectors[0].x, polygon.vectors[1].y, polygon.vectors[1].x);
	std::vector<float> x12 = Interpolate(polygon.vectors[1].y, polygon.vectors[1].x, polygon.vectors[2].y, polygon.vectors[2].x);
	std::vector<float> x02 = Interpolate(polygon.vectors[0].y, polygon.vectors[0].x, polygon.vectors[2].y, polygon.vectors[2].x);

	// ������������ �������� ������
	x01.pop_back();
	std::vector<float> x012;
	x012.insert(x012.end(), x01.begin(), x01.end());
	x012.insert(x012.end(), x12.begin(), x12.end());

	// ����������, ����� �� ������ ����� � ������
	int m = x012.size() / 2;
	if (x02[m] < x012[m]) {
		isLeft = 1;

		xLeft = x02;
		xRight = x012;
	}
	else {
		xLeft = x012;
		xRight = x02;
	}
}

void Rasterizator::getZLeftAndZRight(Triangle polygon)
{

	// ���������� �������� z ��� ������ ������������
	std::vector<float> z01 = Interpolate(polygon.vectors[0].y, polygon.vectors[0].z, polygon.vectors[1].y, polygon.vectors[1].z);
	std::vector<float> z12 = Interpolate(polygon.vectors[1].y, polygon.vectors[1].z, polygon.vectors[2].y, polygon.vectors[2].z);
	std::vector<float> z02 = Interpolate(polygon.vectors[0].y, polygon.vectors[0].z, polygon.vectors[2].y, polygon.vectors[2].z);

	// ������������ �������� z ��� �������� ������
	z01.pop_back();
	std::vector<float> z012;
	z012.insert(z012.end(), z01.begin(), z01.end());
	z012.insert(z012.end(), z12.begin(), z12.end());

	// ����������, ����� �� ������ ����� � ������
	if (isLeft == 1)
	{
		zLeft = z02;
		zRight = z012;
	}
	else
	{
		zLeft = z012;
		zRight = z02;
	}
}

void Rasterizator::getHLeftAndHRight(Triangle polygon)
{
	// ���������� �������� h ��� ������ ������������
	std::vector<float> h01 = Interpolate(polygon.vectors[0].y, polygon.vectors[0].shade, polygon.vectors[1].y, polygon.vectors[1].shade);
	std::vector<float> h12 = Interpolate(polygon.vectors[1].y, polygon.vectors[1].shade, polygon.vectors[2].y, polygon.vectors[2].shade);
	std::vector<float> h02 = Interpolate(polygon.vectors[0].y, polygon.vectors[0].shade, polygon.vectors[2].y, polygon.vectors[2].shade);

	// ������������ �������� h ��� �������� ������
	h01.pop_back();
	std::vector<float> h012;
	h012.insert(h012.end(), h01.begin(), h01.end());
	h012.insert(h012.end(), h12.begin(), h12.end());

	// ����������, ����� �� ������ ����� � ������
	if (isLeft == 1)
	{
		hLeft = h02;
		hRight = h012;
	}
	else
	{
		hLeft = h012;
		hRight = h02;
	}
}



std::vector<float> Rasterizator::Interpolate(float i0, float d0, float i1, float d1)
{
	std::vector<float> values;
	if (i0 == i1)
	{
		values.push_back(d0);
		return values;
	}

	float accretion = (d1 - d0) / (i1 - i0);
	float d = d0;
	float step = (i1 > i0) ? 1 : -1; // ���������� ��� �������������

	int imax = static_cast<int>((i1 - i0) / step) + 1;
	for (int i = 0; i < imax; i++) // �������������� i � �������������� ����
		//for (float i = i0; i != i1; i += step) // �������������� i � �������������� ����
	{
		values.push_back(d);
		d = d + accretion;
	}

	// ��������� ��������� ��������, ����� �������������, ��� d1 �������� � ���������
	values.push_back(d1);

	return values;
}


void Rasterizator::UpdateXleftAndXRight(Triangle& polygon)
{
	SortYPoints(polygon);
	getXleftAndRight(polygon);
	getZLeftAndZRight(polygon);
	getHLeftAndHRight(polygon);
}

void Rasterizator::DrawLines(Triangle polygon, RGBQUAD(&frameBuffer)[1080][1920], float(&depthBuffer)[1080][1920], RGBQUAD color)
{
	// ��������� �������������� ��������
	for (int y = polygon.vectors[0].y; y <= polygon.vectors[2].y; y++) {

		int xL = this->xLeft[y - polygon.vectors[0].y];
		int xR = this->xRight[y - polygon.vectors[0].y];
		if (xR > 1920 || xL < 0) return;

		std::vector<float> hSegment = this->Interpolate(xL, this->hLeft[y - polygon.vectors[0].y], xR, this->hRight[y - polygon.vectors[0].y]);
		for (int x = xL; x <= xR; x++) {
			if (y - polygon.vectors[0].y >= 0 && y - polygon.vectors[0].y < this->zLeft.size())
			{
				float zL = this->zLeft[y - polygon.vectors[0].y];
				float zR = this->zRight[y - polygon.vectors[0].y];

				float z;
				if (xR - xL != 0)
				{
					z = zL + (x - xL) * (zR - zL) / (xR - xL);
				}
				else
				{
					z = zL;
				}

				if (z < depthBuffer[x][y])
				{
					RGBQUAD shadedColor = { color.rgbRed * hSegment[x - xL],color.rgbGreen * hSegment[x - xL],color.rgbBlue * hSegment[x - xL], 0 };
					SetPoint(frameBuffer, x, y, shadedColor);
					depthBuffer[x][y] = z;
				}
			}

		}
	}
}

void Rasterizator::DrawPolygon(Triangle polygon, RGBQUAD(&frameBuffer)[1080][1920], float(&depthBuffer)[1080][1920], RGBQUAD color)
{
	RectangleStruct rect = FindTriangleBoundingRectangle2D(polygon);

	for (int y = rect.top; y <= rect.bottom; y++)
	{
		for (int x = rect.left; x <= rect.right; x++)
		{
			if (IsInTriangle(x, y, polygon))
			{

				SetPoint(frameBuffer, x, y, color);
			}
		}
	}
}


RectangleStruct FindTriangleBoundingRectangle2D(Triangle polygon)
{
	RectangleStruct result;
	result.bottom = getMax(polygon.vectors[0].y, polygon.vectors[1].y, polygon.vectors[2].y);
	result.top = getMin(polygon.vectors[0].y, polygon.vectors[1].y, polygon.vectors[2].y);

	result.left = getMin(polygon.vectors[0].x, polygon.vectors[1].x, polygon.vectors[2].x);
	result.right = getMax(polygon.vectors[0].x, polygon.vectors[1].x, polygon.vectors[2].x);

	return result;
}

RectangleStruct FindTriangleBoundingRectangleInGlobal(Triangle polygon)
{
	RectangleStruct result;
	result.bottom = getMax(polygon.vectorsInGlobal[0].y, polygon.vectorsInGlobal[1].y, polygon.vectorsInGlobal[2].y);
	result.top = getMin(polygon.vectorsInGlobal[0].y, polygon.vectorsInGlobal[1].y, polygon.vectorsInGlobal[2].y);

	result.left = getMin(polygon.vectorsInGlobal[0].x, polygon.vectorsInGlobal[1].x, polygon.vectorsInGlobal[2].x);
	result.right = getMax(polygon.vectorsInGlobal[0].x, polygon.vectorsInGlobal[1].x, polygon.vectorsInGlobal[2].x);

	return result;
}

bool Rasterizator::IsInTriangle(float x, float y, Triangle polygon)
{
	float inASide, inBSide, inCSide;

	// (y1-y2)x + (x2-x1)y + x1y2 - x2y1 = 0 

	inASide = (polygon.vectors[0].y - polygon.vectors[1].y) * x + (polygon.vectors[1].x - polygon.vectors[0].x) * y + polygon.vectors[0].x * polygon.vectors[1].y - polygon.vectors[1].x * polygon.vectors[0].y;
	inBSide = (polygon.vectors[1].y - polygon.vectors[2].y) * x + (polygon.vectors[2].x - polygon.vectors[1].x) * y + polygon.vectors[1].x * polygon.vectors[2].y - polygon.vectors[2].x * polygon.vectors[1].y;
	inCSide = (polygon.vectors[2].y - polygon.vectors[0].y) * x + (polygon.vectors[0].x - polygon.vectors[2].x) * y + polygon.vectors[2].x * polygon.vectors[0].y - polygon.vectors[0].x * polygon.vectors[2].y;


	return (inASide >= 0 && inBSide >= 0 && inCSide >= 0) || (inASide < 0 && inBSide < 0 && inCSide < 0);
}


// E01(P) = (P.x - V0.x) * (V1.y - V0.y) - (P.y - V0.y) * (V1.x - V0.x)
bool isInEdgeFunction(const HomogeneousCoordinateStruct& a, const HomogeneousCoordinateStruct& b, float x, float y)
{
	return ((x - a.x) * (b.y - a.y) - (y - a.y) * (b.x - a.x) >= 0);
}

bool Rasterizator::IsInPolygon(float x, float y, const Triangle& polygon)
{
	bool inside = true;
	inside &= isInEdgeFunction(polygon.vectors[0], polygon.vectors[1], x, y);
	inside &= isInEdgeFunction(polygon.vectors[1], polygon.vectors[2], x, y);
	inside &= isInEdgeFunction(polygon.vectors[2], polygon.vectors[0], x, y);


	return inside;
}

// E01(P) = (P.x - V0.x) * (V1.y - V0.y) - (P.y - V0.y) * (V1.x - V0.x)
float edgeFunction(const HomogeneousCoordinateStruct& a, const HomogeneousCoordinateStruct& b, float x, float y)
{
	return (x - a.x) * (b.y - a.y) - (y - a.y) * (b.x - a.x);
}

// E01(P) = (V0.x - V1.x) * (P.y - V0.y) - (V0.y - V1.y) * (P.x - V0.x)
float edgeFunctionReversePositive(const HomogeneousCoordinateStruct& a, const HomogeneousCoordinateStruct& b, float x, float y)
{
	return (a.x - b.x) * (y - a.y) - (a.y - b.y) * (x - a.x);
}

void Rasterizator::DrawPolygonBarycentric(const Triangle& polygon, std::vector<PointLightStruct> lightnings,CoordinateStruct& CameraGlobalCoordinates, RGBQUAD(&frameBuffer)[1080][1920], float(&depthBuffer)[1080][1920], RGBQUAD color)
{
	bool istopleft = IsTopLeft(polygon);
	RectangleStruct rect = FindTriangleBoundingRectangle2D(polygon);
	//RectangleStruct rectInGlobal = FindTriangleBoundingRectangleInGlobal(polygon);

	float w0, w1, w2, area;
	//float weightInGlobal0, weightInGlobal1, weightInGlobal2, areaInGlobal;

	area = edgeFunction(polygon.vectors[0], polygon.vectors[1], polygon.vectors[2].x, polygon.vectors[2].y);
	//areaInGlobal = edgeFunction(polygon.vectorsInGlobal[0], polygon.vectorsInGlobal[1], polygon.vectorsInGlobal[2].x, polygon.vectorsInGlobal[2].y);

	for (int y = rect.top; y <= rect.bottom; y++)
	{
		for (int x = rect.left; x <= rect.right; x++)
		{
			
				w0 = edgeFunction(polygon.vectors[1], polygon.vectors[2], x, y);
				w1 = edgeFunction(polygon.vectors[2], polygon.vectors[0], x, y);
				w2 = edgeFunction(polygon.vectors[0], polygon.vectors[1], x, y);


			

			if ((w0 >= 0 && w1 >= 0 && w2 >= 0) || (w0 < 0 && w1 < 0 && w2 < 0))
			{
				w0 /= area;
				w1 /= area;
				w2 /= area;


				float oneOverZ = polygon.vectors[0].z * w0 + polygon.vectors[1].z * w1 + polygon.vectors[2].z * w2;
				float z = 1 / ( - oneOverZ);
				// TODO : Lock
				if (z < depthBuffer[x][y])
				{
					depthBuffer[x][y] = z;

					///////////////////////////////////////////////////////////////////////////////////////////////////
					float curXInGlobal = polygon.vectorsInGlobal[0].x * w0 + polygon.vectorsInGlobal[1].x * w1 + polygon.vectorsInGlobal[2].x * w2;
					float curYInGlobal = polygon.vectorsInGlobal[0].y * w0 + polygon.vectorsInGlobal[1].y * w1 + polygon.vectorsInGlobal[2].y * w2;
					float curZInGlobal = polygon.vectorsInGlobal[0].z * w0 + polygon.vectorsInGlobal[1].z * w1 + polygon.vectorsInGlobal[2].z * w2;
					CoordinateStruct curPointInGlobal = { curXInGlobal, curYInGlobal, curZInGlobal };

					float curXNormalInGlobal = polygon.vectorsInGlobal[0].normal.x * w0 + polygon.vectorsInGlobal[1].normal.x * w1 + polygon.vectorsInGlobal[2].normal.x * w2;
					float curYNormalInGlobal = polygon.vectorsInGlobal[0].normal.y * w0 + polygon.vectorsInGlobal[1].normal.y * w1 + polygon.vectorsInGlobal[2].normal.y * w2;
					float curZNormalInGlobal = polygon.vectorsInGlobal[0].normal.z * w0 + polygon.vectorsInGlobal[1].normal.z * w1 + polygon.vectorsInGlobal[2].normal.z * w2;
					CoordinateStruct hitNormal = { curXNormalInGlobal, curYNormalInGlobal, curZNormalInGlobal };

					CoordinateStruct hitColor = calculatePhongLight(curPointInGlobal, hitNormal,CameraGlobalCoordinates, lightnings);
					///////////////////////////////////////////////////////////////////////////////////////////////////

					/*float r = w0 * polygon.vectors[0].diffuse.x + w1 * polygon.vectors[1].diffuse.x + w2 * polygon.vectors[2].diffuse.x;
					float g = w0 * polygon.vectors[0].diffuse.y + w1 * polygon.vectors[1].diffuse.y + w2 * polygon.vectors[2].diffuse.y;
					float b = w0 * polygon.vectors[0].diffuse.z + w1 * polygon.vectors[1].diffuse.z + w2 * polygon.vectors[2].diffuse.z;
					RGBQUAD shadedColor = { r,g,b,0 };*/

					RGBQUAD lightedColor = { color.rgbBlue * hitColor.x, color.rgbGreen * hitColor.y, color.rgbRed * hitColor.z };
					SetPoint(frameBuffer, x, y, /*shadedColor*/ lightedColor);

				}
			}
		}
	}
}

bool Rasterizator::IsTopLeft(const Triangle& polygon)
{
	// ���������, ��� ����� v0, v1 � v2 ���� �� ������� �������
	float area = (polygon.vectors[1].x - polygon.vectors[0].x) * (polygon.vectors[2].y - polygon.vectors[0].y) - (polygon.vectors[1].y - polygon.vectors[0].y) * (polygon.vectors[2].x - polygon.vectors[0].x);
	if (area < 0.0f)
	{
		return false;
	}

	// ���������, ��� ����� v0 �������� ����� ������� ������ ������������
	if (polygon.vectors[0].y != getMin(polygon.vectors[0].y, polygon.vectors[1].y, polygon.vectors[2].y))
	{
		return false;
	}

	// ���������, ��� ����� v0 �������� ����� ����� ������ ������������
	if (polygon.vectors[0].x != getMin( polygon.vectors[0].x, polygon.vectors[1].x, polygon.vectors[2].x ))
	{
		return false;
	}

	return true;
}