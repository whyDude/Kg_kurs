#ifndef _DRAWINGAREA_H_
#define _DRAWINGAREA_H_

#include "Graphics.h"

#include "Vec.h"
#include "Matrix.h"
#include "WidgetRect.h"

namespace graphics
{
	enum class TransfromType : int16_t
	{
		Translation,
		Rotation,
		Scale
	};

	class DrawingArea : public Graphics
	{
	public:

		float PZDZ_GLOBAL_X;
		float PZDZ_GLOBAL_Y;
		float PZDZ_GLOBAL_Z;

		DrawingArea() : _areaRect(controls::WidgetRect()), _backgroundColor(0) {};
		DrawingArea(int32_t x, int32_t y, int32_t width, int32_t heignt, uint32_t backgroud);
		DrawingArea(controls::WidgetRect& areaSize, uint32_t backgroud);
		DrawingArea(const DrawingArea& area);
		DrawingArea(DrawingArea&& area) noexcept;
		~DrawingArea();

		const DrawingArea& operator=(const DrawingArea& area);

		int32_t getX() const { return _areaRect.left; }
		int32_t getY() const { return _areaRect.top; }
		int32_t getWidth() const { return _areaRect.width; }
		int32_t getHeight() const { return _areaRect.height; }
		controls::WidgetRect getRect() const { return _areaRect; }
		uint32_t* getCanvas() const { return _canvas; }
		uint32_t getBackgroundColor() const { return _backgroundColor; }
		
		void setBackgroundColor(uint32_t background) { _backgroundColor = background; }

		void updateSize(controls::WidgetRect& newAreaRect);

		void addObject(int16_t objectType, uint32_t color);
		Object<float>* getObjectByIndex(int16_t index);
		void setObjectTransform(int16_t objectIndex, TransfromType type, float transformX, float transformY, float transformZ);
		void moveObjects(int32_t moveX, int32_t moveY, int32_t moveZ);
		void rotateObjects(int32_t rotX, int32_t rotY, int32_t rotZ);
		void scaleObjects(float scaleXYZ);

		void drawAreaEdge(uint32_t color);
		void drawAreaObjects(bool showFrame, bool showEdges);

	protected:
		virtual controls::WidgetRect getCanvasRect() override { return _areaRect; }
		virtual void clearCanvas(int32_t canvasSize) override;

		virtual uint32_t& at(int32_t x, int32_t y) override;
		virtual uint32_t at(int32_t x, int32_t y) const override;

		virtual void calculateObjects(std::vector<Triangle<float>>& transformedTriangles) override;

		virtual bool checkBounds(int32_t x, int32_t y) const override;
		virtual bool checkBounds(math::Vec2d<int32_t> point) const override;

	private:
		controls::WidgetRect _areaRect;
		uint32_t _backgroundColor;

		std::vector<Object<float>> objects;
	};
}
#endif