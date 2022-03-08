#pragma once
#include "utils.h"

namespace NN
{
	//includes output, does not include input
	inline constexpr int num_layers = 4;

	//input gets repeated this many times on the input layer
	inline constexpr int input_repetitions = 1;
	inline constexpr int input_planes = 2;
	inline constexpr int falling_piece_offset = rows * cols;
	inline constexpr int input_size = rows * cols * input_planes * input_repetitions;

	enum layer_types
	{
		ACCUMULATOR_LAYER,
		FIRST_HIDDEN_LAYER,
		SECOND_HIDDEN_LAYER,
		OUTPUT_LAYER
	};

	inline constexpr int layer_sizes[num_layers] =
	{
		1024,
		1024,
		1024,
		4
	};
}