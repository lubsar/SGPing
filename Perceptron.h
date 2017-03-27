#pragma once
#include <vector>
using namespace std;

namespace sgann {
	class Perceptron {
	private:
		vector<float*> inputs;
		vector<float> weights;

	public:
		float bias;
		Perceptron() {
			this->bias = 0.0f;
		}

		Perceptron(float bias) {
			this->bias = bias;
		}

		void add_input(float*, float);
		void clear_inputs();
		bool calculate();
	};
}
