#include "nn.h"
#include <iostream>
#include <fstream>

void NN::nn::read_weights(const std::string& filename)
{
	std::ifstream weights_file(filename, std::ios::binary);
	if (!weights_file.good())
	{
		std::cout << "loasdd" << "\n";
	}

	float accumulator_weights[input_size][layer_sizes[0]];
	float accumulator_biases[layer_sizes[0]];

	float first_layer_weights[layer_sizes[1]][layer_sizes[0]];
	float first_layer_biases[layer_sizes[1]];

	float second_layer_weights[layer_sizes[2]][layer_sizes[1]];
	float second_layer_biases[layer_sizes[2]];

	float output_layer_weights[layer_sizes[3]][layer_sizes[2]];
	float output_layer_biases[layer_sizes[3]];

	weights_file.read((char*)accumulator_weights, sizeof(accumulator_weights));
	weights_file.read((char*)accumulator_biases, sizeof(accumulator_biases));
	nn_accumulator::read_weights((float*)accumulator_weights, accumulator_biases);

	weights_file.read((char*)first_layer_weights, sizeof(first_layer_weights));
	weights_file.read((char*)first_layer_biases, sizeof(first_layer_biases));
	first_layer.load_weights((float*)first_layer_weights, first_layer_biases);

	weights_file.read((char*)second_layer_weights, sizeof(second_layer_weights));
	weights_file.read((char*)second_layer_biases, sizeof(second_layer_biases));
	second_layer.load_weights((float*)second_layer_weights, second_layer_biases);

	weights_file.read((char*)output_layer_weights, sizeof(output_layer_weights));
	weights_file.read((char*)output_layer_biases, sizeof(output_layer_biases));
	output_layer.load_weights((float*)output_layer_weights, output_layer_biases);

	weights_file.close();
}

template<int size>
void ReLU(float* input)
{
	constexpr int floats_per_pack = sizeof(__m128) / sizeof(float);
	constexpr int num_packs = size / floats_per_pack;
	for (int pack = 0, input_neuron = 0; pack < num_packs; pack++, input_neuron += floats_per_pack)
	{
		__m128 input_pack = _mm_load_ps(&input[input_neuron]);
		static constexpr __m128 zero = { 0.0f,0.0f,0.0f,0.0f };
		input_pack = _mm_max_ps(input_pack, zero);
		_mm_store_ps(&input[input_neuron], input_pack);
	}
}

template<int size>
void tanh(float* input)
{
	constexpr int floats_per_pack = sizeof(__m128) / sizeof(float);
	constexpr int num_packs = size / floats_per_pack;
	for (int pack = 0, input_neuron = 0; pack < num_packs; pack++, input_neuron += floats_per_pack)
	{
		__m128 input_pack = _mm_load_ps(&input[input_neuron]);
		input_pack = _mm_tanh_ps(input_pack);
		_mm_store_ps(&input[input_neuron], input_pack);
	}
}


float* NN::nn::calculate_moves(tetris_bitboard& playfield)
{
	auto acc_output = playfield.get_acc().calculate_output(playfield.get_falling_piece_center(),playfield.get_piece_orientation());
	ReLU<nn_accumulator::accumulator_size>(acc_output);
	
	auto first_layer_output = first_layer.forward(acc_output);
	ReLU<layer_sizes[FIRST_HIDDEN_LAYER]>(first_layer_output);
	auto second_layer_output = second_layer.forward(first_layer_output);
	ReLU<layer_sizes[SECOND_HIDDEN_LAYER]>(second_layer_output);
	auto output = output_layer.forward(second_layer_output);
	tanh<layer_sizes[OUTPUT_LAYER]>(output);
	return output;
}