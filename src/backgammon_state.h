////    Basic Backgammon game state structure to pass info from class to class
////    Part of a term project for MSU's CSE 841 - Artificial Intelligence
////    Austin Ferguson - 2019

#ifndef BACKGAMMON_STATE_H
#define BACKGAMMON_STATE_H
//Local
//Empirical
#include "base/vector.h"
//Stabdard Library

struct BackgammonMove{
public:
    int start;
    int end;
    BackgammonMove(int start_, int end_):
        start(start_),
        end(end_){
    }
};

struct BackgammonState{
public:
    emp::vector<int> tokens_per_point;
    emp::vector<int> dice_vals;
    size_t tokens_finished_agent_1 = 0;
    size_t tokens_finished_agent_2 = 0;
    size_t tokens_off_agent_1 = 0;
    size_t tokens_off_agent_2 = 0;
    bool game_over = false;
    size_t cur_agent = 1;
    size_t turn_count = 0;
    int winner_id = -1;
    emp::vector<BackgammonMove> possible_moves;
    BackgammonState():
        tokens_per_point(),
        dice_vals(),
        possible_moves(){
    }
};


#endif