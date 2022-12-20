#include "Presets.h"

namespace graphics
{
	namespace presets
	{
		// Пирамида 3
		std::vector<math::Vec3d<float>> verticesPresetPyramid3 = {
			{ 0.0f, -1.0f,  0.0f},
			{-1.0f,  1.0f, -1.0f},
			{ 1.0f,  1.0f, -1.0f},
			{-1.0f,  1.0f,  1.0f}
		};

		std::vector<ConnectedVertices> connectedVerticesPyramid3 = {
			{2,0,1},			// перед
			{1,0,3},			// лево
			{3,0,2},			// право
			{2,1,3}				// низ
		};


		// Куб
		std::vector<math::Vec3d<float>> verticesPresetCube = {
			{-1.0f, -1.0f, -1.0f},
			{-1.0f,  1.0f, -1.0f},
			{ 1.0f,  1.0f, -1.0f},
			{ 1.0f, -1.0f, -1.0f},
			{-1.0f, -1.0f,  2.0f},
			{-1.0f,  1.0f,  2.0f},
			{ 1.0f,  1.0f,  2.0f},
			{ 1.0f, -1.0f,  2.0f}
		};

		std::vector<ConnectedVertices> connectedVerticesCube = {
			{0,1,2}, {0,2,3},	// перед
			{4,5,1}, {4,1,0},	// лево
			{3,2,6}, {3,6,7},	// право
			{7,6,5}, {7,5,4},	// зад
			{4,0,3}, {4,3,7},	// низ
			{1,5,6}, {1,6,2}	// вверх
		};
	}
}