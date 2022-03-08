#pragma once
#include "nn_utils.h"
#include "linear_layer.h"
#include "tetris_bitboard.h"

#include <string>

namespace NN
{


	class nn
	{
	public:	
		using first_hidden_layer_type	=	linear_layer<layer_sizes[ACCUMULATOR_LAYER], layer_sizes[FIRST_HIDDEN_LAYER]>;
		using second_hidden_layer_type	=	linear_layer<layer_sizes[FIRST_HIDDEN_LAYER], layer_sizes[SECOND_HIDDEN_LAYER]>;
		using output_layer_type			=	linear_layer < layer_sizes[SECOND_HIDDEN_LAYER], layer_sizes[OUTPUT_LAYER ] > ;

		float* calculate_moves(tetris_bitboard& playfield);
		void read_weights(const std::string& filename);
	private:
		first_hidden_layer_type first_layer;
		second_hidden_layer_type second_layer;
		output_layer_type output_layer;
	};

}