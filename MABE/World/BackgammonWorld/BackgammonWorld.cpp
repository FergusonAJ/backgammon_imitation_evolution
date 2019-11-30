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
#include "../../../src/agent_random.h"
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

BackgammonWorld::BackgammonWorld(std::shared_ptr<ParametersTable> PT_)
    : AbstractWorld(PT_) {

  // columns to be added to ave file
  popFileColumns.clear();
  popFileColumns.push_back("score");
  popFileColumns.push_back("score_VAR"); // specifies to also record the
                                         // variance (performed automatically
                                         // because _VAR)
}

void BackgammonWorld::evaluateSolo(std::shared_ptr<Organism> org, int analyze,
                             int visualize, int debug) {
  auto brain = org->brains[brainNamePL->get(PT)]; // std::shared_ptr<AbstractBrain>, I think
  for (int r = 0; r < evaluationsPerGenerationPL->get(PT); r++) {
    // Create the game and agents
    BackgammonGame game;
    game.SetSeed(Global::randomSeedPL->get());
    BackgammonAgent_Brain agent_1(brain);
    BackgammonAgent_PubEval agent_2; 
    // Setup and attach agents
    agent_1.SetRandomSeed(Global::randomSeedPL->get());
    game.AttachAgent(&agent_1);
    //agent_2.SetRandomSeed(Random::getInt(100,1000000000));
    game.AttachAgent(&agent_2);
   
    int num_matches = numberOfMatchesPL->get(PT); 
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
    double score = ((double)wins_agent_1) / ((double)(num_matches));
    /*
    brain->resetBrain();
    brain->setInput(0, 1); // give the brain a constant 1 (for wire brain)
    brain->update();
    double score = 0.0;
    for (int i = 0; i < brain->nrOutputValues; i++) {
      if (modePL->get(PT) == 0)
        score += Bit(brain->readOutput(i));
      else
        score += brain->readOutput(i);
    }
    if (score < 0.0)
      score = 0.0;
    */
    org->dataMap.append("score", score);
    if (visualize)
      std::cout << "organism with ID " << org->ID << " scored " << score
                << std::endl;
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



