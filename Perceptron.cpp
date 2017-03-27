#include "Perceptron.h"
#include <numeric>
#include <iostream>

namespace sgann {
	void Perceptron::add_input(float* input, float weight) {
		this->inputs.push_back(input);
		this->weights.push_back(weight);
	}

	void Perceptron::clear_inputs() {
		this->inputs.clear();
		this->weights.clear();
	}

	bool Perceptron::calculate() {
		float sum = 0.0f;
		for (unsigned i = 0; i < inputs.size(); i++) {
			sum += *inputs[i] * weights[i];
		}

		cout << sum << endl;

		return sum + bias > 0;
	}
}