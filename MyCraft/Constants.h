#ifndef CONSTANTS_H
#define CONSTANTS_H

namespace CONSTANTS {
	constexpr int WINDOW_HEIGHT = 1060;
	constexpr int WINDOW_WIDTH = 1920;

	constexpr size_t Dimension_1DSize = 16;
	constexpr size_t Dimension_2DSize = 256;
	constexpr size_t Dimension_3DSize = 4096;
	constexpr size_t MAX_BLOCK_HEIGHT = 255ull;

	constexpr size_t MODELS_UBO_BINDING_POINT = 0;
	constexpr size_t VP_UBO_BINDING_POINT = 1;
	constexpr size_t SSAO_KERNELS_BINDING_POINT = 2;
}

#endif // CONSTANTS_H