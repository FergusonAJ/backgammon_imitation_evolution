////    Quick and simple Backgammon game for testing new agents
////    Part of a term project for MSU's CSE 841 - Artificial Intelligence
////    Austin Ferguson - 2019

// Local
#include "./backgammon.h"
#include "./agent_terminal.h"
#include "./agent_random.h"
#include "./agent_weighted.h"
//Empirical
#include "tools/Random.h"

//MyAnimate player;
//BackgammonAgent_Terminal agent_1;
BackgammonAgent_Weighted agent_1;
BackgammonAgent_Weighted agent_2;
BackgammonGame game;
int main(){
    emp::Random random;
    
    size_t num_iters = 10000;
    size_t wins_agent_1 = 0;
    size_t wins_agent_2 = 0;
    size_t starts_agent_1 = 0;
    size_t starts_agent_2 = 0;
    double avg_off_agent_1 = 0;
    double avg_off_agent_2 = 0;
    double avg_finished_agent_1 = 0;
    double avg_finished_agent_2 = 0;

    agent_1.SetRandomSeed(random.GetUInt());
    game.AttachAgent(&agent_1);
    agent_2.SetRandomSeed(random.GetUInt());
    game.AttachAgent(&agent_2);
    
    agent_1.SetWeight_MostForward(  0.0);
    agent_1.SetWeight_AvgForward(   0.0);
    agent_1.SetWeight_LeastForward( 1.0);
    agent_1.SetWeight_Aggressive(   0.0);
    agent_1.SetWeight_WideDefense(  0.0);
    agent_1.SetWeight_TallDefense(  0.0);

    agent_2.SetWeight_MostForward(  0.0);
    agent_2.SetWeight_AvgForward(   0.0);
    agent_2.SetWeight_LeastForward( 0.0);
    agent_2.SetWeight_Aggressive(   0.0);
    agent_2.SetWeight_WideDefense(  0.0);
    agent_2.SetWeight_TallDefense(  0.0);

    for(size_t game_id = 0; game_id < num_iters; ++game_id){
        game.Restart();
        if(game.GetState().cur_agent == 1)
            starts_agent_1++;
        else if(game.GetState().cur_agent == 2)
            starts_agent_2++;
        
        game.Start();
        double off_agent_1 = 0;
        double off_agent_2 = 0;
        double finished_agent_1 = 0;
        double finished_agent_2 = 0;
        while(!game.GetState().game_over){
            game.Step();
            off_agent_1 += game.GetState().tokens_off_agent_1;
            off_agent_2 += game.GetState().tokens_off_agent_2;
            finished_agent_1 += game.GetState().tokens_finished_agent_1;
            finished_agent_2 += game.GetState().tokens_finished_agent_2;
        }
        if(game.GetState().winner_id == 1)
            wins_agent_1++;
        else if(game.GetState().winner_id == 2)
            wins_agent_2++;
        avg_off_agent_1 += (off_agent_1 / game.GetState().turn_count) / num_iters;
        avg_off_agent_2 += (off_agent_2 / game.GetState().turn_count) / num_iters;
        avg_finished_agent_1 += (finished_agent_1 / game.GetState().turn_count) / num_iters;
        avg_finished_agent_2 += (finished_agent_2 / game.GetState().turn_count) / num_iters;
    }
    std::cout << "Agent 1 starts: " << starts_agent_1 << std::endl;
    std::cout << "Agent 2 starts: " << starts_agent_2 << std::endl;
    std::cout << "Agent 1 wins: " << wins_agent_1 << std::endl;
    std::cout << "Agent 2 wins: " << wins_agent_2 << std::endl;
    std::cout << "Agent 1 average off: " << avg_off_agent_1 << std::endl;
    std::cout << "Agent 2 average off: " << avg_off_agent_2 << std::endl;
    std::cout << "Agent 1 average finished: " << avg_finished_agent_1 << std::endl;
    std::cout << "Agent 2 average finished: " << avg_finished_agent_2 << std::endl;
    return 0;
}
