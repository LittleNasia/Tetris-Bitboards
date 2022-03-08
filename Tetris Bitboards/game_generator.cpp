#include "game_generator.h"

#include <random>
#include <iostream>
#include <fstream>


void game_generator::generate_games()
{
	tetris_bitboard board;

	std::random_device device;
	std::mt19937 generator(device());
	std::uniform_int_distribution<int> distribution(0, 15);

	std::vector<std::vector<position>> games;
	
	const int possible_moves[] = { 0,1,2,4,8 };
	NN::nn network;
	network.read_weights("tetris.mlp");
	while(true)
	{
		
		std::vector<position> match;
		while (!board.is_over())
		{
			position current_pos;
			int move = distribution(generator);
			//board.print_board();
			//std::cin >> move;
			std::memcpy(current_pos.bb, board.get_bb(), sizeof(current_pos.bb));
			current_pos.move_played = move;
			current_pos.piece_center = board.get_falling_piece_center();
			current_pos.falling_piece_type = board.get_piece_orientation();
			auto output = network.calculate_moves(board);
			float max = -10;
			int max_index = 0;
			for (int i = 0; i < 4; i++)
			{
				std::cout << output[i] << "\n";
				if (output[i] > max)
				{
					max_index = i;
					max = output[i];
				}
			}
			board.print_board();
			std::cin >> move;
			move = (1ULL << max_index);
			board.on_frame(move);
			match.push_back(current_pos);
		}
		int score = board.get_score();
		if (score > 660)
		{
			games.push_back(match);
		}
		board.new_game();
		if (games.size() > 1000)
		{
			break;
		}
	}
	std::ofstream game_file("games.bin", std::ios::binary);
	for (auto& game : games)
	{
		for (auto& pos : game)
		{
			game_file.write((char*)pos.bb, sizeof(pos.bb));
			game_file.write((char*)&pos.piece_center, sizeof(pos.piece_center));
			game_file.write((char*)&pos.falling_piece_type, sizeof(pos.falling_piece_type));
			game_file.write((char*)&pos.move_played, sizeof(pos.move_played));
		}
		
	}
	game_file.close();
	
	

}