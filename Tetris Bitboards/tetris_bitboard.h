#pragma once
#include <cstdint>

#include "utils.h"
#include "nn_accumulator.h"

class tetris_bitboard
{
public:
	inline static constexpr int rows_per_bb = 6;
	inline static constexpr int num_bitboards = 4;
	inline static constexpr int bits_per_bb = rows_per_bb * cols;
	tetris_bitboard();
	void on_frame(unsigned int input);
	void new_game();
	void print_board();

	bool is_over() const { return game_over; }
	const bitboard* get_bb() const { return bbs; }
	NN::nn_accumulator& get_acc() { return acc; }
	const point get_falling_piece_center() const { return falling_piece_center; }
	piece_orientation get_piece_orientation() const { return falling_piece_orientation; }
	int get_score() const { return (__popcnt64(bbs[0]) + __popcnt64(bbs[1]) + __popcnt64(bbs[2]) + __popcnt64(bbs[3])) + line_count * rows * cols; }
private:
	//bit scan forward
	int bsf();
	//pops the bit in the index 
	int pop(int index);
	//checks if the piece can be placed in the given coords
	template<bool set>
	bool check_falling_piece(piece_orientation piece_id, point piece_coords);
	//population count for a row of given index
	int popcount_row(int row_index);

	void clear_row(int row_index);
	bitboard			bbs[num_bitboards];
	point				falling_piece_center;
	piece_orientation	falling_piece_orientation;
	piece_type			falling_piece_type;


	int line_count;
	int frame_count;
	int spawn_count;
	bool game_over;

	NN::nn_accumulator acc;

	void spawn_piece();
	unsigned int rng_value = 0xFFFF;// &__rdtsc() & _rdseed16_step();
	inline unsigned int rng() {
		return rng_value = ((((rng_value >> 9) & 1) ^ ((rng_value >> 1) & 1)) << 15) | (rng_value >> 1);
	}


	
};

