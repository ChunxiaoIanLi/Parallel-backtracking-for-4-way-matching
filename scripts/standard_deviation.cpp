#include <iostream>
#include <cmath>
#include <vector>

float calculateSD(std::vector<float> data)
{
    float sum = 0.0, mean, standardDeviation = 0.0;

    int i;

    for(i = 0; i < 10; ++i)
        {
            sum += data[i];
        }

    mean = sum/10;

    for(i = 0; i < 10; ++i)
        standardDeviation += pow(data[i] - mean, 2);

    return sqrt(standardDeviation / 9);
}

int main()
{
    int i;
	std::vector<float> data;

	std::cout << "Enter 10 elements: ";
    for(i = 0; i < 10; ++i){
		data.push_back(i*1.0);
	}
	std::cout << std::endl << "Standard Deviation = " << calculateSD(data);

    return 0;
}
