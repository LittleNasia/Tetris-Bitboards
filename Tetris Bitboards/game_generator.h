#pragma once
#include "nn.h"

struct position
{
	bitboard bb[4];
	point piece_center;
	piece_orientation falling_piece_type;
	int move_played;
};

class game_generator
{
public:
	static void generate_games();
};

