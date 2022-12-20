#ifndef _OBJECT_H_
#define _OBJECT_H_

#include "Matrix.h"
#include "Presets.h"
#include "Config.h"

#include <list>
#include <vector>
#include <string>
#include <cstdint>
#include <algorithm>

namespace graphics
{
	namespace type
	{
		enum ObjectType
		{
			PYRAMID3 = 0,
			CUBE = 1,
		};
	}

	template <class T>
	class Triangle
	{
	public:
		math::Vec3d<T> points[3];
		uint32_t color{};
		int16_t objectId{};

	public:
		Triangle() {};
		Triangle(T x1, T y1, T z1, T x2, T y2, T z2, T x3, T y3, T z3, uint32_t newColor = 0)
		{
			points[0].x = x1; points[0].y = y1; points[0].z = z1;
			points[1].x = x2; points[1].y = y2; points[1].z = z2;
			points[2].x = x3; points[2].y = y3; points[2].z = z3;
			color = newColor;
		}
		Triangle(const math::Vec2d<T>& p1, const math::Vec2d<T>& p2, const math::Vec2d<T>& p3, uint32_t newColor = 0)
		{
			points[0] = p1;
			points[1] = p2;
			points[2] = p3;
			color = newColor;
		}
		Triangle(const math::Vec3d<T>& p1, const math::Vec3d<T>& p2, const math::Vec3d<T>& p3, uint32_t newColor = 0)
		{
			points[0] = p1;
			points[1] = p2;
			points[2] = p3;
			color = newColor;
		}
		Triangle(const Triangle& triangle)
		{
			points[0] = triangle.points[0];
			points[1] = triangle.points[1];
			points[2] = triangle.points[2];
			color = triangle.color;
		}
		Triangle(Triangle&& triangle) noexcept
		{
			points[0] = triangle.points[0];
			points[1] = triangle.points[1];
			points[2] = triangle.points[2];
			color = triangle.color;
			objectId = triangle.objectId;
		}

		Triangle& operator=(const Triangle& triangle)
		{
			points[0] = triangle.points[0];
			points[1] = triangle.points[1];
			points[2] = triangle.points[2];
			color = triangle.color;
			return *this;
		}

		Triangle operator*(const math::Matrix<T>& mat)
		{
			math::Vec3d<T> p1 = mat * points[0];
			math::Vec3d<T> p2 = mat * points[1];
			math::Vec3d<T> p3 = mat * points[2];

			return Triangle(p1, p2, p3, color);
		}

		uint32_t getColor() { return color; }
		void setColor(uint32_t newColor) { color = newColor; }

		uint32_t getObjectId() { return objectId; }
		void setObjectId(int16_t nobjectId) { objectId = nobjectId; }
	};

	template <class T>
	class Shape
	{
	public:
		std::vector<Triangle<T>> triangles;

	public:
		Shape() {}
		Shape(const Shape& shape) { triangles = shape.triangles; }
		Shape(Shape&& shape) noexcept { triangles = shape.triangles; }

		Shape& operator=(const Shape shape) { triangles = shape.triangles; return *this; }
	};


	template <class T>
	class Object
	{
	public:
		Object() : type(-1), color(0) { objectId = _idCounter++; for (auto& tri : faces.triangles) tri.setObjectId(objectId); }
		Object(int16_t objectType, uint32_t objectColor) : type(objectType), color(objectColor) 
		{ objectId = _idCounter++; for (auto& tri : faces.triangles) tri.setObjectId(objectId); }

		Object(const Object& obj);
		Object(Object&& obj) noexcept;

		void create();
		math::Matrix<T> getTransform() const;

		void translate(T xShift, T yShift, T zShift);
		void rotate(T xTheta, T yTheta, T zTheta);
		void scale(T xScale, T yScale, T zScale);

		void setUserTranslateXYZ(T moveX, T moveY, T moveZ);
		void setUserRotateXYZ(T rotX, T rotY, T rotZ);
		void setUserScaleXYZ(T scaleXYZ);

		uint32_t getColor() { return color; }
		void setColor(uint32_t newColor) { color = newColor; }

	private:
		std::vector<ConnectedVertices> connectedVertices;

		math::Matrix<T> translateMat;
		math::Matrix<T> rotateMatX, rotateMatY, rotateMatZ;
		math::Matrix<T> scaleMat;

		math::Vec3d<T> userTranslateValue{};
		math::Vec3d<T> userRotateValue{};
		T userScaleValue{};

	public:
		Shape<T> faces;
		std::vector<math::Vec3d<T>> vertices;
		int16_t type;
		uint32_t color;
		int16_t objectId;

	private:
		static int16_t _idCounter;
	};

	template<class T>
	inline Object<T>::Object(const Object& obj)
	{
		type = obj.type;
		color = obj.color;
		objectId = _idCounter++;

		connectedVertices = obj.connectedVertices;

		translateMat = obj.translateMat;
		rotateMatX = obj.rotateMatX;
		rotateMatY = obj.rotateMatY;
		rotateMatZ = obj.rotateMatZ;
		scaleMat = obj.scaleMat;

		userTranslateValue = obj.userTranslateValue;
		userRotateValue = obj.userRotateValue;
		userScaleValue = obj.userScaleValue;

		faces = obj.faces;
		vertices = obj.vertices;

		for (auto& tri : faces.triangles)
			tri.setObjectId(objectId);
	}

	template<class T>
	inline Object<T>::Object(Object&& obj) noexcept
	{
		type = obj.type;
		color = obj.color;
		objectId = obj.objectId;

		connectedVertices = obj.connectedVertices;

		translateMat = obj.translateMat;
		rotateMatX = obj.rotateMatX;
		rotateMatY = obj.rotateMatY;
		rotateMatZ = obj.rotateMatZ;
		scaleMat = obj.scaleMat;

		userTranslateValue = obj.userTranslateValue;
		userRotateValue = obj.userRotateValue;
		userScaleValue = obj.userScaleValue;

		faces = obj.faces;
		vertices = obj.vertices;
	}

	template<class T>
	inline void Object<T>::create()
	{
		switch (type)
		{
		case type::PYRAMID3:
		{
			vertices = presets::verticesPresetPyramid3;
			connectedVertices = presets::connectedVerticesPyramid3;
			break;
		}
		case type::CUBE:
		{
			vertices = presets::verticesPresetCube;
			connectedVertices = presets::connectedVerticesCube;
			break;
		}
		default:
			throw "Unknown figure!\n";
			break;
		}

		// Составляем сетку объекта
		for (int16_t i = 0; i < connectedVertices.size(); ++i)
		{
			faces.triangles.push_back(
				Triangle<T>(
					vertices[connectedVertices[i].vertices[0]],
					vertices[connectedVertices[i].vertices[1]],
					vertices[connectedVertices[i].vertices[2]]
				)
			);
			faces.triangles[i].setObjectId(objectId);
		}
	}

	template<class T>
	inline math::Matrix<T> Object<T>::getTransform() const
	{
		return rotateMatX * rotateMatY * rotateMatZ * scaleMat * translateMat;
	}

	template<class T>
	inline void Object<T>::translate(T xShift, T yShift, T zShift)
	{
		translateMat.translation(xShift + userTranslateValue.x, yShift + userTranslateValue.y, zShift + userTranslateValue.z);
	}

	template<class T>
	inline void Object<T>::rotate(T xTheta, T yTheta, T zTheta)
	{
		rotateMatX.rotateX(xTheta + userRotateValue.x);
		rotateMatY.rotateY(yTheta + userRotateValue.y);
		rotateMatZ.rotateZ(zTheta + userRotateValue.z);
	}

	template<class T>
	inline void Object<T>::scale(T xScale, T yScale, T zScale)
	{
		scaleMat.scale(xScale * (config::objectPreset::standartScalingX + userScaleValue),
			yScale * (config::objectPreset::standartScalingY + userScaleValue),
			zScale * (config::objectPreset::standartScalingZ + userScaleValue));
	}

	template<class T>
	inline void Object<T>::setUserTranslateXYZ(T moveX, T moveY, T moveZ)
	{
		userTranslateValue.x += moveX;
		userTranslateValue.y += moveY;
		userTranslateValue.z += moveZ;
	}

	template<class T>
	inline void Object<T>::setUserScaleXYZ(T scaleXYZ)
	{
		userScaleValue += scaleXYZ;
	}

	template<class T>
	inline void Object<T>::setUserRotateXYZ(T rotX, T rotY, T rotZ)
	{
		userRotateValue.x = static_cast<int32_t>(userRotateValue.x + rotX) % 360;
		userRotateValue.y = static_cast<int32_t>(userRotateValue.y + rotY) % 360;
		userRotateValue.z = static_cast<int32_t>(userRotateValue.z + rotZ) % 360;
	}

	template<class T>
	int16_t graphics::Object<T>::_idCounter = 0;
}
#endif // !_OBJECT_H_
