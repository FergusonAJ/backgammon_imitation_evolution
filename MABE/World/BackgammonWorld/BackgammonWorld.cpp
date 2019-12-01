//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

// Evaluates agents on how many '1's they can output. This is a purely fixed
// task
// that requires to reactivity to stimuli.
// Each correct '1' confers 1.0 point to score, or the decimal output determined
// by 'mode'.

// Local
#include "BackgammonWorld.h"
#include "../../../src/backgammon.h"
#include "../../../src/agent_brain.h"
#include "../../../src/agent_weighted.h"
#include "../../../src/agent_pubeval.h"
// Empirical
// Standard Lib


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
}

void BackgammonWorld::evaluateSolo(std::shared_ptr<Organism> org, int analyze,
                             int visualize, int debug) {
  auto brain = org->brains[brainNamePL->get(PT)]; // std::shared_ptr<AbstractBrain>, I think
  for (int r = 0; r < evaluationsPerGenerationPL->get(PT); r++) {
    // Create the game and agents
    BackgammonGame game;
    game.SetSeed(Global::randomSeedPL->get());
    BackgammonAgent_Brain agent_brain(brain);
    BackgammonAgent_PubEval agent_2; 
    // Setup and attach agents
    agent_brain.SetRandomSeed(Global::randomSeedPL->get());
    game.AttachAgent(&agent_brain);
    //agent_2.SetRandomSeed(Random::getInt(100,1000000000));
    game.AttachAgent(&agent_2);
   
    size_t starts_agent_1 = 0;
    size_t starts_agent_2 = 0;
    size_t wins_agent_1 = 0;
    size_t wins_agent_2 = 0;
    //Run the matches!
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
    org->dataMap.append("score", score * score_scaling_factor);
    if (visualize)
      std::cout << "organism with ID " << org->ID << " scored " << score
                << std::endl;
    std::vector<std::string> weight_name_vec {"mostForward", "avgForward", "leastForward", 
        "aggressive", "wideDefense", "tallDefense"};
    BackgammonAgent_Weighted agent_weighted;
    agent_weighted.SetRandomSeed(Global::randomSeedPL->get());
    game.ReplaceAgent(&agent_weighted, 1);
    game.Restart();
    size_t num_turns = 0;
    double num_correct = 0;
    double avg_prediction_score = 0;
    size_t brain_predicted_move_idx;
    BackgammonState state;
    for(size_t weight_idx = 0; weight_idx < weight_name_vec.size(); weight_idx++){
        // Setup the correct opponent
        agent_weighted.SetWeight_MostForward(weight_name_vec[weight_idx]  == "mostForward" ? 1 : 0);
        agent_weighted.SetWeight_AvgForward(weight_name_vec[weight_idx]   == "avgForward"  ? 1 : 0);
        agent_weighted.SetWeight_LeastForward(weight_name_vec[weight_idx] == "leastForward"? 1 : 0);
        agent_weighted.SetWeight_Aggressive(weight_name_vec[weight_idx]   == "aggressive"   ? 1 : 0);
        agent_weighted.SetWeight_WideDefense(weight_name_vec[weight_idx]  == "wideDefense" ? 1 : 0);
        agent_weighted.SetWeight_TallDefense(weight_name_vec[weight_idx]  == "tallDefense" ? 1 : 0);
        avg_prediction_score = 0;
        for(size_t i = 0; i < num_prediction_matches; ++i){
            brain->resetBrain();
            game.Restart();
            game.Start();
            num_turns = 0;
            num_correct = 0;
            state = game.GetState();
            while(!state.game_over){
                if(state.cur_agent == 1){
                    num_turns++;
                    brain_predicted_move_idx = agent_brain.GetMoveIdx(state);
                    game.Step();   
                    state = game.GetState();
                    if(state.previous_move_idx == brain_predicted_move_idx)
                        num_correct += 1;
                }
                else{
                    game.Step();
                    state = game.GetState();
                }
            }
            avg_prediction_score += num_correct / num_turns;
        }
        org->dataMap.append(weight_name_vec[weight_idx], avg_prediction_score / num_prediction_matches);
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
        {"B:" + brainNamePL->get(PT) + ",24,1"}}};
  // requires a root group and a brain (in root namespace) and no addtional
  // genome,
  // the brain must have 1 input, and the variable numberOfOutputs outputs
}



