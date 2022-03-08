#pragma once
#include "nn_utils.h"
#include <iostream>
namespace NN
{
	class alignas(64) nn_accumulator
	{
	public:
		static constexpr int accumulator_size = layer_sizes[ACCUMULATOR_LAYER];
		inline static float weights[input_size][accumulator_size];
		inline static float biases[accumulator_size];

		float accumulated_output[accumulator_size];
		float output[accumulator_size];

		static void read_weights(float* weights_from_file, float* biases_from_file)
		{
			for (int row = 0, index = 0; row < input_size; row++)
			{
				for (int col = 0; col < accumulator_size; col++, index++)
				{
					weights[row][col] = weights_from_file[index];
				}
			}
			std::memcpy(biases, biases_from_file, sizeof(biases));
		}

	
		
		//accumulates the wieghts for the piece that just landed
		void set_piece_in_place(point new_position, piece_orientation piece);
		//adds the current falling piece weights to the accumulated output and returns it 
		float* calculate_output(point falling_piece_position, piece_orientation falling_piece);
		//in case rows were cleared a full recalculation of accumulator is needed
		void refresh_accumulator(const bitboard* playfield);
	};
}


