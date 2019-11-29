////    Simple human-controlled agent (All based out of the terminal)
////    Part of a term project for MSU's CSE 841 - Artificial Intelligence
////    Austin Ferguson - 2019

#ifndef BACKGAMMON_AGENT_TERMINAL_H
#define BACKGAMMON_AGENT_TERMINAL_H

//Local 
#include "./agent_base.h"
//Empirical

//Standard
#include <string>
#include <iostream>

class BackgammonAgent_Terminal : public BackgammonAgent_Base{
private:    
    virtual void PromptTurn(
            const BackgammonState& state, 
            std::function<void(BackgammonMove)> callback) override{
        std::cout << "Your turn!" << std::endl;
        std::cout << "Your dice roll: ";
        for(int x : state.dice_vals)
            std::cout << " " << x;
        std::cout << std::endl;
        std::cout << "Please select a move (via the index in []):" << std::endl;
        for(size_t i = 0; i < state.possible_moves.size(); ++i){
            std::cout << "[" << i + 1 << "] " << state.possible_moves[i].start  << " -> " <<
                    state.possible_moves[i].end << std::endl;
        }
        std::string input;
        int input_idx;
        std::cin >> input_idx;
        while(input_idx < 1 || input_idx > (int)state.possible_moves.size()){
            std::cout << "Unrecognized input. Please try again:" << std::endl; 
            if(std::cin.fail()){
                std::cin.clear();
                std::cin.ignore(INT_MAX, '\n');
            }
            std::cin >> input_idx;
        }
        input_idx -= 1;
        std::cout << "You selected move: [" << input_idx + 1 << "]" << 
                state.possible_moves[input_idx].start  << " -> " << 
                state.possible_moves[input_idx].end <<
                std::endl;
       callback(state.possible_moves[input_idx]); 
    }

    virtual void Notify(
            const emp::vector<BackgammonMove>& moves, const emp::vector<int>& dice_vals) override{
        std::cout << "Moves taken: " << std::endl; 
        for(const BackgammonMove& move : moves)
            std::cout << "\t" << move.start << " -> " << move.end << std::endl;
        std::cout << "Using dice:";
        for(int x : dice_vals)
            std::cout << " " << x;
        std::cout << std::endl;
    }

public :
    BackgammonAgent_Terminal(){ 
    }
};


#endif
