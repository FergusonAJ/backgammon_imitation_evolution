////    Base clas to derive all backgammon playing agents
////    Part of a term project for MSU's CSE 841 - Artificial Intelligence
////    Austin Ferguson - 2019

#ifndef BACKGAMMON_AGENT_BASE_H
#define BACKGAMMON_AGENT_BASE_H

// Local
#include "./backgammon_state.h"
// Empirical
#include "./base/Ptr.h"
#include "./base/vector.h"
// Standard Lib
#include <string>
#include <functional>

class BackgammonAgent_Base{
public:
    // Gives agent current state and asks for a callback with the agent's move
    virtual void PromptTurn(
            const BackgammonState& state, 
            std::function<void(BackgammonMove)> callback) = 0;
    // Tells the agent about a move that was made
    virtual void Notify(
            const emp::vector<BackgammonMove>& moves, const emp::vector<int>& dice_vals) = 0;
    // Compiler gives a warning if this isn't here
    virtual ~BackgammonAgent_Base(){
    }
};

#endif
