////    Basic Backgammon game controller
////    Part of a term project for MSU's CSE 841 - Artificial Intelligence
////    Austin Ferguson - 2019

#ifndef BACKGAMMON_GAME_H
#define BACKGAMMON_GAME_H

//Local 
#include "./agent_base.h"
#include "./backgammon_state.h"
//#include "./agent_human.h"
//Empirical
#include "tools/Random.h"
#include "base/Ptr.h"
//Standard
#include <cmath>
#include <string>
#include <sstream>

#define PRINT_OUTPUT false
 
class BackgammonGame{
private:
    emp::Random rand;
    BackgammonState state;
    emp::Ptr<BackgammonAgent_Base> agent_1 = nullptr;
    emp::Ptr<BackgammonAgent_Base> agent_2 = nullptr;
    bool play_until_end = false;
    
    // Place all tokens in their starting configurations
    void InitializeTokens(){
        //25 so we have one empty value and can index at 1(as is std)
        state.tokens_per_point.resize(25, 0); 
        // Player 1
        state.tokens_per_point[1]  =  2;
        state.tokens_per_point[12] =  5;
        state.tokens_per_point[17] =  3;
        state.tokens_per_point[19] =  5;
        // Player 2
        state.tokens_per_point[24] =  -2;
        state.tokens_per_point[13] =  -5;
        state.tokens_per_point[8] =   -3;
        state.tokens_per_point[6] =   -5;
    }
    // DEBUG METHOD: Place all tokens such that bearoff is easy to reach
    void InitializeTokens_Bearoff(){
        //25 so we have one empty value and can index at 1(as is std)
        state.tokens_per_point.resize(25, 0); 
        // Player 1
        state.tokens_per_point[18]  =  2;
        state.tokens_per_point[20] =  5;
        state.tokens_per_point[21] =  3;
        state.tokens_per_point[19] =  5;
        // Player 2
        state.tokens_per_point[7] =  -2;
        state.tokens_per_point[5] =  -5;
        state.tokens_per_point[4] =   -3;
        state.tokens_per_point[6] =   -5;
    }
    // Roll two d6's.
    void RollDice(){
        state.dice_vals[0] = rand.GetUInt(1, 7);
        state.dice_vals[1] = rand.GetUInt(1, 7);
        if(state.dice_vals[0] == state.dice_vals[1]){
            state.dice_vals[2] = state.dice_vals[0];
            state.dice_vals[3] = state.dice_vals[0];
        }
        else{
            state.dice_vals[2] = 0; 
            state.dice_vals[3] = 0;
        }
        if(PRINT_OUTPUT){
            std::cout << "Dice: ";
            for(size_t i = 0; i < state.dice_vals.size(); ++i)
                std::cout << state.dice_vals[i] << " ";
            std::cout << std::endl;
        }
    }
    // Sent as a callback to agents
    void HandleMove(BackgammonMove move){
        //std::cout << "Agent " << state.cur_agent << ": "  << move.start << " -> " << move.end << std::endl;
        if(move.start > 0 && move.end > 0)
            SpendDice(move.start > move.end ? move.start - move.end : move.end - move.start, state);
        else if(move.start < 0)
            SpendDice(move.start * -1, state);
        else if(move.end < 0)
            SpendDice(move.end * -1, state);
        MoveToken(move, state);
        agent_1->Notify({move}, state.dice_vals);
        agent_2->Notify({move}, state.dice_vals);
        if(!AdvanceStep()){
            if(PRINT_OUTPUT)
                PrintBoard(state); 
            return;
        }
        if(PRINT_OUTPUT)
            PrintBoard(state); 
        if(GetRemainingTokenCount(1, state) > 15 || GetRemainingTokenCount(2, state) > 15){
            std::cout << "ERROR: Extra tokens on the board!" << std::endl;
            exit(-1);
        }
        if(play_until_end)
            NextTurn();
    }
    // Computes the output of a move WITHOUT reflecting it in the actual game state
    BackgammonState TryMove(const BackgammonState& s, const BackgammonMove& move){
        BackgammonState new_state = BackgammonState(s);
        if(move.start > 0 && move.end > 0)
            SpendDice(move.start > move.end ? move.start - move.end : move.end - move.start, new_state);
        else if(move.start < 0)
            SpendDice(move.start * -1, new_state);
        else if(move.end < 0)
            SpendDice(move.end * -1, new_state);
        MoveToken(move, new_state);
        if(GetRemainingTokenCount(1, new_state) == 0 || GetRemainingTokenCount(2, new_state) == 0){
            FinishGame(new_state, true);
        }
        return new_state;
        
    }
    // Queries the next player for a move
    void NextTurn(){ 
        if(state.cur_agent == 1){
            agent_1->PromptTurn(state, [this](BackgammonMove move) mutable{
                    this->HandleMove(move);
                });
        }
        else{
            agent_2->PromptTurn(state, [this](BackgammonMove move) mutable{
                    this->HandleMove(move);
                });
        }
    }
    // Update's bookkeeping variables appropriately after a move has been made
    //     Returns true if play should continue
    bool AdvanceStep(){
        state.turn_count++;
        // If an agent has finished all tokens, stop!
        if(GetRemainingTokenCount(1, state) == 0 || GetRemainingTokenCount(2, state) == 0){
            FinishGame(state);
            return false;
        }
        // If all dice have been used, next turn!
        if(state.dice_vals[0] <= 0 && state.dice_vals[1] <= 0 && 
                state.dice_vals[2] <= 0 && state.dice_vals[3] <= 0){
            RollDice();
            state.cur_agent = (state.cur_agent == 1 ? 2 : 1);
            if(PRINT_OUTPUT){
                std::cout << std::endl << "######## Agent #" << state.cur_agent << 
                    " ########" << std::endl; 
            }
        }
        UpdatePossibleMoves();
        state.in_bearoff_agent_1 = CheckBearoff(state, 1);
        state.in_bearoff_agent_2 = CheckBearoff(state, 2);
        // If that agent can't make a move, try the other until we get something that works!
        while(state.possible_moves.size() == 0){
            state.turn_count++;
            RollDice();
            state.cur_agent = (state.cur_agent == 1 ? 2 : 1);
            if(PRINT_OUTPUT){
                std::cout << std::endl << "######## Agent #" << state.cur_agent << 
                    " ########" << std::endl; 
            }
            UpdatePossibleMoves();
            state.in_bearoff_agent_1 = CheckBearoff(state, 1);
            state.in_bearoff_agent_2 = CheckBearoff(state, 2);
        }
        return true; 
    }
    // Returns the number of tokens still in play (not finished) for the given player
    size_t GetRemainingTokenCount(size_t agent_id, BackgammonState& s){
        size_t count = 0;
        if(agent_id == 1){
            for(size_t point_idx = 1; point_idx < s.tokens_per_point.size(); ++point_idx){
                if(s.tokens_per_point[point_idx] > 0)
                    count += s.tokens_per_point[point_idx];
            }
            return count + s.tokens_off_agent_1;
        }
        else if(agent_id == 2){
            for(size_t point_idx = 1; point_idx < s.tokens_per_point.size(); ++point_idx){
                if(s.tokens_per_point[point_idx] < 0)
                    count += -1 * s.tokens_per_point[point_idx];
            }
            return count + s.tokens_off_agent_2;
        }
        else{
            std::cerr << "ERROR: Asked to count tokens for agent #" << agent_id
                << ", should always be 1 or 2!" << std::endl;
            exit(-1);
        }
    }
    // Prints a banner for whoever won 
    void FinishGame(BackgammonState& s, bool suppress_prints = false){
        s.game_over = true;
        if(PRINT_OUTPUT && !suppress_prints){
            std::cout << std::endl;
            std::cout << std::endl;
            std::cout << std::endl;
        }
        if(GetRemainingTokenCount(1, s) == 0){
            s.winner_id = 1;
            if(PRINT_OUTPUT && !suppress_prints){
                std::cout << "#################################" << std::endl;
                std::cout << "########  Agent 1 wins!  ########" << std::endl;
                std::cout << "#################################" << std::endl;
            }
        }
        else if(GetRemainingTokenCount(2, s) == 0){
            s.winner_id = 2;
            if(PRINT_OUTPUT && !suppress_prints){
                std::cout << "#################################" << std::endl;
                std::cout << "########  Agent 2 wins!  ########" << std::endl;
                std::cout << "#################################" << std::endl;
            }
        }
        else{
            std::cerr << "ERROR: FinishGame() called with no winner..." << std::endl;
            exit(-1);
        }
    }
    // Actually moves a token (checker??) from start to end points
    void MoveToken(const BackgammonMove& move, BackgammonState& s){
        int diff = (s.cur_agent == 1) ? 1 : -1;
        // If we're not moving a token onto the board subtract a token from the start point
        if(move.start > 0) 
            s.tokens_per_point[move.start] -= diff;
        else{ // If we are moving a token onto the board, make sure we keep track of it
            if(s.cur_agent == 1)
                s.tokens_off_agent_1--;
            else if(s.cur_agent == 2)
                s.tokens_off_agent_2--;
            else{
                std::cerr << "ERROR: current agent set to " << s.cur_agent;
                std::cout << ", it should always be 1 or 2." << std::endl;
            }
        }
        // If the goal point has an enemy token in it, knock it off the board
        if(move.end > 0 && s.tokens_per_point[move.end] == -1 * diff){
            s.tokens_per_point[move.end] = diff; 
            if(diff == 1)
                s.tokens_off_agent_2++; 
            else
                s.tokens_off_agent_1++; 
        }
        // Add token to end point
        else if(move.end > 0){
            s.tokens_per_point[move.end] += diff;
        }
        // If we are moving a token off the board (into the "end zone"), make sure to count it
        else{
            if(s.cur_agent == 1)
                s.tokens_finished_agent_1++;
            else if(s.cur_agent == 2)
                s.tokens_finished_agent_2++;
            else{
                std::cerr << "ERROR: current agent set to " << s.cur_agent;
                std::cerr << ", it should always be 1 or 2." << std::endl;
            }
        }
    }  
    // Helper method to get a nice formatted string for a token count (for +/- and padding mostly)
    std::string GetTokenCountString(int count){
        std::ostringstream oss;
        if(count > 0)
            oss << "+" << count;
        else if(count < 0)
            oss << count;
        else
            return "00";
        return oss.str();
           
    }
    // Converts the dice as it's stored to something more useful
    emp::vector<int> GetDiceVals(){
        emp::vector<int> vals(state.dice_vals);
        return vals; 
    } 
    // Figures out all legal moves from the current state
    void UpdatePossibleMoves(){
        state.possible_moves.clear();
        emp::vector<size_t> used_die_mask; // Used to not duplicate moves
        used_die_mask.resize(7, 0);
        // First, check to see if any tokens have been knocked off the board
        //     If so, putting one back on the board is the only possible type of move  
        if(state.cur_agent == 1 && state.tokens_off_agent_1 > 0){
            for(size_t die_idx = 0; die_idx < 4; ++die_idx){
                int die_val = state.dice_vals[die_idx];
                if(die_val > 0 && !used_die_mask[die_val]){
                    if(state.tokens_per_point[die_val] >= -1){
                        state.possible_moves.emplace_back(-1 * die_val, die_val);
                        used_die_mask[die_val] = 1;
                    }
                }
            }           
        }
        else if(state.cur_agent == 2 && state.tokens_off_agent_2 > 0){
            for(size_t die_idx = 0; die_idx < 4; ++die_idx){
                int die_val = state.dice_vals[die_idx];
                if(die_val > 0 && !used_die_mask[die_val]){
                    if(state.tokens_per_point[25 - die_val] <= 1){
                        state.possible_moves.emplace_back(-1 * die_val, 25 - die_val);
                        used_die_mask[die_val] = 1;
                    }   
                }
            }           
        }
        else{
            // Checks to see if a token can be moved from each point (triangle) 
            for(int point_idx = 1; point_idx < 25; ++point_idx){
                // Weed out points that the current player does not control
                if(state.tokens_per_point[point_idx] == 0 ||
                        (state.cur_agent == 1 && state.tokens_per_point[point_idx] < 0) || 
                        (state.cur_agent == 2 && state.tokens_per_point[point_idx] > 0)) 
                    continue; 
                // Check each dice and point combo
                used_die_mask.clear();
                used_die_mask.resize(7, 0);
                for(size_t die_idx = 0; die_idx < 4; ++die_idx){
                    int die_val = state.dice_vals[die_idx];
                    if(die_val > 0 && !used_die_mask[die_val]){
                        if(state.cur_agent == 1){
                            if(point_idx + die_val <= 24 && 
                                    state.tokens_per_point[point_idx + die_val] >= -1){
                                state.possible_moves.emplace_back(point_idx, point_idx + die_val);
                                used_die_mask[die_val] = 1;
                            }
                            else if(point_idx + die_val > 24 && CheckBearoff(state)){
                                state.possible_moves.emplace_back(point_idx, -1 * die_val);
                                used_die_mask[die_val] = 1;
                            }
                        }
                        else{
                            if(point_idx - die_val > 0 && 
                                    state.tokens_per_point[point_idx - die_val] <= 1){
                                state.possible_moves.emplace_back(point_idx, point_idx - die_val);
                                used_die_mask[die_val] = 1;
                            } 
                            else if(point_idx - die_val <= 0 && CheckBearoff(state)){
                                state.possible_moves.emplace_back(point_idx, -1 * die_val);
                                used_die_mask[die_val] = 1;
                            }
                        }
                    }
                } 
            }
        }
    }
    // Spends the necessary dice to make a move
    void SpendDice(size_t dist, BackgammonState& s){
        for(size_t i = 4; i > 0; --i){
            if(s.dice_vals[i - 1] == (int)dist){
                s.dice_vals[i - 1] *= -1;
                break;
            }
        }
    }
    // Checks to see if the current agent is in the "bear-off" stage (all tokens in their home)
    bool CheckBearoff(const BackgammonState& s, int agent_id = -1){
        if(agent_id == -1)
            agent_id = s.cur_agent;
        if(agent_id == 1){
            if(s.tokens_off_agent_1 > 0)
                return false;
            for(size_t point_idx = 1; point_idx <= 18; ++point_idx){
                if(s.tokens_per_point[point_idx] > 0)
                    return false;
            }
        }
        else{
            if(s.tokens_off_agent_2 > 0)
                return false;
            for(size_t point_idx = 7; point_idx <= 24; ++point_idx){
                if(s.tokens_per_point[point_idx] < 0)
                    return false;
            }
        }
        return true;
    }   

public:
    // Default constructor - set up a game 
    BackgammonGame():
        rand(), 
        state(){
        state.dice_vals.resize(4, 0);
    }
    // Sets the RNG seed
    void SetSeed(int seed){
        rand.ResetSeed(seed);
    }
    // Resets all state variables for a brand new game
    void Restart(){
        state.tokens_per_point.clear();
        InitializeTokens();
        state.tokens_finished_agent_1 = 0;
        state.tokens_finished_agent_2 = 0;
        state.tokens_off_agent_1 = 0;
        state.tokens_off_agent_2 = 0;
        state.game_over = false;
        state.winner_id = -1;
        RollDice();
        while(state.dice_vals[0] == state.dice_vals[1])
            RollDice();
        state.cur_agent = (state.dice_vals[0] > state.dice_vals[1]) ? 1 : 2;
        if(PRINT_OUTPUT)
            std::cout << std::endl <<"######## Agent #" <<state.cur_agent <<" ########" <<std::endl; 
        RollDice();
    }
    // Attaches an agent to play the game
    bool AttachAgent(BackgammonAgent_Base* agent){
        if(agent_1 == nullptr){
            agent_1 = agent;
            agent->SetAgentId(1);
        }
        else if(agent_2 == nullptr){
            agent_2 = agent;
            agent->SetAgentId(2);
        }
        else
            return false;
        agent->SetTestMoveFunc(
            [this](const BackgammonState& s, const BackgammonMove& move){
                return this->TryMove(s, move);
            }
        );
        return true;
    }
    // Attaches an agent to play the game
    void ReplaceAgent(BackgammonAgent_Base* agent, size_t agent_id){
        if(agent_id == 1){
            agent_1 = agent;
            agent->SetAgentId(1);
        }
        else{
            agent_2 = agent;
            agent->SetAgentId(2);
        }
        agent->SetTestMoveFunc(
            [this](const BackgammonState& s, const BackgammonMove& move){
                return this->TryMove(s, move);
            }
        );
    }
    // Starts the game!    
    void Start(bool b = false){
        play_until_end = b;
        UpdatePossibleMoves();
        if(PRINT_OUTPUT)
            PrintBoard(state);
        NextTurn();
    } 
    // Advance play by one turn 
    void Step(){
        NextTurn();
    }
    // Returns a copy of the current game state
    BackgammonState GetState(){
        BackgammonState new_state(state);
        return new_state;
    }
    // Prints the given board state
    void PrintBoard(const BackgammonState& s){
        // Print upper point labels
        for(size_t point_idx = 13; point_idx <= 18; ++point_idx)
            std::cout << point_idx << " ";
        std::cout << "XX ";
        for(size_t point_idx = 19; point_idx < 25; ++point_idx)
            std::cout << point_idx << " ";
        std::cout << std::endl;
        // Print upper token counts
        for(size_t point_idx = 13; point_idx <= 18; ++point_idx)
            std::cout << GetTokenCountString(s.tokens_per_point[point_idx]) << " ";
        std::cout << "XX ";
        for(size_t point_idx = 19; point_idx < 25; ++point_idx)
            std::cout << GetTokenCountString(s.tokens_per_point[point_idx]) << " ";
        std::cout << std::endl;
        std::cout << std::endl;
        std::cout << std::endl;
        // Print lower token counts
        for(size_t point_idx = 12; point_idx > 6; --point_idx)
            std::cout << GetTokenCountString(s.tokens_per_point[point_idx]) << " ";
        std::cout << "XX ";
        for(size_t point_idx = 6; point_idx > 0; --point_idx)
            std::cout << GetTokenCountString(s.tokens_per_point[point_idx]) << " ";
        std::cout << std::endl;
        //Print lower point labels
        for(size_t point_idx = 12; point_idx > 6; --point_idx)
            std::cout << (point_idx >= 10 ? "" : "0") << point_idx << " ";
        std::cout << "XX ";
        for(size_t point_idx = 6; point_idx > 0; --point_idx)
            std::cout << "0" << point_idx << " ";
        std::cout << std::endl;
        std::cout << "Agent 1: " << s.tokens_off_agent_1 << " off, " << 
                s.tokens_finished_agent_1 << " finished" << std::endl;
        std::cout << "Agent 2: " << s.tokens_off_agent_2 << " off, " << 
                s.tokens_finished_agent_2 << " finished" << std::endl;
        std::cout << std::endl;
    } 
};

#endif
