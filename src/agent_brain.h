////    Uses a brain from MABE to choose the next move
////    Part of a term project for MSU's CSE 841 - Artificial Intelligence
////    Austin Ferguson - 2019

#ifndef BACKGAMMON_AGENT_BRAIN_H
#define BACKGAMMON_AGENT_BRAIN_H

//Local 
#include "./agent_base.h"
#include "../MABE/Brain/AbstractBrain.h"
//Empirical
#include "tools/Random.h"
//Standard
#include <string>
#include <iostream>
#include <memory>

class BackgammonAgent_Brain : public BackgammonAgent_Base{
private: 
    emp::Random rand;
    bool rand_seeded = false;   
    std::shared_ptr<AbstractBrain> brain_ptr;
    virtual void PromptTurn(
            const BackgammonState& state, 
            std::function<void(BackgammonMove)> callback) override{
        if(!rand_seeded){
            std::cout << std::endl;
            std::cout << std::endl;
            std::cerr << "ERROR: Random agent's random seed was not set." << std::endl;
            exit(-1);
        }
        
        callback(state.possible_moves[ScoreMoves(state)]);
    }
    virtual void Notify(
            const emp::vector<BackgammonMove>& moves, const emp::vector<int>& dice_vals) override{
    }
    size_t ScoreMoves(const BackgammonState& state){
        double cur_score = 0;
        double max_score = 0;
        std::vector<size_t> max_val_indices; 
        for(size_t move_idx = 0; move_idx < state.possible_moves.size(); ++move_idx){
            BackgammonState new_state = test_move_func(state, state.possible_moves[move_idx]); 
            brain_ptr->resetBrain();
            LoadBrainInput(new_state);
            brain_ptr->update();
            cur_score = brain_ptr->readOutput(0);
            if(cur_score > max_score | max_val_indices.size() == 0){
                max_score = cur_score;
                max_val_indices.clear();
                max_val_indices.push_back(move_idx);
            }
            else if(cur_score == max_score){
                max_val_indices.push_back(move_idx);
            }
        }
        size_t idx = rand.GetUInt(0, max_val_indices.size());
        return max_val_indices[idx];
    }
    void LoadBrainInput(const BackgammonState& state){
        for(size_t i = 1; i < 25; ++i){
            brain_ptr->setInput(i - 1, state.tokens_per_point[i] / 15.0);
        }
    }

public :
    BackgammonAgent_Brain(std::shared_ptr<AbstractBrain> ptr):
        rand(),
        brain_ptr(ptr){ 
    }
    void SetRandomSeed(int seed){
        rand.ResetSeed(seed);
        rand_seeded = true;
    }
    size_t GetMoveIdx(BackgammonState& state){
        if(!rand_seeded){
            std::cout << std::endl;
            std::cout << std::endl;
            std::cerr << "ERROR: Random agent's random seed was not set." << std::endl;
            exit(-1);
        }
        return ScoreMoves(state);
    }
};


#endif
