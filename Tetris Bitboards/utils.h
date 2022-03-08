#pragma once
#include<algorithm>
using bitboard = uint64_t;

struct point
{
	int row;
	int col;
};

enum piece_orientation
{
	PIECE_T_UP,
	PIECE_T_RIGHT,
	PIECE_T_DOWN,
	PIECE_T_LEFT,

	PIECE_J_LEFT,
	PIECE_J_UP,
	PIECE_J_RIGHT,
	PIECE_J_DOWN,

	PIECE_Z_HORIZONTAL,
	PIECE_Z_VERTICAL,

	PIECE_O_DEFAULT,

	PIECE_S_HORIZONTAL,
	PIECE_S_VERTICAL,

	PIECE_L_RIGHT,
	PIECE_L_DOWN,
	PIECE_L_LEFT,
	PIECE_L_UP,

	PIECE_I_HORIZONTAL,
	PIECE_I_VERTICAL,

	PIECE_NONE_ORIENTATION
};

enum piece_type
{
	PIECE_T,
	PIECE_J,
	PIECE_Z,
	PIECE_O,
	PIECE_S,
	PIECE_L,
	PIECE_I,
	PIECE_NONE
};

enum tile_type
{
	TILE_EMPTY,
	TILE_FALLING,
	TILE_FALLEN,
	TILE_NONE
};

enum input_type
{
	INPUT_LEFT = 1,
	INPUT_RIGHT = 2,
	INPUT_SPIN_LEFT = 4,
	INPUT_SPIN_RIGHT = 8
};

inline constexpr int blocks_per_piece = 4;
inline constexpr int rows = 22;
inline constexpr int cols = 10;

inline constexpr int falling_piece_square_rows = 5;
inline constexpr int falling_piece_square_cols = 5;
inline constexpr int base_center_index = (2 * cols) + 2;



inline constexpr point piece_block_coordinates[PIECE_NONE_ORIENTATION][blocks_per_piece] = 
{
		{{ -1,  0 }, {  0,  0 }, {  1,  0 }, {  0, -1 }}, //PIECE_T_UP
		{{  0, -1 }, {  0,  0 }, {  1,  0 }, {  0,  1 }}, //PIECE_T_RIGHT
		{{ -1,  0 }, {  0,  0 }, {  1,  0 }, {  0,  1 }}, //PIECE_T_LEFT
		{{  0, -1 }, { -1,  0 }, {  0,  0 }, {  0,  1 }}, //PIECE_T_DOWN

		{{  0, -1 }, {  0,  0 }, { -1,  1 }, {  0,  1 }}, //PIECE_J_LEFT
		{{ -1, -1 }, { -1,  0 }, {  0,  0 }, {  1,  0 }}, //PIECE_J_UP
		{{  0, -1 }, {  1, -1 }, {  0,  0 }, {  0,  1 }}, //PIECE_J_RIGHT
		{{ -1,  0 }, {  0,  0 }, {  1,  0 }, {  1,  1 }}, //PIECE_J_DOWN

		{{ -1,  0 }, {  0,  0 }, {  0,  1 }, {  1,  1 }}, //PIECE_Z_HORIZONTAL
		{{  1, -1 }, {  0,  0 }, {  1,  0 }, {  0,  1 }}, //PIECE_Z_VERTICAL

		{{ -1,  0 }, {  0,  0 }, { -1,  1 }, {  0,  1 }}, //PIECE_O

		{{  0,  0 }, {  1,  0 }, { -1,  1 }, {  0,  1 }}, //PIECE_S_HORIZONTAL
		{{  0, -1 }, {  0,  0 }, {  1,  0 }, {  1,  1 }}, //PIECE_S_VERTICAL

		{{  0, -1 }, {  0,  0 }, {  0,  1 }, {  1,  1 }}, //PIECE_L_RIGHT
		{{ -1,  0 }, {  0,  0 }, {  1,  0 }, { -1,  1 }}, //PIECE_L_DOWN
		{{ -1, -1 }, {  0, -1 }, {  0,  0 }, {  0,  1 }}, //PIECE_L_LEFT
		{{  1, -1 }, { -1,  0 }, {  0,  0 }, {  1,  0 }}, //PIECE_L_UP

		{{  0, -2 }, {  0, -1 }, {  0,  0 }, {  0,  1 }}, //PIECE_Z_HORIZONTAL
		{{ -2,  0 }, { -1,  0 }, {  0,  0 }, {  1,  0 }}  //PIECE_Z_VERTICAL
};

//assigns piece_type to each piece_orientation
inline constexpr piece_type piece_orientation_types[PIECE_NONE_ORIENTATION] =
{
	PIECE_T,
	PIECE_T,
	PIECE_T,
	PIECE_T,

	PIECE_J,
	PIECE_J,
	PIECE_J,
	PIECE_J,

	PIECE_Z,
	PIECE_Z,

	PIECE_O,

	PIECE_S,
	PIECE_S,

	PIECE_L,
	PIECE_L,
	PIECE_L,
	PIECE_L,

	PIECE_I,
	PIECE_I,
};
//denotes which indices of the piece_block_coordinates array belong to the current piece type
inline constexpr std::pair<int,int> piece_indices_boundaries[PIECE_NONE] = 
{
	{0,3},
	{4,7},
	{8,9},
	{10,10},
	{11,12},
	{13,16},
	{17,18}
};
//contains the default orientation for each of the piece types
inline constexpr piece_orientation default_piece_orientations[PIECE_NONE] =
{
	PIECE_T_DOWN,
	PIECE_J_DOWN,
	PIECE_Z_HORIZONTAL,
	PIECE_O_DEFAULT,
	PIECE_S_HORIZONTAL,
	PIECE_L_DOWN,
	PIECE_I_HORIZONTAL
};
//heights relative to the center, to the bottom and to the top
inline constexpr std::pair<int, int> piece_heights[PIECE_NONE_ORIENTATION] =
{
	{-1,1},
	{0,1},
	{-1,1},
	{-1,0},
	{-1,0},
	{-1,1},
	{0,1},
	{-1,1},
	{-1,1},
	{0,1},
	{-1,0},
	{-1,1},
	{0,1},
	{0,1},
	{-1,1},
	{-1,0},
	{-1,1},
	{0,0},
	{-2,1},
};
//widths relative to the center, to the right and to the left of the center
//for example, a piece can't be put in a place, where center + width.first would be negative
inline constexpr std::pair<int, int> piece_widths[PIECE_NONE_ORIENTATION] =
{
	{-1,0},
	{-1,1},
	{0,1},
	{-1,1},
	{-1,1},
	{-1,0},
	{-1,1},
	{0,1},
	{0,1},
	{-1,1},
	{0,1},
	{0,1},
	{-1,1},
	{-1,1},
	{0,1},
	{-1,1},
	{-1,0},
	{-2,1},
	{0,0},
};

inline constexpr bitboard piece_masks[PIECE_NONE_ORIENTATION] = 
{
4301262848,
4309647360,
4307554304,
14684160,
14688256,
4299167744,
2162163712,
12889100288,
8602521600,
6455033856,
12595200,
4307558400,
12891193344,
8604614656,
4299173888,
14682112,
6446649344,
15728640,
4299165700,
};

inline constexpr bitboard bottom_row_masks[falling_piece_square_rows] = {
18446744073709551584,
18446744073709519840,
18446744073677013984,
18446744040391017440,
18446709955530556384,
};

inline constexpr bitboard upper_row_masks[falling_piece_square_rows] = {
34084860461056,
34118146457600,
34118178963456,
34118178995200,
34118178995231,
};

inline constexpr int bitboard_index[rows] = {
0,
0,
0,
0,
0,
0,
1,
1,
1,
1,
1,
1,
2,
2,
2,
2,
2,
2,
3,
3,
3,
3,
};


inline constexpr int row_in_bb_index[rows] =
{
		0,
		1,
		2,
		3,
		4,
		5,
		0,
		1,
		2,
		3,
		4,
		5,
		0,
		1,
		2,
		3,
		4,
		5,
		0,
		1,
		2,
		3,
};

inline constexpr bitboard row_template[rows] =
{
		1023,
		1047552,
		1072693248,
		1098437885952,
		1124800395214848,
		1151795604700004352,
		1023,
		1047552,
		1072693248,
		1098437885952,
		1124800395214848,
		1151795604700004352,
		1023,
		1047552,
		1072693248,
		1098437885952,
		1124800395214848,
		1151795604700004352,
		1023,
		1047552,
		1072693248,
		1098437885952,
};

inline constexpr bitboard num_rows_masks[rows] =
{
		1023,
		1048575,
		1073741823,
		1099511627775,
		1125899906842623,
		1152921504606846975,
};