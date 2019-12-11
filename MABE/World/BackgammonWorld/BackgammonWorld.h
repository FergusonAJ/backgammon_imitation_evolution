//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

#pragma once

// Local (MABE)
#include "../AbstractWorld.h"
// Local (Non-MABE)
#include "../../../src/backgammon.h"
#include "../../../src/agent_brain.h"
#include "../../../src/agent_weighted.h"
#include "../../../src/agent_random.h"
#include "../../../src/agent_pubeval.h"
// Empirical

// Standard Library
#include <cstdlib>
#include <thread>
#include <vector>
#include <utility>

class BackgammonWorld : public AbstractWorld {

public:
  static std::shared_ptr<ParameterLink<int>> modePL;
  static std::shared_ptr<ParameterLink<int>> numberOfOutputsPL;
  static std::shared_ptr<ParameterLink<int>> numberOfMatchesPL;
  static std::shared_ptr<ParameterLink<int>> numberOfPredictionMatchesPL;
  static std::shared_ptr<ParameterLink<int>> evaluationsPerGenerationPL;
  static std::shared_ptr<ParameterLink<std::string>> groupNamePL;
  static std::shared_ptr<ParameterLink<std::string>> brainNamePL;
  static std::shared_ptr<ParameterLink<double>> scoreScalingFactorPL;
  static std::shared_ptr<ParameterLink<std::string>> comparisonAgentPL;

  double score_scaling_factor;
  int num_matches, num_prediction_matches;

  BackgammonGame game;
  BackgammonAgent_Brain agent_brain;
  BackgammonAgent_PubEval agent_pub_eval; 
  BackgammonAgent_Random agent_random, agent_random_2; 
  std::vector<BackgammonAgent_Weighted> agent_weighted_vec;
  std::vector<std::string> weight_name_vec {"mostForward", "avgForward", "leastForward", 
        "aggressive", "wideDefense", "tallDefense"};
  bool use_random_agent;

  BackgammonWorld(std::shared_ptr<ParametersTable> PT_ = nullptr);
  virtual ~BackgammonWorld() = default;

  void evaluateSolo(std::shared_ptr<Organism> org, int analyze,
                            int visualize, int debug);
  void evaluate(std::map<std::string, std::shared_ptr<Group>> &groups,
                int analyze, int visualize, int debug);

  virtual std::unordered_map<std::string, std::unordered_set<std::string>>
    requiredGroups() override;
};

std::shared_ptr<ParameterLink<int>> BackgammonWorld::modePL =
    Parameters::register_parameter(
        "WORLD_BACKGAMMON-mode", 0, "0 = bit outputs before adding, 1 = add outputs");
std::shared_ptr<ParameterLink<int>> BackgammonWorld::numberOfOutputsPL =
    Parameters::register_parameter("WORLD_BACKGAMMON-numberOfOutputs", 10,
                                   "number of outputs in this world");
std::shared_ptr<ParameterLink<int>> BackgammonWorld::numberOfMatchesPL =
    Parameters::register_parameter("WORLD_BACKGAMMON-numberOfMatches", 1000,
                                   "number of matches between pairs of brains");
std::shared_ptr<ParameterLink<int>> BackgammonWorld::numberOfPredictionMatchesPL =
    Parameters::register_parameter("WORLD_BACKGAMMON-numberOfPredictionMatches", 10,
                                   "Number of matches for each brain to try and predict each agent");
std::shared_ptr<ParameterLink<int>> BackgammonWorld::evaluationsPerGenerationPL =
    Parameters::register_parameter("WORLD_BACKGAMMON-evaluationsPerGeneration", 1,
                                   "Number of times to test each Genome per "
                                   "generation (useful with non-deterministic "
                                   "brains)");
std::shared_ptr<ParameterLink<std::string>> BackgammonWorld::groupNamePL =
    Parameters::register_parameter("WORLD_BACKGAMMON_NAMES-groupNameSpace",
                                   (std::string) "root::",
                                   "namespace of group to be evaluated");
std::shared_ptr<ParameterLink<std::string>> BackgammonWorld::brainNamePL =
    Parameters::register_parameter(
        "WORLD_BACKGAMMON_NAMES-brainNameSpace", (std::string) "root::",
        "namespace for parameters used to define brain");
std::shared_ptr<ParameterLink<double>> BackgammonWorld::scoreScalingFactorPL =
    Parameters::register_parameter("WORLD_BACKGAMMON_NAMES-scoreScalingFactor", 1.0, 
                                   "How to weight the true score");
std::shared_ptr<ParameterLink<std::string>> BackgammonWorld::comparisonAgentPL =
    Parameters::register_parameter(
        "WORLD_BACKGAMMON-comparisonAgent", (std::string) "PUB_EVAL",
        "Determines which type of agent to compare against (RANDOM or PUB_EVAL)");

BackgammonWorld::BackgammonWorld(std::shared_ptr<ParametersTable> PT_)
        : AbstractWorld(PT_) {
    score_scaling_factor = scoreScalingFactorPL->get(PT);
    num_matches = numberOfMatchesPL->get(PT); 
    num_prediction_matches = numberOfPredictionMatchesPL->get(PT);
    // columns to be added to ave file
    popFileColumns.clear();
    popFileColumns.push_back("score");
    popFileColumns.push_back("score_VAR"); // specifies to also record the
                                           // variance (performed automatically
                                           // because _VAR)
    popFileColumns.push_back("winRate");
    popFileColumns.push_back("winRate_VAR");
    popFileColumns.push_back("mostForward");
    popFileColumns.push_back("mostForward_VAR");
    popFileColumns.push_back("avgForward");
    popFileColumns.push_back("avgForward_VAR");
    popFileColumns.push_back("leastForward");
    popFileColumns.push_back("leastForward_VAR");
    popFileColumns.push_back("aggressive");
    popFileColumns.push_back("aggressive_VAR");
    popFileColumns.push_back("wideDefense");
    popFileColumns.push_back("wideDefense_VAR");
    popFileColumns.push_back("tallDefense");
    popFileColumns.push_back("tallDefense_VAR");
    
    use_random_agent = comparisonAgentPL->get(PT) == "RANDOM"; 
    
    // Set up Backgammon prerequisites
    game.SetSeed(Global::randomSeedPL->get());
    agent_brain.SetRandomSeed(Global::randomSeedPL->get());
    agent_random.SetRandomSeed(Global::randomSeedPL->get());
    agent_random_2.SetRandomSeed(Global::randomSeedPL->get());
    // Attach the agents to be used
    game.AttachAgent(&agent_brain);
    if(use_random_agent)
        game.AttachAgent(&agent_random);
    else
        game.AttachAgent(&agent_pub_eval);
    for(size_t i = 0; i < 6; ++i){
        BackgammonAgent_Weighted agent_weighted;
        agent_weighted.SetRandomSeed(Global::randomSeedPL->get());
        agent_weighted.SetAgentId(1);
        game.ReplaceAgent(&agent_weighted, 1); // Just to grab a callback
        agent_weighted.SetWeight_MostForward( i == 0 ? 1 : 0);// Most Forward
        agent_weighted.SetWeight_AvgForward(  i == 1 ? 1 : 0);// Avg Forward 
        agent_weighted.SetWeight_LeastForward(i == 2 ? 1 : 0);// Least Forward
        agent_weighted.SetWeight_Aggressive(  i == 3 ? 1 : 0);// Aggressive
        agent_weighted.SetWeight_WideDefense( i == 4 ? 1 : 0);// Wide Defense
        agent_weighted.SetWeight_TallDefense( i == 5 ? 1 : 0);// Tall Defense
        agent_weighted_vec.push_back(agent_weighted);
    }
    game.ReplaceAgent(&agent_brain, 1); // Replace the agent to be checked
    if(use_random_agent)
        game.AttachAgent(&agent_random);
    else
        game.AttachAgent(&agent_pub_eval);
}

void BackgammonWorld::evaluateSolo(std::shared_ptr<Organism> org, int analyze,
                             int visualize, int debug) {
  auto brain = org->brains[brainNamePL->get(PT)]; // std::shared_ptr<AbstractBrain>, I think
    agent_brain.InsertBrain(brain);
  for (int r = 0; r < evaluationsPerGenerationPL->get(PT); r++) {
    // Reattach brain and PubEval agents
    game.ReplaceAgent(&agent_brain, 1);
    if(use_random_agent)
        game.ReplaceAgent(&agent_random, 2);
    else
        game.ReplaceAgent(&agent_pub_eval, 2);
    // Run some matches! 
    size_t starts_agent_1 = 0;
    size_t starts_agent_2 = 0;
    size_t wins_agent_1 = 0;
    size_t wins_agent_2 = 0;
    for(size_t match_id = 0; match_id < num_matches; ++match_id){
        game.Restart();
        if(game.GetState().cur_agent == 1)
            starts_agent_1++;
        else if(game.GetState().cur_agent == 2)
            starts_agent_2++;
        
        game.Start();
        while(!game.GetState().game_over){
            game.Step();
        }
        if(game.GetState().winner_id == 1)
            wins_agent_1++;
        else if(game.GetState().winner_id == 2)
            wins_agent_2++;
    }
    // Set score where needed
    double score = ((double)wins_agent_1) / ((double)(num_matches));
    org->dataMap.append("score", 0.0);
    org->dataMap.append("winRate", score * score_scaling_factor);
    if (visualize)
      std::cout << "organism with ID " << org->ID << " scored " << score
                << std::endl;
    // Set up for prediction scoring
    game.ReplaceAgent(&agent_random_2, 1); // We just need a way to advance the turns
    game.Restart();
    size_t num_turns = 0;
    std::vector<double> num_correct_vec;
    std::vector<double> avg_prediction_score_vec;
    avg_prediction_score_vec.resize(weight_name_vec.size(), 0);
    size_t brain_move_idx;
    size_t brain_agent_idx;
    std::pair<size_t, size_t> brain_results;
    BackgammonState state;
    for(size_t i = 0; i < num_prediction_matches; ++i){
        brain->resetBrain();
        game.Restart();
        game.Start();
        num_turns = 0;
        num_correct_vec.clear();
        num_correct_vec.resize(weight_name_vec.size(), 0);
        state = game.GetState();
        while(!state.game_over){
            if(state.cur_agent == 1){
                num_turns++;
                brain_results = agent_brain.GetMoveInfo(state);
                brain_move_idx = brain_results.first;
                brain_agent_idx = brain_results.second;
                if(agent_weighted_vec[brain_agent_idx].GetMoveIdx(state) == brain_move_idx)
                    num_correct_vec[brain_agent_idx] += 1.0;
                game.Step();   
                state = game.GetState();
            }
            else{
                game.Step();
                state = game.GetState();
            }
        }
        for(size_t weight_idx = 0; weight_idx < weight_name_vec.size(); weight_idx++)
            avg_prediction_score_vec[weight_idx] += num_correct_vec[weight_idx] / num_turns;
    }
    for(size_t weight_idx = 0; weight_idx < weight_name_vec.size(); weight_idx++){
        org->dataMap.append(weight_name_vec[weight_idx], 
                avg_prediction_score_vec[weight_idx] / num_prediction_matches);
    }
  }
}

void BackgammonWorld::evaluate(std::map<std::string, std::shared_ptr<Group>> &groups,
                      int analyze, int visualize, int debug) {
  int popSize = groups[groupNamePL->get(PT)]->population.size();
  for (int i = 0; i < popSize; i++) {
    evaluateSolo(groups[groupNamePL->get(PT)]->population[i], analyze,
                 visualize, debug);
  }
}

std::unordered_map<std::string, std::unordered_set<std::string>>
BackgammonWorld::requiredGroups() {
  return {{groupNamePL->get(PT),
        {"B:" + brainNamePL->get(PT) + ",24,7"}}};
  // requires a root group and a brain (in root namespace) and no addtional
  // genome,
  // the brain must have 1 input, and the variable numberOfOutputs outputs
}



