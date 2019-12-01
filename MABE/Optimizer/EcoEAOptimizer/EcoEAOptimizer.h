//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

#pragma once // directive to insure that this .h file is only included one time

#include "../AbstractOptimizer.h"
#include "../../Utilities/MTree.h"

#include <iostream>
#include <numeric>
#include <algorithm>
#include <vector>
#include <memory>

class EcoEAOptimizer : public AbstractOptimizer {
public:
	static std::shared_ptr<ParameterLink<double>> epsilonPL;
	static std::shared_ptr<ParameterLink<std::string>> nextPopSizePL;
	static std::shared_ptr<ParameterLink<int>> numberParentsPL;
	static std::shared_ptr<ParameterLink<int>> tournamentSizePL;
	static std::shared_ptr<ParameterLink<std::string>> optimizeValueNamesPL;
	static std::shared_ptr<ParameterLink<double>> inflowPL;
	static std::shared_ptr<ParameterLink<double>> outflowPL;
	static std::shared_ptr<ParameterLink<int>> leadingUpdatesPL;

	std::vector<double> scores, adjusted_scores;
	std::vector<std::string> optimize_value_name_vec;
    std::vector<double> resource_pools;
	double epsilon, inflow, outflow;
    int tournament_size, leading_updates;

	std::shared_ptr<Abstract_MTree> nextPopSizeFormula;
	std::vector<std::shared_ptr<Organism>> newPopulation;
	std::vector<std::shared_ptr<Organism>> oldPopulation;

	int number_parents;

	bool recordOptimizeValues;

	EcoEAOptimizer(std::shared_ptr<ParametersTable> PT_ = nullptr);

	virtual void optimize(std::vector<std::shared_ptr<Organism>> &population) override;

	virtual void cleanup(std::vector<std::shared_ptr<Organism>> &population) override;


	int TournamentSelect(size_t tournament_size);
};
