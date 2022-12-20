#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

#include <cstdint>
#include <chrono>

#include "Config.h"
#include "Vec.h"
#include "Matrix.h"
#include "Object.h"

#include "WidgetRect.h"

namespace graphics
{
	class Graphics
	{
	protected:
		uint32_t* _canvas = nullptr;

	public:
		Graphics() {};

	public:
		void drawPixel(int32_t x, int32_t y, uint32_t color);
		void drawPixel(int32_t x, int32_t y, uint32_t color, controls::WidgetRect* canvasRect);
		void drawLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t color);
		void drawPolygon(math::Vec2d<int32_t>* points, int16_t cpt, uint32_t color);
		void drawTriangle(math::Vec3d<int32_t>* points, uint32_t color);
		void fillTriangle(math::Vec3d<int32_t>* points, uint32_t color, controls::WidgetRect* canvasRect = nullptr);
		void fillRect(int32_t left, int32_t top, int32_t right, int32_t bottom, uint32_t color);

		void drawPixel(math::Vec2d<int32_t> coord, uint32_t color);
		void drawLine(math::Vec2d<int32_t> point1, math::Vec2d<int32_t> point2, uint32_t color);

		void drawObjects(std::vector<Triangle<float>>& transformedTriangles, bool showFrame, bool showEdges);

	protected:
		void drawLineBresenham(int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t color);
		bool crossingNumber(math::Vec3d<float>* points[], int16_t numberPoint, math::Vec3d<float>* checkPoint);
		bool segmentIntersection(const math::Vec3d<float>& A, const math::Vec3d<float>& B, math::Vec3d<float>& C, math::Vec3d<float>& D);
		float getZDepth(math::Vec3d<float>& A, math::Vec3d<float>& B, math::Vec3d<float>& C, math::Vec3d<float>& P);

		// algorithm deleting invisible segmentrs
		void warnockAlgorithm(const std::vector<Triangle<float>*>& transformedTriangles, float left, float top, float right, float bottom, bool showEdges);

		virtual controls::WidgetRect getCanvasRect() = 0;

		virtual void clearCanvas(int32_t canvasSize) = 0;

		virtual uint32_t& at(int32_t x, int32_t y) = 0;
		virtual uint32_t at(int32_t x, int32_t y) const = 0;

		virtual void calculateObjects(std::vector<Triangle<float>>& transformedTriangles) = 0;

		virtual bool checkBounds(int32_t x, int32_t y) const = 0;
		virtual bool checkBounds(math::Vec2d<int32_t> point) const = 0;
	};
}
#endif