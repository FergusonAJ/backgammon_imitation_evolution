//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

#include "EcoEAOptimizer.h"
#include <iostream>
#include <numeric>
#include <algorithm>
#include <vector>
#include <memory>


std::shared_ptr<ParameterLink<std::string>> EcoEAOptimizer::nextPopSizePL =
Parameters::register_parameter("OPTIMIZER_ECOEA-nextPopSize", (std::string) "-1",
	"size of population after optimization(MTree). -1 indicates use current population size");
std::shared_ptr<ParameterLink<int>> EcoEAOptimizer::numberParentsPL =
Parameters::register_parameter("OPTIMIZER_ECOEA-numberParents", 1,
	"number of parents used to produce offspring");
std::shared_ptr<ParameterLink<double>> EcoEAOptimizer::epsilonPL =
Parameters::register_parameter("OPTIMIZER_ECOEA-epsilon", 0.0,
	"cutoff when conducting per formula selection.\n"
	"e.g. 0.1 = organisms in the top 90% are kept. use 0.0 for classic EcoEA.");
std::shared_ptr<ParameterLink<int>> EcoEAOptimizer::tournamentSizePL = 
Parameters::register_parameter("OPTIMIZER_ECOEA-tournamentSize", 7,
	"Size of each tournament in EcoEA selection");
std::shared_ptr<ParameterLink<std::string>> EcoEAOptimizer::optimizeValueNamesPL =
Parameters::register_parameter("OPTIMIZER_ECOEA-optimizeValueNames", (std::string) "mostForward,avgForward,leastForward,aggressive,wideDefense,tallDefense",
	"Names of data map elements to be used for EcoEA selection");
std::shared_ptr<ParameterLink<double>> EcoEAOptimizer::inflowPL =
Parameters::register_parameter("OPTIMIZER_ECOEA-inflow", 0.10,
	"Resource inflow per update");
std::shared_ptr<ParameterLink<double>> EcoEAOptimizer::outflowPL =
Parameters::register_parameter("OPTIMIZER_ECOEA-outflow", 0.01,
    "Resource outflow rate per update (as a percentage)");
std::shared_ptr<ParameterLink<int>> EcoEAOptimizer::leadingUpdatesPL =
Parameters::register_parameter("OPTIMIZER_ECOEA-leadingUpdates", 100,
    "Number of updates of inflow to start with.");
std::shared_ptr<ParameterLink<double>> EcoEAOptimizer::consumptionFractionPL =
Parameters::register_parameter("OPTIMIZER_ECOEA-consumptionFraction", 1.0,
    "Percentage of resources an organism actually gets (1.0 for normal)");
std::shared_ptr<ParameterLink<double>> EcoEAOptimizer::maxCapPL =
Parameters::register_parameter("OPTIMIZER_ECOEA-maxCap", 1.0,
    "Maximum resource an organism can take from one resource pool per update");


EcoEAOptimizer::EcoEAOptimizer(std::shared_ptr<ParametersTable> PT_) : AbstractOptimizer(PT_) {
    convertCSVListToVector(optimizeValueNamesPL->get(PT), optimize_value_name_vec);
	epsilon = epsilonPL->get(PT);
	nextPopSizeFormula = stringToMTree(nextPopSizePL->get(PT));
	number_parents = numberParentsPL->get(PT);
    tournament_size = tournamentSizePL->get(PT);	
    inflow = inflowPL->get(PT);
    outflow = outflowPL->get(PT);
    leading_updates = leadingUpdatesPL->get(PT);
    consumption_frac = consumptionFractionPL->get(PT);
    max_cap = maxCapPL->get(PT);
    resource_pools.reserve(optimize_value_name_vec.size());
    std::cout << "EcoEA Inflow: " << inflow << std::endl;
    std::cout << "EcoEA Outflow: " << outflow << std::endl;
    std::cout << "EcoEA Leading Updates: " << leading_updates << std::endl;
    std::cout << "EcoEA Consumption Fraction: " << consumption_frac << std::endl;
    std::cout << "EcoEA Maximum Cap: " << max_cap << std::endl;
    for(size_t opt_idx = 0; opt_idx < optimize_value_name_vec.size(); ++opt_idx){
        resource_pools[opt_idx] = inflow * leading_updates;
    }   
	// leave this undefined so that max.csv is not generated
	//optimizeFormula = optimizeValueMT;

	popFileColumns.clear();
}



int EcoEAOptimizer::TournamentSelect(size_t tourney_size) {
  int winner_idx, challenger_idx;
  winner_idx = Random::getIndex(scores.size());
  for (int i = 0; i < tourney_size - 1; ++i) {
    challenger_idx = Random::getIndex(scores.size());
    if (adjusted_scores[challenger_idx] > adjusted_scores[winner_idx]) {
      winner_idx = challenger_idx;
    }
  }
  return winner_idx;
}


void EcoEAOptimizer::optimize(std::vector<std::shared_ptr<Organism>> &population) {
  
  scores.clear();
  adjusted_scores.clear();
  
  std::vector<double> avgScores;
  avgScores.reserve(optimize_value_name_vec.size());
  std::vector<double> maxScores;
  maxScores.reserve(optimize_value_name_vec.size());
  std::vector<double> minScores;
  minScores.reserve(optimize_value_name_vec.size());
  std::vector<size_t> num_at_max;
  num_at_max.resize(optimize_value_name_vec.size(), 0);
  double max_score = -10000000;
  double max_adjusted_score = -10000000;
  for (auto &org : population){
    scores.push_back(org->dataMap.getDoubleVector("score")[0]);
    if(org->dataMap.getDoubleVector("score")[0] > max_score)
        max_score = org->dataMap.getDoubleVector("score")[0];
    adjusted_scores.push_back(org->dataMap.getDoubleVector("score")[0]);
    for(size_t opt_idx = 0; opt_idx < optimize_value_name_vec.size(); ++opt_idx){
        if(org->dataMap.getDoubleVector(optimize_value_name_vec[opt_idx])[0] > maxScores[opt_idx])
            maxScores[opt_idx] = org->dataMap.getDoubleVector(optimize_value_name_vec[opt_idx])[0];
        if(org->dataMap.getDoubleVector(optimize_value_name_vec[opt_idx])[0] < minScores[opt_idx])
            minScores[opt_idx] = org->dataMap.getDoubleVector(optimize_value_name_vec[opt_idx])[0];
        avgScores[opt_idx] += org->dataMap.getDoubleVector(optimize_value_name_vec[opt_idx])[0];
    }
  }
  // Divide through averages
  for(size_t opt_idx = 0; opt_idx < optimize_value_name_vec.size(); ++opt_idx){
    avgScores[opt_idx] /= population.size();
  }
  // Account for epsilon
  for (auto &org : population){
    for(size_t opt_idx = 0; opt_idx < optimize_value_name_vec.size(); ++opt_idx){
        if(org->dataMap.getDoubleVector(optimize_value_name_vec[opt_idx])[0] 
                >= maxScores[opt_idx] - epsilon)
            num_at_max[opt_idx]++;
   }
  }
  
  std::vector<double> resource_differences;
  resource_differences.resize(optimize_value_name_vec.size(), 0); 
  double diff;
  // Update adjusted scores based on resource pools
  for (size_t pop_idx = 0; pop_idx < population.size(); ++pop_idx){
    auto & org = population[pop_idx];
    for(size_t opt_idx = 0; opt_idx < optimize_value_name_vec.size(); ++opt_idx){
        if(org->dataMap.getDoubleVector(optimize_value_name_vec[opt_idx])[0] 
                >= maxScores[opt_idx] - epsilon){
            diff = resource_pools[opt_idx] / num_at_max[opt_idx];
            diff *= consumption_frac;
            if(diff > max_cap)
                diff = max_cap;
            adjusted_scores[pop_idx] += diff;
            resource_differences[opt_idx] += diff;
        }
    }
    if(adjusted_scores[pop_idx] > max_adjusted_score){
        max_adjusted_score = adjusted_scores[pop_idx];
    }
  }

  std::cout << std::endl << "Resource pools:" << std::endl;
  // Update resource pools
  for(size_t opt_idx = 0; opt_idx < optimize_value_name_vec.size(); ++opt_idx){
    resource_pools[opt_idx] -= resource_differences[opt_idx];
    resource_pools[opt_idx] -= resource_pools[opt_idx] * outflow;
    if(resource_pools[opt_idx] < 0)
        resource_pools[opt_idx] = 0;
    resource_pools[opt_idx] += inflow;
    std::cout << optimize_value_name_vec[opt_idx] << ": " << resource_pools[opt_idx] << std::endl;
  }

  size_t nextPopulationTargetSize = nextPopSizeFormula->eval(PT)[0];
  nextPopulationTargetSize = nextPopulationTargetSize == -1
                                 ? population.size()
                                 : nextPopulationTargetSize;

  // generate new organisms
  // do not add to population until all have been
  // selected
  newPopulation.clear();
  newPopulation.reserve(nextPopulationTargetSize);

  // generate a list of 'nextPopulationTargetSize' new orgs into 'newPopulation'
  // for each, generate a 'parents' vector with 'numberParents' parent orgs
  // parents are selected with lexiSelect( m_choose_n(population.size(), poolSize))
  //   where the m_choose_n command selects 'poolSize' number of population indexes
  std::generate_n(
      std::back_inserter(newPopulation), nextPopulationTargetSize, [&] {
        std::vector<std::shared_ptr<Organism>> parents;
        std::generate_n(std::back_inserter(parents), number_parents, [&] {
          return population[TournamentSelect(tournament_size)];
        });
        return parents[0]->makeMutatedOffspringFromMany(parents);
      });

  oldPopulation = population;
  population.insert(population.end(), newPopulation.begin(), newPopulation.end());
  std::cout << "maxes: raw = " << max_score << ", adjusted = " << max_adjusted_score << std::endl;
  for (size_t fIndex = 0; fIndex < optimize_value_name_vec.size(); fIndex++) {
    std::cout << std::endl
              << "   " << optimize_value_name_vec[fIndex]
              << ":  max = " << std::to_string(maxScores[fIndex])
              << ":  min = " << std::to_string(minScores[fIndex])
              << "   ave = " << std::to_string(avgScores[fIndex]) << std::flush;
  }
}

void EcoEAOptimizer::cleanup(std::vector<std::shared_ptr<Organism>> &population) {
	for (auto org : oldPopulation) {
		org->kill();
	}
	population.swap(newPopulation);
	newPopulation.clear();
}

