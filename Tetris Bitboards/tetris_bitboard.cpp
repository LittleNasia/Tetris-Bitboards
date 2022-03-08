#include "tetris_bitboard.h"
#include <iostream>
#include <intrin.h>

tetris_bitboard::tetris_bitboard() : spawn_count(0), game_over(false), frame_count(0), line_count(0)
{
	new_game();
}

void tetris_bitboard::new_game()
{
	
	game_over = false;
	spawn_count = 0;
	line_count = 0;
	frame_count = 0;
	std::memset(bbs, 0, sizeof(bbs));
	falling_piece_orientation = PIECE_T_UP;
	falling_piece_type = PIECE_T;
	spawn_piece();
	acc.refresh_accumulator(bbs);
}

void tetris_bitboard::spawn_piece()
{
	falling_piece_center = { rows - 2, 5 };
	spawn_count++;
	//index contains the current piece type
	int index = (rng() & 0xFF00) >> 8;
	index += spawn_count;
	index &= 7;
	if (index == 7 || (((piece_type)index) == piece_orientation_types[falling_piece_orientation]))
	{
		//extract high byte of the random value
		index = (rng() & 0xFF00) >> 8;
		index &= 7;
		index += spawn_count;
		index %= 7;
	}
	falling_piece_orientation = default_piece_orientations[index];
	falling_piece_type = (piece_type)index;
	if (!check_falling_piece<false>(falling_piece_orientation, falling_piece_center))
	{
		game_over = true;
	}
}

template<bool set>
bool tetris_bitboard::check_falling_piece(piece_orientation piece_id, point piece_coords)
{
	//if the piece is in illegal position, stop checking
	if ((piece_coords.row + piece_heights[piece_id].first < 0) || (piece_coords.row + piece_heights[piece_id].second >= rows) ||
		(piece_coords.col + piece_widths[piece_id].first < 0) || (piece_coords.col + piece_widths[piece_id].second >= cols))
	{
		return false;
	}
	//check which bitboard the center of the piece belongs to
	int curr_bitboard_index = bitboard_index[piece_coords.row];
	//index of the center of the current piece in the current bitboard we're using
	int center_index = row_in_bb_index[piece_coords.row] * cols + piece_coords.col;
	
	//iterate over piece indices of the current piece type
	bitboard curr_piece_bb = piece_masks[piece_id];
	while (curr_piece_bb)
	{
		int curr_bitboard_index_offset = 0;
		//get one of the positions of the blocks in current piece
		int index = _lzcnt_u64(curr_piece_bb) ^ 63;
		//calculate the index relative to the base center
		int index_relative = index - base_center_index;	
		//calculate the index relative to the center of the current piece
		int index_relative_curr_center = center_index + index_relative;
		

		//if the relative index is lower than 0, that means we go to the bitboard below
		if (index_relative_curr_center < 0)
		{
			index_relative_curr_center = bits_per_bb + index_relative_curr_center;
			curr_bitboard_index_offset = -1;
		}
		//it's also possible we might've ran out of bits in current bitboard
		//in which case we go to the bitboard above
		else if (index_relative_curr_center >= bits_per_bb)
		{
			index_relative_curr_center = index_relative_curr_center - bits_per_bb;
			curr_bitboard_index_offset = 1;
		}
		//depending on our goals, we either check if it's possible for piece to exist there
		//or we set the piece in place
		if (set)
		{
			bbs[curr_bitboard_index + curr_bitboard_index_offset] ^= (1ULL << index_relative_curr_center);
			
		}
		else
		{
			if (bbs[curr_bitboard_index + curr_bitboard_index_offset] & (1ULL << index_relative_curr_center))
			{
				return false;
			}
		}
		curr_piece_bb ^= (1ULL << index);
	}
	//check if rows need to be cleared
	if (set)
	{
		int cleared_rows = 0;
		for (int row_offset = piece_heights[piece_id].first; row_offset <= piece_heights[piece_id].second; row_offset++)
		{
			//if we cleared the row below, then the current row will go down by 1, and so we have to decrement the index
			int row_to_check = piece_coords.row + row_offset - cleared_rows;
			if (popcount_row(row_to_check) == 10)
			{
				clear_row(row_to_check);
				cleared_rows++;
				line_count++;
			}
		}
		if (cleared_rows)
		{
			acc.refresh_accumulator(bbs);
		}
		else
		{
			acc.set_piece_in_place(piece_coords, falling_piece_orientation);
		}
		spawn_piece();
	}
	return true;
}
void tetris_bitboard::clear_row(int row_index)
{
	//check which bitboard the row belongs to
	int curr_bitboard_index = bitboard_index[row_index];
	int row_bb_index = row_in_bb_index[row_index];
	bitboard curr_bb = bbs[curr_bitboard_index];
	//get everything above current row 
	//num_rows_masks[row_index] returns a mask of everything below and including current row, we want everything above
	bitboard mask_above_row = (~num_rows_masks[row_index]) & curr_bb;
	//shift the mask one row to the bottom
	mask_above_row >>= cols;
	//the current bitboard is everything above the row shifted down plus everything below intact
	bbs[curr_bitboard_index] = (curr_bb & (num_rows_masks[row_index - 1])) | mask_above_row;

	//now for each of the bitboards above our current bitboard, we copy the lowest row down to the top of the current bitboard
	//next we shift all rows down by one
	for (int bb_index = curr_bitboard_index + 1; bb_index < num_bitboards; bb_index++)
	{
		//get the lowest row of the next bitboard
		bitboard lowest_row = bbs[bb_index] & num_rows_masks[0];
		//shift it up by 5 rows so that it becomes the highest row
		lowest_row <<= (5 * cols);
		//add it up to the lower bitboard
		bbs[bb_index - 1] |= lowest_row;
		//remove it from the current bitboard
		bbs[bb_index] &= ~lowest_row;
		//shift the current bitboard one row down;
		bbs[bb_index] >>= cols;
	}
}
void tetris_bitboard::on_frame(unsigned int input)
{
	if (game_over)
	{
		return;
	}
	frame_count++;
	//check if required spin is possible
	//piece O can't be spun, and so we don't check for it 
	if (falling_piece_type != PIECE_O)
	{
		if (input & (INPUT_SPIN_LEFT | INPUT_SPIN_RIGHT) && !((input & INPUT_SPIN_LEFT) && (input & INPUT_SPIN_RIGHT)))
		{
			//the user desires a spin to happen, we need to see if it's possible
			//we first get the id of the next rotation
			int nextID;
			if (input & INPUT_SPIN_LEFT)
				nextID = falling_piece_orientation - 1;
			else
				nextID = falling_piece_orientation + 1;
			//check if the nextID went above the index of the highest indexed rotation for the current piece
			//in which case we loop it back to the first
			if (piece_indices_boundaries[falling_piece_type].second < nextID)
			{
				nextID = piece_indices_boundaries[falling_piece_type].first;
			}
			//it also may have gone below first boundary, in case we're subtracting from nextID
			//in which case we loop back from the other side
			else if (piece_indices_boundaries[falling_piece_type].first > nextID)
			{
				nextID = piece_indices_boundaries[falling_piece_type].second;
			}

			if (check_falling_piece<false>((piece_orientation)nextID, falling_piece_center))
			{
				falling_piece_orientation = (piece_orientation)nextID;
			}
		}
	}
	//check if required movement is possible
	if (input & (INPUT_LEFT | INPUT_RIGHT) && !((input & INPUT_LEFT) && (input & INPUT_RIGHT)))
	{
		point new_center_coords = { falling_piece_center.row, falling_piece_center.col + ((input & INPUT_LEFT) ? -1 : 1) };
		if (check_falling_piece<false>(falling_piece_orientation, new_center_coords))
		{
			falling_piece_center = new_center_coords;
		}
	}
	//check if fall is possible
	point fallen_piece_coords = { falling_piece_center.row-1, falling_piece_center.col };
	if (check_falling_piece<false>(falling_piece_orientation, fallen_piece_coords))
	{
		falling_piece_center = fallen_piece_coords;
	}
	else
	{
		check_falling_piece<true>(falling_piece_orientation, falling_piece_center);
	}

}


int tetris_bitboard::popcount_row(int row_index)
{
	return __popcnt64(bbs[bitboard_index[row_index]] & row_template[row_index]);
}



void tetris_bitboard::print_board()
{
	for (int row = rows - 1; row >= 0; row--)
	{
		for (int col = 0; col < cols; col++)
		{
			bool is_falling_piece = false;

			for (int block = 0; block < 4; block++)
			{
				int block_row = piece_block_coordinates[falling_piece_orientation][block].row + falling_piece_center.row;
				int block_col = piece_block_coordinates[falling_piece_orientation][block].col + falling_piece_center.col;
				if (block_row == row && block_col == col)
				{
					is_falling_piece = true;
					break;
				}
			}

			std::cout << "[";
			if (is_falling_piece)
			{
				std::cout << "O";
			}
			

			bool is_taken = false;
			bitboard bb_to_use = bbs[bitboard_index[row]];
			int row_in_bb = row_in_bb_index[row];
			int square_index = row_in_bb * cols + col;

			if (bb_to_use & (1ULL << square_index))
			{
				std::cout << "X";
			}
			else if (!is_falling_piece)
			{
				std::cout << " ";
			}
			std::cout << "]";
		}
		std::cout << "\n";
	}
	std::cout << "game over " << game_over << "\n";
	std::cout << "center coords " << falling_piece_center.row << ", " << falling_piece_center.col << "\n";
	std::cout << "falling piece type " << falling_piece_orientation << "\n";
	std::cout << "piece_heights " << piece_heights[falling_piece_orientation].first << ", " << piece_heights[falling_piece_orientation].second << "\n";
}