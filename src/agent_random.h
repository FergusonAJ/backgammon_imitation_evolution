////    Extremely simple agent that picks moves randomly
////    Part of a term project for MSU's CSE 841 - Artificial Intelligence
////    Austin Ferguson - 2019

#ifndef BACKGAMMON_AGENT_RANDOM_H
#define BACKGAMMON_AGENT_RANDOM_H

//Local 
#include "./agent_base.h"
//Empirical
#include "tools/Random.h"
//Standard
#include <string>
#include <iostream>

class BackgammonAgent_Random : public BackgammonAgent_Base{
private: 
    emp::Random rand;
    bool rand_seeded = false;   
    virtual void PromptTurn(
            const BackgammonState& state, 
            std::function<void(BackgammonMove)> callback) override{
        if(!rand_seeded){
            std::cout << std::endl;
            std::cout << std::endl;
            std::cerr << "ERROR: Random agent's random seed was not set." << std::endl;
            exit(-1);
        }
        callback(state.possible_moves[rand.GetUInt(0, state.possible_moves.size())]);
    }

    virtual void Notify(
            const emp::vector<BackgammonMove>& moves, const emp::vector<int>& dice_vals) override{
    }

public :
    BackgammonAgent_Random():
        rand(){ 
    }

    void SetRandomSeed(int seed){
        rand.ResetSeed(seed);
        rand_seeded = true;
    }
};


#endif
