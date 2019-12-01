////    Simple agent that picks moves based on weighted criteria
////    Part of a term project for MSU's CSE 841 - Artificial Intelligence
////    Austin Ferguson - 2019

#ifndef BACKGAMMON_AGENT_WEIGHTED_H
#define BACKGAMMON_AGENT_WEIGHTED_H

//Local 
#include "./agent_base.h"
//Empirical
#include "tools/Random.h"
//Standard
#include <string>
#include <iostream>

class BackgammonAgent_Weighted : public BackgammonAgent_Base{
private: 
    emp::Random rand;
    bool rand_seeded = false;   
    emp::vector<double> score_vec;

    // Score Weights
    double weight_most_forward = 0;
    double weight_avg_forward = 0;
    double weight_least_forward = 0;
    double weight_aggressive = 1;
    double weight_wide_defense = 0;
    double weight_tall_defense = 0;
    
    virtual void PromptTurn(
            const BackgammonState& state, 
            std::function<void(BackgammonMove)> callback) override{
        if(!rand_seeded){
            std::cout << std::endl;
            std::cout << std::endl;
            std::cout << "ERROR: Random agent's random seed was not set." << std::endl;
            exit(-1);
        }
        size_t idx = ScoreMoves(state);
        callback(state.possible_moves[idx]);
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
            cur_score = 0;
            cur_score += Score_MostForward(state, new_state) * weight_most_forward;
            cur_score += Score_AvgForward(state, new_state) * weight_avg_forward;
            cur_score += Score_LeastForward(state, new_state) * weight_least_forward;
            cur_score += Score_Aggressive(state, new_state) * weight_aggressive;
            cur_score += Score_WideDefense(state, new_state) * weight_wide_defense;
            cur_score += Score_TallDefense(state, new_state) * weight_tall_defense;
            if(cur_score > max_score){
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
    double Score_MostForward(const BackgammonState& old_state, const BackgammonState& new_state){
        size_t max_idx = 0;
        if(agent_id == 1){
            if(new_state.tokens_finished_agent_1 > old_state.tokens_finished_agent_1)
                return 1.0;
            for(size_t i = 1; i < 25; ++i){
                if(new_state.tokens_per_point[i] > old_state.tokens_per_point[i])
                    max_idx = i;
            }
            return max_idx / 25;
        }
        else if(agent_id == 2){
            if(new_state.tokens_finished_agent_2 > old_state.tokens_finished_agent_2)
                return 1.0;
            for(size_t i = 24; i > 0; --i){
                if(new_state.tokens_per_point[i] < old_state.tokens_per_point[i])
                    max_idx = i;
            }
            return (25 - max_idx) / 25.0;
        }
        else{
            std::cerr << "ERROR: Agent id not set!" << std::endl;
            exit(-1);
            return 0;
        }
    }
    double Score_AvgForward(const BackgammonState& old_state, const BackgammonState& new_state){
        double sum = 0;
        if(agent_id == 1){
            for(size_t i = 0; i < 25; ++i){
                if(new_state.tokens_per_point[i] > 0){
                    sum += new_state.tokens_per_point[i] * (i * i / 625.0);
                }
            }
            return sum / 15;
        } 
        else if(agent_id == 2){
            for(size_t i = 24; i > 0; --i){
                if(new_state.tokens_per_point[i] < 0){
                    sum += -1 * new_state.tokens_per_point[i] * (((25 - i) * (25 - 1))/ 625.0);
                }
            }
            return sum / 15;
        }
        else{
            std::cerr << "ERROR: Agent id not set!" << std::endl;
            exit(-1);
            return 0;
        }
    }
    double Score_LeastForward(const BackgammonState& old_state, const BackgammonState& new_state){
        size_t min_idx = 0;
        if(agent_id == 1){
            for(size_t i = 24; i > 0; --i){
                if(new_state.tokens_per_point[i] > old_state.tokens_per_point[i])
                    min_idx = i;
            }
            return min_idx / 25;
        }
        else if(agent_id == 2){
            for(size_t i = 1; i < 25; ++i){
                if(new_state.tokens_per_point[i] < old_state.tokens_per_point[i])
                    min_idx = i;
            }
            return (25 - min_idx) / 25.0;
        }
        else{
            std::cerr << "ERROR: Agent id not set!" << std::endl;
            exit(-1);
            return 0;
        }
    }
    double Score_WideDefense(const BackgammonState& old_state, const BackgammonState& new_state){
        size_t num_defended = 0;
        if(agent_id == 1){
            for(size_t i = 0; i < 25; ++i){
                if(new_state.tokens_per_point[i] >= 2)
                    num_defended++;
            }
            return num_defended / 25.0;
        } 
        else if(agent_id == 2){
            for(size_t i = 0; i < 25; ++i){
                if(new_state.tokens_per_point[i] <= -2)
                    num_defended++;
            }
            return num_defended / 25.0;
        }
        else{
            std::cerr << "ERROR: Agent id not set!" << std::endl;
            exit(-1);
            return 0;
        }
    }
    double Score_TallDefense(const BackgammonState& old_state, const BackgammonState& new_state){
        int largest_pile = 0;
        if(agent_id == 1){
            for(size_t i = 0; i < 25; ++i){
                if(new_state.tokens_per_point[i] >= largest_pile)
                    largest_pile = new_state.tokens_per_point[i];
            }
            return largest_pile / 15.0;
        } 
        else if(agent_id == 2){
            for(size_t i = 0; i < 25; ++i){
                if(new_state.tokens_per_point[i] <= largest_pile)
                    largest_pile = new_state.tokens_per_point[i];
            }
            return largest_pile * -1 / 15.0;
        }
        else{
            std::cerr << "ERROR: Agent id not set!" << std::endl;
            exit(-1);
            return 0;
        }
    } 
    /*
    double Score_Template(const BackgammonState& old_state, const BackgammonState& new_state){
        if(agent_id == 1){
        } 
        else if(agent_id == 2){
        }
        else{
            std::cerr << "ERROR: Agent id not set!" << std::endl;
            exit(-1);
            return 0;
        }
    }
    */
    double Score_Aggressive(const BackgammonState& old_state, const BackgammonState& new_state){
        if(agent_id == 1){
            if(new_state.tokens_off_agent_2 > old_state.tokens_off_agent_2)
                return 1.0;
            return 0.0;
        } 
        else if(agent_id == 2){
            if(new_state.tokens_off_agent_1 > old_state.tokens_off_agent_1)
                return 1.0;
            return 0.0;
        }
        else{
            std::cerr << "ERROR: Agent id not set!" << std::endl;
            exit(-1);
            return 0;
        }
    }
public :
    BackgammonAgent_Weighted():
        rand(){ 
    }

    void SetRandomSeed(int seed){
        rand.ResetSeed(seed);
        rand_seeded = true;
    }
    void SetWeight_MostForward(double d){
        weight_most_forward = d;
    }
    void SetWeight_AvgForward(double d){
        weight_avg_forward = d;
    }
    void SetWeight_LeastForward(double d){
        weight_least_forward = d;
    }
    void SetWeight_Aggressive(double d){
        weight_aggressive = d;
    }
    void SetWeight_WideDefense(double d){
        weight_wide_defense = d;
    }
    void SetWeight_TallDefense(double d){
        weight_tall_defense = d;
    }
    
};

#endif
