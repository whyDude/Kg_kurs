#ifndef _PRESETS_H_
#define _PRESETS_H_

#include "Vec.h"
#include <vector>

namespace graphics
{
	struct ConnectedVertices
	{
	public:
		int32_t vertices[3];
	};

	namespace presets
	{
		extern std::vector<math::Vec3d<float>> verticesPresetCube;
		extern std::vector<ConnectedVertices> connectedVerticesCube;

		extern std::vector<math::Vec3d<float>> verticesPresetPyramid3;
		extern std::vector<ConnectedVertices> connectedVerticesPyramid3;
	}
}

#endif // !_PRESETS_H_
