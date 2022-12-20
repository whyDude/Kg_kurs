#include "DrawingArea.h"

graphics::DrawingArea::DrawingArea(int32_t x, int32_t y, int32_t width, int32_t heignt, uint32_t backgroud)
{
	_backgroundColor = backgroud;
	_areaRect = controls::WidgetRect(x, y, width, heignt);
	_canvas = new uint32_t[_areaRect.width * _areaRect.height];
}

graphics::DrawingArea::DrawingArea(controls::WidgetRect& areaSize, uint32_t backgroud)
{
	_areaRect = areaSize;
	_backgroundColor = backgroud;
	_canvas = new uint32_t[_areaRect.width * _areaRect.height];
}

graphics::DrawingArea::DrawingArea(const DrawingArea& area)
{
	_areaRect = area._areaRect;
	_backgroundColor = area._backgroundColor;

	if (_canvas != nullptr) delete[] _canvas;
	_canvas = new uint32_t[area._areaRect.width * area._areaRect.height];

	for (int16_t i = 0; i < area._areaRect.height; ++i)
	{
		for (int16_t j = 0; j < area._areaRect.width; ++j)
		{
			_canvas[i * _areaRect.width + j] = area._canvas[i * _areaRect.width + j];
		}
	}
}

graphics::DrawingArea::DrawingArea(DrawingArea&& area) noexcept
{
	_areaRect = area._areaRect;
	_backgroundColor = area._backgroundColor;
	_canvas = area._canvas;
	area._canvas = nullptr;
}

graphics::DrawingArea::~DrawingArea()
{
	if (_canvas != nullptr)
		delete[] _canvas;
}

const graphics::DrawingArea& graphics::DrawingArea::operator=(const DrawingArea& area)
{
	_areaRect = area._areaRect;
	_backgroundColor = area._backgroundColor;

	if (_canvas != nullptr) delete[] _canvas;
	_canvas = new uint32_t[area._areaRect.width * area._areaRect.height];

	for (int16_t i = 0; i < area._areaRect.height; ++i)
	{
		for (int16_t j = 0; j < area._areaRect.width; ++j)
		{
			_canvas[i * _areaRect.width + j] = area._canvas[i * _areaRect.width + j];
		}
	}

	return *this;
}

void graphics::DrawingArea::updateSize(controls::WidgetRect& newAreaRect)
{
	_areaRect = newAreaRect;

	uint32_t* tempCanvas = new uint32_t[_areaRect.width * _areaRect.height];
	delete[] _canvas;
	_canvas = tempCanvas;
}

void graphics::DrawingArea::addObject(int16_t objectType, uint32_t color)
{
	int16_t size;
	objects.push_back(std::move(Object<float>(objectType, color)));
	size = objects.size();
	objects[size - 1].create();
}

graphics::Object<float>* graphics::DrawingArea::getObjectByIndex(int16_t index)
{
	if (index > objects.size() || index < 0) return nullptr;
	return &objects[index];
}

void graphics::DrawingArea::setObjectTransform(int16_t objectIndex, TransfromType type, float transformX, float transformY, float transformZ)
{
	if (objectIndex > objects.size() || objectIndex < 0) return;

	switch (type)
	{
	case graphics::TransfromType::Translation:
		objects[objectIndex].translate(transformX, transformY, transformZ);
		break;
	case graphics::TransfromType::Rotation:
		objects[objectIndex].rotate(transformX, transformY, transformZ);
		break;
	case graphics::TransfromType::Scale:
		objects[objectIndex].scale(transformX, transformY, transformZ);
		break;
	default:
		break;
	}
}

void graphics::DrawingArea::moveObjects(int32_t moveX, int32_t moveY, int32_t moveZ)
{
	for (auto& object : objects)
		object.setUserTranslateXYZ(moveX, moveY, moveZ);
}

void graphics::DrawingArea::rotateObjects(int32_t rotX, int32_t rotY, int32_t rotZ)
{
	for (auto& object : objects)
		object.setUserRotateXYZ(rotX, rotY, rotZ);
}

void graphics::DrawingArea::scaleObjects(float scaleXYZ)
{
	for (auto& object : objects)
		object.setUserScaleXYZ(scaleXYZ);
}

void graphics::DrawingArea::drawAreaEdge(uint32_t color)
{
	math::Vec2d<int32_t> points[] = { {_areaRect.left, _areaRect.top }, {_areaRect.right - 1, _areaRect.top}, 
		{_areaRect.right - 1, _areaRect.bottom - 1 }, {_areaRect.left, _areaRect.bottom - 1 } };

	drawPolygon(points, 4, config::color::areaBounds);
}

void graphics::DrawingArea::drawAreaObjects(bool showFrame, bool showEdges)
{
	clearCanvas(_areaRect.width * _areaRect.height);

	if (objects.size() == 0) return;

	std::vector<Triangle<float>> transformedTriangles;
	calculateObjects(transformedTriangles);
	drawObjects(transformedTriangles, showFrame, showEdges);
}

void graphics::DrawingArea::calculateObjects(std::vector<Triangle<float>>& transformedTriangles)
{
	// Выбор матрицы проекции
	math::Matrix<float>  matProjection;
	matProjection.projectionPerspective(90.0f, static_cast<float>(_areaRect.height) / _areaRect.width, 0.1f, 1000.0f);

	float scaleX = _areaRect.width;
	float scaleY = _areaRect.height;
	float scaleZ = 1.0f;

	//math::Vec3d<float> light(config::objectPreset::lightPositionX, config::objectPreset::lightPositionY, config::objectPreset::lightPositionZ);
	math::Vec3d<float> light(PZDZ_GLOBAL_X, PZDZ_GLOBAL_Y, PZDZ_GLOBAL_Z);

	math::Matrix<float> centeringMatrix;
	centeringMatrix.init();
	centeringMatrix.translation(_areaRect.width / 2, _areaRect.height / 2, 10.0f);

	math::Matrix<float> centeringShadowMatrix;
	centeringShadowMatrix.init();
	centeringShadowMatrix.translation(_areaRect.width / 2, _areaRect.height * 0.90f, 10.0f);

	int16_t i = 0;
	for (auto& obj : objects)
	{
		float standartPositionX = (i == 0) ? config::objectPreset::standartObject1PositionX : config::objectPreset::standartObject2PositionX;
		float standartPositionY = (i == 0) ? config::objectPreset::standartObject1PositionY : config::objectPreset::standartObject2PositionY;
		float standartPositionZ = (i == 0) ? config::objectPreset::standartObject1PositionZ : config::objectPreset::standartObject2PositionZ;

		// Устанавливаем начальные преобразования
		setObjectTransform(i, graphics::TransfromType::Translation, standartPositionX, standartPositionY, standartPositionZ);
		setObjectTransform(i, graphics::TransfromType::Scale, scaleX, scaleY, scaleZ);
		setObjectTransform(i, graphics::TransfromType::Rotation, config::objectPreset::standartRotationX, config::objectPreset::standartRotationY, config::objectPreset::standartRotationZ);


		for (auto& tri : obj.faces.triangles)
		{
			Triangle<float>  triTransformed;

			Triangle<float> shadowFace;
			
			triTransformed = tri * obj.getTransform();
			triTransformed = triTransformed * matProjection;

			for (int16_t j = 0; j < 3; ++j)
			{
				triTransformed.points[j] = triTransformed.points[j] / (triTransformed.points[j].w * 0.1f);
			}

			triTransformed = triTransformed * centeringMatrix;

			triTransformed.setColor(obj.color);
			transformedTriangles.push_back(triTransformed);
			transformedTriangles.back().objectId = obj.objectId;

			for (int16_t i = 0; i < 3; ++i)
			{
				shadowFace.points[i].x = triTransformed.points[i].x + (static_cast<float>(_areaRect.height) - triTransformed.points[i].y)
					* (triTransformed.points[i].x - light.x) / (triTransformed.points[i].y - light.y);
				shadowFace.points[i].z = triTransformed.points[i].z + (static_cast<float>(_areaRect.height) - triTransformed.points[i].y)
					* (triTransformed.points[i].z - light.z) / (triTransformed.points[i].y - light.y);
				shadowFace.points[i].y = 1.1f * static_cast<float>(_areaRect.height) - shadowFace.points[i].z * 5.0f;
			}

			shadowFace.setColor(config::objectPreset::color::standartShadowColor);
			transformedTriangles.push_back(shadowFace);
			transformedTriangles.back().objectId = obj.objectId;
		}
		++i;
	}
}

void graphics::DrawingArea::clearCanvas(int32_t canvasSize)
{
	for (int32_t i = 0; i < canvasSize; ++i)
	{
		_canvas[i] = _backgroundColor;
	}
}

uint32_t& graphics::DrawingArea::at(int32_t x, int32_t y)
{
	if (!checkBounds(x, y))
		return _canvas[0];
	return _canvas[(y - _areaRect.top) * _areaRect.width + (x - _areaRect.left)];
}

uint32_t graphics::DrawingArea::at(int32_t x, int32_t y) const
{
	if (!checkBounds(x, y))
		return _canvas[0];
	return _canvas[(y - _areaRect.top) * _areaRect.width + (x - _areaRect.left)];
}


bool graphics::DrawingArea::checkBounds(int32_t x, int32_t y) const
{
	if ((x >= _areaRect.left) && (x <= _areaRect.right - 1) &&
		(y >= _areaRect.top) && (y <= _areaRect.bottom - 1))
		return true;
	return false;
}

bool graphics::DrawingArea::checkBounds(math::Vec2d<int32_t> point) const
{
	if ((point.x >= _areaRect.left) && (point.x <= _areaRect.right - 1) &&
		(point.y >= _areaRect.top) && (point.y <= _areaRect.bottom - 1))
		return true;
	return false;
}