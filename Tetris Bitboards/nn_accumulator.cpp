#include "nn_accumulator.h"
#include "tetris_bitboard.h"
#include <iostream>

namespace NN
{
	void nn_accumulator::set_piece_in_place(point new_position, piece_orientation piece)
	{
		bitboard current_piece = piece_masks[piece];
		for (int block = 0; block < blocks_per_piece; block++)
		{
			int block_index_on_bitmask = __lzcnt64(current_piece) ^ 63;
			int center_index = new_position.row * cols + new_position.col;
			int offset = block_index_on_bitmask - base_center_index;
			int current_block_index = center_index + offset;

			for (int output_neuron = 0; output_neuron < accumulator_size; output_neuron++)
			{
				accumulated_output[output_neuron] += weights[current_block_index][output_neuron];
			}
			current_piece ^= (1ULL << block_index_on_bitmask);
		}
	}

	float* nn_accumulator::calculate_output(point falling_piece_position, piece_orientation falling_piece)
	{
		std::memcpy(output, accumulated_output, sizeof(accumulated_output));
		bitboard current_piece = piece_masks[falling_piece];
		for (int block = 0; block < blocks_per_piece; block++)
		{
			int block_index_on_bitmask = __lzcnt64(current_piece) ^ 63;
			int center_index = falling_piece_position.row * cols + falling_piece_position.col;
			int offset = block_index_on_bitmask - base_center_index;
			int current_block_index = center_index + offset;
			for (int output_neuron = 0; output_neuron < accumulator_size; output_neuron++)
			{
				output[output_neuron] += weights[current_block_index + falling_piece_offset][output_neuron];
			}
			current_piece ^= (1ULL << block_index_on_bitmask);
		}
		return output;
	}

	void nn_accumulator::refresh_accumulator(const bitboard* playfield)
	{
		std::memcpy(accumulated_output, biases, sizeof(accumulated_output));

		for (int curr_bb_index = 0; curr_bb_index < tetris_bitboard::num_bitboards; curr_bb_index++)
		{
			int square_offset = 60 * curr_bb_index;
			bitboard curr_bb = playfield[curr_bb_index];
			while (curr_bb)
			{
				int block_index = (__lzcnt64(curr_bb) ^ 63);
				int feature_index = block_index + square_offset;
				for (int output_neuron = 0; output_neuron < accumulator_size; output_neuron++)
				{
					accumulated_output[output_neuron] += weights[feature_index][output_neuron];
				}
				curr_bb ^= (1ULL << block_index);
			}
		}
	}
}

