#include <iostream>
#include <random>
#include <vector>
#include <string>
#include <cmath>
#include <iomanip>
#include <ctime>
#include <fstream>

double fitness(std::pair<double, double> unit) // Фитнес функция
{
	double fit = cos(unit.first)*cos(unit.second);
	return fit;
}

std::vector<std::pair<double, double>> startUnitGenerator() // Генерация начальной популяции
{
	std::ofstream file0("file0.txt");
	std::vector<std::pair<double, double>> unitVector(4);
	for (int i = 0; i < 4; i++)
	{
		std::mt19937 engine(std::random_device{}());
		auto generator = std::uniform_real_distribution<double>(0.0, 4.0);
		double x = generator(engine) - 2;
		generator = std::uniform_real_distribution<double>(0.0, 4.0);
		double y = generator(engine) - 2;
		unitVector[i] = std::make_pair(x, y);
	}

	std::vector<double> fit(4);
	std::cout << "0(start)";
	double maxFit = -1;
	double sumFit = 0;
	for (int i = 0; i < 4; i++)
	{
		fit[i] = fitness(unitVector[i]);
		sumFit += fitness(unitVector[i]);
	}
	for (int i = 0; i < 4; i++)
	{
		//fit[i] /= sumFit;
		maxFit = std::max(maxFit, fit[i]);
	}
	std::cout << "\t|" << std::fixed << std::setprecision(5) << unitVector[0].first << " \t|" << unitVector[0].second << " \t|" << fit[0] << " \t|\n";
	for (int i = 1; i < 4; i++)
	{
		std::cout << "\t\t|" << std::fixed << std::setprecision(5) << unitVector[i].first << " \t|" << unitVector[i].second << " \t|" << fit[i] << " \t|\n";
	}
	for (int i = 0; i < 4; i++)
	{
		file0 << "(" << unitVector[i].first << ", " << unitVector[i].second << ")\n";
	}
	std::cout << "\t\t\t\t\t\t\t\t|" << maxFit << "\t" << sumFit / 4 << "\n\n";
	return unitVector;
}

std::pair<std::pair<double, double>, std::pair<double, double>> children(std::pair<double, double> parent1, std::pair<double, double> parent2) // Кроссовер
{
	std::pair<double, double> child1;
	child1.first = parent1.first;
	child1.second = parent2.second;
	std::pair<double, double> child2;
	child2.first = parent2.first;
	child2.second = parent1.second;
	return std::make_pair(child1, child2);
}

std::pair<double, double> mutation(std::pair<double, double> unit) // Мутация
{
	std::mt19937 engine(std::random_device{}());
	int parameter = rand() % 2;
	auto generatorDouble = std::uniform_real_distribution<double>(0.0, 0.2);
	double delta = generatorDouble(engine) - 0.1;
	if (parameter == 0)
	{
		unit.first += delta;
	}
	else
	{
		unit.second += delta;
	}
	return unit;
}

std::pair<int, int> parentGenerator(std::vector<double> fitness, double sumFit) // Выбор родителей
{
	std::pair<int, int> parent;
	std::mt19937 engine(std::random_device{}());
	auto generator = std::uniform_real_distribution<double>(0.0, sumFit);
	double p1 = generator(engine);
	int chosenIndex;
	if (p1 < fitness[0])
	{
		parent.first = chosenIndex = 0;
	}
	else if (p1 < fitness[0] + fitness[1])
	{
		parent.first = chosenIndex = 1;
	}
	else if (p1 < fitness[0] + fitness[1] + fitness[2])
	{
		parent.first = chosenIndex = 2;
	}
	else
	{
		parent.first = chosenIndex = 3;
	}
	bool mistake = true;
	while (mistake)
	{
		mistake = false;
		generator = std::uniform_real_distribution<double>(0.0, sumFit);
		double p2 = generator(engine);
		if (p2 < fitness[0])
		{
			parent.second = 0;
		}
		else if (p2 < fitness[0] + fitness[1])
		{
			parent.second = 1;
		}
		else if (p2 < fitness[0] + fitness[1] + fitness[2])
		{
			parent.second = 2;
		}
		else
		{
			parent.second = 3;
		}
		if (parent.second == chosenIndex)
		{
			mistake = true;
		}
	}
	return parent;
}

std::vector<std::pair<double, double>> iteration(std::vector<std::pair<double, double>> unitVector) // Итерация
{
	std::vector<std::pair<double, double>> newVector = unitVector;
	std::mt19937 engine(std::random_device{}());
	std::vector<double> fit(4);
	double maxFit = -1;
	double sumFit = 0;
	for (int i = 0; i < 4; i++)
	{
		fit[i] = fitness(unitVector[i]);
		sumFit += fitness(unitVector[i]);
	}
	std::pair<int, int> parent = parentGenerator(fit, sumFit);
	std::pair<std::pair<double, double>, std::pair<double, double>> newChildren = children(unitVector[parent.first], unitVector[parent.second]);
	newVector.push_back (newChildren.first);
	newVector.push_back (newChildren.second);
	for (int i = 0; i < 6; i++)
	{
		auto generator = std::uniform_real_distribution<double>(0.0, 1.0);
		double mutationChance = generator(engine);
		if (mutationChance < 0.25)
		{
			newVector[i] = mutation(newVector[i]);
		}
	}
	std::vector<double> newFit(6);
	sumFit = 0;
	for (int i = 0; i < 6; i++)
	{
		newFit[i] = fitness(newVector[i]);
	}
	double minFit = 1;
	int minFitIndex1;
	for (int i = 0; i < 6; i++)
	{
		if (newFit[i] < minFit)
		{
			minFit = newFit[i];
			minFitIndex1 = i;
		}
	}
	minFit = 1;
	int minFitIndex2;
	for (int i = 0; i < 6; i++)
	{
		if (i != minFitIndex1)
		{
			if (newFit[i] < minFit)
			{
				minFit = newFit[i];
				minFitIndex2 = i;
			}
		}
	}
	std::vector<std::pair<double, double>> vectorAfterDelete;
	for (int i = 0; i < 6; i++)
	{
		if ((i != minFitIndex1) && (i != minFitIndex2))
		{
			vectorAfterDelete.push_back(newVector[i]);
		}
	}
	unitVector = vectorAfterDelete;
	for (int i = 0; i < 4; i++)
	{
		fit[i] = fitness(unitVector[i]);
		sumFit += fitness(unitVector[i]);
	}
	for (int i = 0; i < 4; i++)
	{
		//fit[i] /= sumFit;
		maxFit = std::max(maxFit, fit[i]);
	}
	for (int i = 0; i < 4; i++)
	{
		std::cout << "\t\t|" << std::fixed << std::setprecision(5) << unitVector[i].first << " \t|" << unitVector[i].second << " \t|" << fit[i] << " \t|\n";
	}
	std::cout << "\t\t\t\t\t\t\t\t|" << maxFit << "\t" << sumFit / 4 << "\n\n";
	return unitVector;
}

void geneticsAlgorithm(int generations) // Генетический алгоритм
{
	std::cout << "Generation" << "\t|X\t\tY\t\tFitness\t\tMax\t\tMiddle\n";
	std::mt19937 engine(std::random_device{}());
	std::vector<std::pair<double, double>> unitVector;
	unitVector.resize(4);
	unitVector = startUnitGenerator();
	for (int i = 1; i <= generations; i++)
	{
		std::string fileName = "file" + std::to_string(i) + ".txt";
		std::ofstream file(fileName);
		std::cout << i;
		unitVector = iteration(unitVector);
		for (int i = 0; i < 4; i++)
		{
			file << "(" << unitVector[i].first << ", " << unitVector[i].second << ")\n";
		}
	}
}

int main()
{
	srand(time(0));
	geneticsAlgorithm(10);
	system("pause");
	return 0;
}
