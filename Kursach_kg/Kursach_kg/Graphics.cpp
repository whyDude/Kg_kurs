#include "Graphics.h"

void graphics::Graphics::drawPixel(int32_t x, int32_t y, uint32_t color)
{
	if (checkBounds(x, y)) at(x, y) = color;
}

void graphics::Graphics::drawPixel(int32_t x, int32_t y, uint32_t color, controls::WidgetRect* canvasRect)
{
	if ((x >= canvasRect->left) && (x <= canvasRect->right - 1) &&
		(y >= canvasRect->top) && (y <= canvasRect->bottom - 1)) at(x, y) = color;
}

void graphics::Graphics::drawLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t color)
{
	drawLineBresenham(x1, y1, x2, y2, color);
}

void graphics::Graphics::drawLineBresenham(int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t color)
{
	/*	dx = (x2-x1)
		dy = (y2-y1)

		f(x,y) = dy*X - dx*Y - (x1*dy - y1*dx)

		f(M) = dy - dx/2
		f(T) = f(M) + dy - dx
		f(B) = f(M) + dy
	*/

	int16_t x, y;
	int16_t deltaX, deltaY;
	int16_t signX, signY;
	int16_t balance;

	signX = (x2 > x1) ? 1 : -1;
	signY = (y2 > y1) ? 1 : -1;

	deltaX = (signX > 0) ? (x2 - x1) : (x1 - x2);
	deltaY = (signY > 0) ? (y2 - y1) : (y1 - y2);

	x = x1; y = y1;

	if (deltaX >= deltaY)				// We should use larger axis [X>=Y]
	{
		deltaY <<= 1;
		balance = deltaY - deltaX;		// f(M)
		deltaX <<= 1;

		while (x != x2)
		{
			drawPixel(x, y, color);
			if (balance >= 0)
			{
				y += signY;
				balance -= deltaX;		// f(T) = f(M) - dx => half part f(T) [still need dy]
			}

			balance += deltaY;			// f(M) + dy => f(B) or part of f(T)
			x += signX;
		}

		drawPixel(x, y, color);
	}

	else								// Similarly for axis [Y>X]
	{
		deltaX <<= 1;
		balance = deltaX - deltaY;
		deltaY <<= 1;

		while (y != y2)
		{
			drawPixel(x, y, color);
			if (balance >= 0)
			{
				x += signX;
				balance -= deltaY;
			}

			balance += deltaX;
			y += signY;
		}

		drawPixel(x, y, color);
	}
}

bool graphics::Graphics::crossingNumber(math::Vec3d<float>* points[], int16_t numberPoint, math::Vec3d<float>* checkPoint)
{
	// This function determines number of intersections with polygon
	// https://stackoverflow.com/questions/27589796/check-point-within-polygon

	int16_t cn = 0;    // the  crossing number counter
	int16_t i = 0;

	do
	{
		int16_t next = (i + 1) % numberPoint;

		if (((points[i]->y <= checkPoint->y) && (points[next]->y > checkPoint->y)) ||
			((points[i]->y > checkPoint->y) && (points[next]->y <= checkPoint->y)))
		{
			// Высчитываем X точку пересечения, выпущенного луча с прямой (как в Scanline)
			float deltaXY = (points[next]->x - points[i]->x) / (points[next]->y - points[i]->y);
			if (checkPoint->x < points[i]->x + deltaXY * (checkPoint->y - points[i]->y))
				++cn;
		}
		i = next;
	} while (i != 0);

	return (cn & 1);    // 0 if even (out), and 1 if  odd (in)
}

bool graphics::Graphics::segmentIntersection(const math::Vec3d<float>& A, const math::Vec3d<float>& B, math::Vec3d<float>& C, math::Vec3d<float>& D)
{
	// Line AB represented as a1x + b1y = c1
	float a1 = B.y - A.y;
	float b1 = A.x - B.x;
	float c1 = a1 * (A.x) + b1 * (A.y);

	// Line CD represented as a2x + b2y = c2
	float a2 = D.y - C.y;
	float b2 = C.x - D.x;
	float c2 = a2 * (C.x) + b2 * (C.y);

	float determinant = a1 * b2 - a2 * b1;
	return (determinant <= 0.000001) ? 0 : 1;
}

float graphics::Graphics::getZDepth(math::Vec3d<float>& A, math::Vec3d<float>& B, math::Vec3d<float>& C, math::Vec3d<float>& P)
{
	math::Vec3d<float> line1(B - A), line2(C - A);
	math::Vec3d<float> faceNormal = line1.crossProduct(line2);
	faceNormal.normalize();

	math::Vec3d<float> lineNormal(0.0f, 0.0f, -1.0f);
	lineNormal.normalize();

	float t;
	t = (faceNormal.dotProduct(A) - faceNormal.dotProduct(P)) / faceNormal.dotProduct(lineNormal);

	math::Vec3d<float> p = P + lineNormal * t;
	return p.z;
}

#define WARNOCK_OPTIMIZED
void graphics::Graphics::warnockAlgorithm(const std::vector<Triangle<float>*>& transformedTriangles, float left, float top, float right, float bottom, bool showEdges)
{
	math::Vec3d<float> windowRect[4] = {
		{left, top, 0.0f}, {right, top, 0.0f},
		{right, bottom, 0.0f}, {left, bottom, 0.0f}
	};

	math::Vec3d<float>* pointsCheck[4];
	int16_t crossingPointsCounter = 0;

	std::vector<std::pair<Triangle<float>*, int16_t>> tempTriangles;
	// Проходимся по всем треугольникам
	for (auto& tri : transformedTriangles)
	{
		crossingPointsCounter = 0;

		// Смотрим есть ли точки треугольника в окне
		pointsCheck[0] = &windowRect[0];
		pointsCheck[1] = &windowRect[1];
		pointsCheck[2] = &windowRect[2];
		pointsCheck[3] = &windowRect[3];

		for (int16_t i = 0; i < 3; ++i)
			if (crossingNumber(pointsCheck, 4, &tri->points[i])) ++crossingPointsCounter;

		// Лежит ли внутри хотя бы 1 точка?
		if (crossingPointsCounter)
		{
			tempTriangles.push_back(std::make_pair(tri, crossingPointsCounter));
			continue;
		}

		// Иначе треугольник не лежит в окне
		// Проверяем, лежит ли окно в треугольнике
		pointsCheck[0] = &tri->points[0];
		pointsCheck[1] = &tri->points[1];
		pointsCheck[2] = &tri->points[2];
		pointsCheck[3] = nullptr;

		crossingPointsCounter = 0;

		for (int16_t i = 0; i < 4; ++i)
			if (crossingNumber(pointsCheck, 3, &windowRect[i])) ++crossingPointsCounter;

		// Лежит ли внутри хотя бы 1 точка?
		if (crossingPointsCounter)
		{
			tempTriangles.push_back(std::make_pair(tri, -crossingPointsCounter));
			continue;
		}

#ifdef WARNOCK_OPTIMIZED
		// Если ни треугольник, ни окно не содержат точки друг друга, то проверяем, пересекаются ли их отрезки
	
		float minX = min(min(tri->points[0].x, tri->points[1].x), tri->points[2].x);
		float maxX = max(max(tri->points[0].x, tri->points[1].x), tri->points[2].x);
	
		float minY = min(min(tri->points[0].y, tri->points[1].y), tri->points[2].y);
		float maxY = max(max(tri->points[0].y, tri->points[1].y), tri->points[2].y);
	
		if (minX > right || minY > bottom) continue;
		if (maxX < left || maxY < top) continue;
	
		if ((right - left <= 1.0f) && (bottom - top <= 1.0f)) continue;
	
		int16_t i = 0;
		bool isIntersected = false;
		do
		{
			int16_t nextI = (i + 1) % 3;
	
			// Ищем пересечение каждого ребра треугольника с каждым ребром окна
			int16_t j = 0;
			do
			{
				int16_t nextJ = (j + 1) % 4;
				if (segmentIntersection(tri->points[i], tri->points[nextI], windowRect[j], windowRect[nextJ])) { isIntersected = true; break; }
				j = nextJ;
			} while (j != 0);
			if (isIntersected) break;
	
			i = nextI;
		} while (i != 0);
	
		if (isIntersected)
			tempTriangles.push_back(std::make_pair(tri, 0));
#endif
	}

	if (showEdges)
	{
		// Show net
		drawLine(left, top, right, top, config::objectPreset::color::standartEdgesColor);
		drawLine(left, top, left, bottom, config::objectPreset::color::standartEdgesColor);
		drawLine(left, bottom, right, bottom, config::objectPreset::color::standartEdgesColor);
		drawLine(right, top, right, bottom, config::objectPreset::color::standartEdgesColor);
	}

	// Анализируем сохраненные нами треугольники
		// Если ничего нет, то просто выходим
	if (tempTriangles.empty())
	{
		//fillRect(left, top, right, bottom, 45000);
		return;
	}
	else if (tempTriangles.size() == 1)
	{
		// Если у нас всего 1 треугольник, смотрим какой он
		switch (tempTriangles.front().second)
		{
			// Если он закрывает собой весь экран, то красим окно в его цвет
		case -4:
			fillRect(left, top, right, bottom, tempTriangles.front().first->color);
			break;
			// Если он лежит внутри окна
		case 3:
		{
			math::Vec3d<int32_t> p[3];
			for (int16_t i = 0; i < 3; i++)
			{
				p[i].x = roundf(tempTriangles.front().first->points[i].x);
				p[i].y = roundf(tempTriangles.front().first->points[i].y);
			}
			fillTriangle(p, tempTriangles.front().first->color);
			break;
		}
		// В ином случае он частично лежит в окне
		default:
			math::Vec3d<int32_t> p[3];
			for (int16_t i = 0; i < 3; i++)
			{
				p[i].x = roundf(tempTriangles.front().first->points[i].x);
				p[i].y = roundf(tempTriangles.front().first->points[i].y);
			}
			controls::WidgetRect canvasRect(left, top, right - left, bottom - top);
			fillTriangle(p, tempTriangles.front().first->color, &canvasRect);
			break;
		}
	}	// Если несколько треугольников в окне, то разбиваем его на подокна
	else
	{
		float deltaX, deltaY;
		deltaX = (right - left > 1.0f) ? ((right - left) / 2.0f) : 1.0f;
		deltaY = (bottom - top > 1.0f) ? ((bottom - top) / 2.0f) : 1.0f;

		if (deltaX) deltaX = roundf(deltaX);
		if (deltaY) deltaY = roundf(deltaY);

		// Если большие окна, то разбиваем их на 4 части
		if ((right - left > 1.0f) && (bottom - top > 1.0f))
		{
#ifdef WARNOCK_OPTIMIZED
			std::vector<Triangle<float>*> triangleOnWindow;
			for (auto& t : tempTriangles)
			{
				triangleOnWindow.push_back(t.first);
			}
			tempTriangles.clear();

			warnockAlgorithm(triangleOnWindow, left, top, left + deltaX, top + deltaY, showEdges);			// Левое верхнее
			warnockAlgorithm(triangleOnWindow, left + deltaX, top, right, top + deltaY, showEdges);			// Правое верхнее
			warnockAlgorithm(triangleOnWindow, left, top + deltaY, left + deltaX, bottom, showEdges);		// Левое нижнее
			warnockAlgorithm(triangleOnWindow, left + deltaX, top + deltaY, right, bottom, showEdges);		// Правое нижнее
#else			
			warnockAlgorithm(transformedTriangles, left, top, left + deltaX, top + deltaY, showEdges);			// Левое верхнее
			warnockAlgorithm(transformedTriangles, left + deltaX, top, right, top + deltaY, showEdges);			// Правое верхнее
			warnockAlgorithm(transformedTriangles, left, top + deltaY, left + deltaX, bottom, showEdges);		// Левое нижнее
			warnockAlgorithm(transformedTriangles, left + deltaX, top + deltaY, right, bottom, showEdges);		// Правое нижнее
#endif
		}
		// Если по Y 1 пиксель
		else if ((right - left > 1.0f) && (bottom - top <= 1.0f))
		{
#ifdef WARNOCK_OPTIMIZED
			std::vector<Triangle<float>*> triangleOnWindow;
			for (auto& t : tempTriangles)
			{
				triangleOnWindow.push_back(t.first);
			}
			tempTriangles.clear();

			// Разбиваем на два горизонтальных окна
			warnockAlgorithm(triangleOnWindow, left, top, left + deltaX, bottom, showEdges);		// Левое
			warnockAlgorithm(triangleOnWindow, left + deltaX, top, right, bottom, showEdges);		// Правое
#else
			warnockAlgorithm(transformedTriangles, left, top, left + deltaX, bottom, showEdges);		// Левое
			warnockAlgorithm(transformedTriangles, left + deltaX, top, right, bottom, showEdges);		// Правое
#endif
		}
		// Если по X 1 пиксель
		else if ((right - left <= 1.0f) && (bottom - top > 1.0f))
		{
#ifdef WARNOCK_OPTIMIZED
			std::vector<Triangle<float>*> triangleOnWindow;
			for (auto& t : tempTriangles)
			{
				triangleOnWindow.push_back(t.first);
			}
			tempTriangles.clear();

			// Разбиваем на два вертикальных окна
			warnockAlgorithm(triangleOnWindow, left, top, right, top + deltaY, showEdges);			// Верхнее
			warnockAlgorithm(triangleOnWindow, left, top + deltaY, right, bottom, showEdges);		// Нижнее
#else
			warnockAlgorithm(transformedTriangles, left, top, right, top + deltaY, showEdges);			// Верхнее
			warnockAlgorithm(transformedTriangles, left, top + deltaY, right, bottom, showEdges);		// Нижнее
#endif
		}
		else
		{
			// Если окно в 1 пиксель
			// Высчитываем минимальную глубину среди всех треугольников и закрашиваем нужным цветом

			math::Vec3d<float> p(static_cast<float>(left), static_cast<float>(top), 0.0f);
			uint32_t color{ 64000 };
			float minZ = FLT_MAX; float tempZ{};

			for (auto& tri : tempTriangles)
			{
				tempZ = getZDepth(tri.first->points[0], tri.first->points[1], tri.first->points[2], p);

				if (tempZ < minZ)
				{
					minZ = tempZ;
					color = tri.first->color;
				}
			}
			drawPixel(left, top, color);
		}
	}
}
#undef WARNOCK_OPTIMIZED

void graphics::Graphics::drawPolygon(math::Vec2d<int32_t>* points, int16_t cpt, uint32_t color)
{
	int16_t pntIn = 0;
	bool needClip = false;

	for (int16_t i = 0; i < cpt; ++i)
	{
		if (checkBounds(points[i].x, points[i].y)) ++pntIn;
		else needClip = true;
	}

	if (pntIn == 0) return;

	for (int16_t i = 0, next = 0; i < cpt; ++i)
	{
		next = (i + 1) % cpt;
		drawLine(points[i], points[next], color);
	}
}

void graphics::Graphics::drawTriangle(math::Vec3d<int32_t>* points, uint32_t color)
{
	drawLine(points[0], points[1], color);
	drawLine(points[1], points[2], color);
	drawLine(points[2], points[0], color);
}

// https://www.avrfreaks.net/sites/default/files/triangles.c
void graphics::Graphics::fillTriangle(math::Vec3d<int32_t>* points, uint32_t color, controls::WidgetRect* canvasRect)
{
	int16_t x1, x2, x3, y1, y2, y3;
	x1 = points[0].x; y1 = points[0].y;
	x2 = points[1].x; y2 = points[1].y;
	x3 = points[2].x; y3 = points[2].y;

	auto drawline = [&](int16_t sx, int16_t ex, int16_t ny) { for (int16_t i = sx; i <= ex; i++) if (canvasRect) drawPixel(i, ny, color, canvasRect); else drawPixel(i, ny, color); };

	int16_t t1x, t2x, y, minx, maxx, t1xp, t2xp;
	bool changed1 = false;
	bool changed2 = false;
	int16_t signx1, signx2, dx1, dy1, dx2, dy2;
	int16_t e1, e2;
	// Sort vertices
	if (y1 > y2) { std::swap(y1, y2); std::swap(x1, x2); }
	if (y1 > y3) { std::swap(y1, y3); std::swap(x1, x3); }
	if (y2 > y3) { std::swap(y2, y3); std::swap(x2, x3); }

	t1x = t2x = x1; y = y1;   // Starting points
	dx1 = static_cast<int16_t>(x2 - x1);
	if (dx1 < 0) { dx1 = -dx1; signx1 = -1; }
	else signx1 = 1;
	dy1 = static_cast<int16_t>(y2 - y1);

	dx2 = static_cast<int16_t>(x3 - x1);
	if (dx2 < 0) { dx2 = -dx2; signx2 = -1; }
	else signx2 = 1;
	dy2 = static_cast<int16_t>(y3 - y1);

	if (dy1 > dx1) { std::swap(dx1, dy1); changed1 = true; }
	if (dy2 > dx2) { std::swap(dy2, dx2); changed2 = true; }

	e2 = static_cast<int16_t>(dx2 >> 1);
	// Flat top, just process the second half
	if (y1 == y2) goto next;
	e1 = static_cast<int16_t>(dx1 >> 1);

	for (int16_t i = 0; i < dx1;) {
		t1xp = 0; t2xp = 0;
		if (t1x < t2x) { minx = t1x; maxx = t2x; }
		else { minx = t2x; maxx = t1x; }
		// process first line until y value is about to change
		while (i < dx1) {
			i++;
			e1 += dy1;
			while (e1 >= dx1) {
				e1 -= dx1;
				if (changed1) t1xp = signx1;//t1x += signx1;
				else          goto next1;
			}
			if (changed1) break;
			else t1x += signx1;
		}
		// Move line
	next1:
		// process second line until y value is about to change
		while (1) {
			e2 += dy2;
			while (e2 >= dx2) {
				e2 -= dx2;
				if (changed2) t2xp = signx2;//t2x += signx2;
				else          goto next2;
			}
			if (changed2)     break;
			else              t2x += signx2;
		}
	next2:
		if (minx > t1x) minx = t1x;
		if (minx > t2x) minx = t2x;
		if (maxx < t1x) maxx = t1x;
		if (maxx < t2x) maxx = t2x;
		drawline(minx, maxx, y);    // Draw line from min to max points found on the y
									// Now increase y
		if (!changed1) t1x += signx1;
		t1x += t1xp;
		if (!changed2) t2x += signx2;
		t2x += t2xp;
		y += 1;
		if (y == y2) break;
	}
next:
	// Second half
	dx1 = static_cast<int16_t>(x3 - x2); if (dx1 < 0) { dx1 = -dx1; signx1 = -1; }
	else signx1 = 1;
	dy1 = static_cast<int16_t>(y3 - y2);
	t1x = x2;

	if (dy1 > dx1) {   // swap values
		std::swap(dy1, dx1);
		changed1 = true;
	}
	else changed1 = false;

	e1 = static_cast<int16_t>(dx1 >> 1);

	for (int16_t i = 0; i <= dx1; i++) {
		t1xp = 0; t2xp = 0;
		if (t1x < t2x) { minx = t1x; maxx = t2x; }
		else { minx = t2x; maxx = t1x; }
		// process first line until y value is about to change
		while (i < dx1) {
			e1 += dy1;
			while (e1 >= dx1) {
				e1 -= dx1;
				if (changed1) { t1xp = signx1; break; }//t1x += signx1;
				else          goto next3;
			}
			if (changed1) break;
			else   	   	  t1x += signx1;
			if (i < dx1) i++;
		}
	next3:
		// process second line until y value is about to change
		while (t2x != x3) {
			e2 += dy2;
			while (e2 >= dx2) {
				e2 -= dx2;
				if (changed2) t2xp = signx2;
				else          goto next4;
			}
			if (changed2)     break;
			else              t2x += signx2;
		}
	next4:

		if (minx > t1x) minx = t1x;
		if (minx > t2x) minx = t2x;
		if (maxx < t1x) maxx = t1x;
		if (maxx < t2x) maxx = t2x;
		drawline(minx, maxx, y);
		if (!changed1) t1x += signx1;
		t1x += t1xp;
		if (!changed2) t2x += signx2;
		t2x += t2xp;
		y += 1;
		if (y > y3) return;
	}
}

void graphics::Graphics::fillRect(int32_t left, int32_t top, int32_t right, int32_t bottom, uint32_t color)
{
	for (int32_t x = left; x < right; x++)
		for (int32_t y = top; y < bottom; y++)
			drawPixel(x, y, color);
}

void graphics::Graphics::drawPixel(math::Vec2d<int32_t> coord, uint32_t color)
{
	drawPixel(coord.x, coord.y, color);
}

void graphics::Graphics::drawLine(math::Vec2d<int32_t> point1, math::Vec2d<int32_t> point2, uint32_t color)
{
	drawLine(point1.x, point1.y, point2.x, point2.y, color);
}

void graphics::Graphics::drawObjects(std::vector<Triangle<float>>& transformedTriangles, bool showFrame, bool showEdges)
{
	controls::WidgetRect canvasRect = getCanvasRect();

	// Делаем затемнение треугольников
	math::Vec3d<float> line1, line2, normal, center, lightDirection, lightPosition(config::objectPreset::lightPositionX,
		config::objectPreset::lightPositionY, config::objectPreset::lightPositionZ);
	for (auto& tri : transformedTriangles)
	{
		center = (tri.points[0] + tri.points[1] + tri.points[2]) / 3.0f;
		line1 = tri.points[1] - tri.points[0];
		line2 = tri.points[2] - tri.points[0];

		normal = line1.crossProduct(line2);
		normal.normalize();

		normal = normal - center;
		lightDirection = lightPosition - center;

		normal.normalize();
		lightDirection.normalize();

		float illumination = lightDirection.dotProduct(normal) / (lightDirection.length() * normal.length());
		illumination = (0.5f + illumination) / 2.0f;

		uint32_t b = static_cast<uint32_t>(std::roundf(static_cast<uint8_t>(tri.color) * illumination));
		uint32_t g = static_cast<uint32_t>(std::roundf(static_cast<uint8_t>(tri.color >> 8) * illumination));
		uint32_t r = static_cast<uint32_t>(std::roundf(static_cast<uint8_t>(tri.color >> 16) * illumination));

		tri.color = b + (g << 8) + (r << 16);
	}

	if (showFrame)
	{
		for (auto& tri : transformedTriangles)
		{
			math::Vec3d<int32_t> points[3];
			for (int16_t i = 0; i < 3; i++)
			{
				points[i].x = roundf(tri.points[i].x);
				points[i].y = roundf(tri.points[i].y);
			}

			drawTriangle(points, config::objectPreset::color::standartEdgesColor);
		}
	}
	else
	{
		// Начало работы алгоритма отбрасывания
		std::vector<Triangle<float>*> ptrTransformedTriangles;
		for (auto& tri : transformedTriangles)
			ptrTransformedTriangles.push_back(&tri);

		warnockAlgorithm(ptrTransformedTriangles, canvasRect.left, canvasRect.top, canvasRect.right, canvasRect.bottom, showEdges);
	}
}
