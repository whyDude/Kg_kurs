#include "Presets.h"

namespace graphics
{
	namespace presets
	{
		// �������� 3
		std::vector<math::Vec3d<float>> verticesPresetPyramid3 = {
			{ 0.0f, -1.0f,  0.0f},
			{-1.0f,  1.0f, -1.0f},
			{ 1.0f,  1.0f, -1.0f},
			{-1.0f,  1.0f,  1.0f}
		};

		std::vector<ConnectedVertices> connectedVerticesPyramid3 = {
			{2,0,1},			// �����
			{1,0,3},			// ����
			{3,0,2},			// �����
			{2,1,3}				// ���
		};


		// ���
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
			{0,1,2}, {0,2,3},	// �����
			{4,5,1}, {4,1,0},	// ����
			{3,2,6}, {3,6,7},	// �����
			{7,6,5}, {7,5,4},	// ���
			{4,0,3}, {4,3,7},	// ���
			{1,5,6}, {1,6,2}	// �����
		};
	}
}