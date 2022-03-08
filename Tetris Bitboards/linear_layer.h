#pragma once
#include <intrin.h>

namespace NN
{

	template<int in_neurons, int out_neurons>
	class alignas(64) linear_layer
	{
		float weights[out_neurons][in_neurons];
		float biases[out_neurons];
		float output[out_neurons];
	public:
		void load_weights(float* loaded_weights, float* loaded_biases)
		{
			for (int col = 0, index = 0; col < in_neurons; col++)
			{
				for (int row = 0; row < out_neurons; row++, index++)
				{
					weights[row][col] = loaded_weights[index];
				}
			}
			std::memcpy(biases, loaded_biases, sizeof(biases));
		}
		linear_layer()
		{
			for (int output_neuron = 0; output_neuron < out_neurons; output_neuron++)
			{
				for (int input_neuron = 0; input_neuron < in_neurons; input_neuron++)
				{
					weights[output_neuron][input_neuron] = 0.01;
				}
				biases[output_neuron] = 0;
			}
		}
		float* forward(const float* input)
		{
			std::memcpy(output, biases, sizeof(output));
			for (int output_neuron = 0; output_neuron < out_neurons; output_neuron++)
			{
				constexpr int floats_per_register = sizeof(__m128) / sizeof(float);
				constexpr int num_packs = in_neurons / floats_per_register;
				for (int pack = 0, input_neuron = 0; pack < num_packs; pack++, input_neuron += floats_per_register)
				{
					const __m128 input_pack = _mm_load_ps(&input[input_neuron]);
					const __m128 weight_pack = _mm_load_ps(&weights[output_neuron][input_neuron]);

					__m128 elementwise_mul_result = _mm_mul_ps(input_pack, weight_pack);

					// sum all results using two horizontal sums
					__m128 first_sum_result = _mm_hadd_ps(elementwise_mul_result, elementwise_mul_result);
					__m128 summed_vector = _mm_hadd_ps(first_sum_result, first_sum_result);

					// returns the lowest byte in a vector
					float sum = _mm_cvtss_f32(summed_vector);
					output[output_neuron] += sum;
				}
			}
			return output;
		}
	};
}