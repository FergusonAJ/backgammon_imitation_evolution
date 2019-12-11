////    Quick and simple Backgammon game for testing new agents
////    Part of a term project for MSU's CSE 841 - Artificial Intelligence
////    Austin Ferguson - 2019

// Local
#include "./test_game_config.h"
#include "./backgammon.h"
#include "./agent_terminal.h"
#include "./agent_random.h"
#include "./agent_weighted.h"
#include "./agent_pubeval.h"
//Empirical
#include "tools/Random.h"
#include "config/ArgManager.h"
#include "config/command_line.h"


BackgammonAgent_Weighted    agent_1_weighted;
BackgammonAgent_PubEval     agent_1_pubeval;
BackgammonAgent_Terminal    agent_1_terminal;
BackgammonAgent_Random      agent_1_random;
BackgammonAgent_Weighted    agent_2_weighted;
BackgammonAgent_PubEval     agent_2_pubeval;
BackgammonAgent_Terminal    agent_2_terminal;
BackgammonAgent_Random      agent_2_random;
BackgammonGame game;

int main(int argc, char* argv[]){
    std::string config_fname = "test_game_config.cfg";
    auto args = emp::cl::ArgManager(argc, argv);
    std::string optional_filename;
    int filename_res = args.UseArg<std::string>("-f", optional_filename);
    if(filename_res == 1){
        config_fname = optional_filename;
    }
    BackgammonTestConfig config;
    config.Read(config_fname);
    std::cout << "Looking for config: " << config_fname << std::endl;
    if (args.ProcessConfigOptions(config, std::cout, config_fname, "config-macros.h") 
            == false) 
        exit(0);
    if (args.TestUnknown() == false) 
        exit(0); // If there are leftover args, throw an error. 

    // Write to screen how the experiment is configured
    std::cout << "==============================" << std::endl;
    std::cout << "|    Current configuration   |" << std::endl;
    std::cout << "==============================" << std::endl;
    config.Write(std::cout);
    std::cout << "==============================\n" << std::endl;

    // Set random seeds
    emp::Random random(config.SEED());    
    game.SetSeed(config.SEED());
    
    int agent_1_seed = config.AGENT_1_SEED();
    if(agent_1_seed < 0){
        agent_1_random.SetRandomSeed(config.SEED());
        agent_1_weighted.SetRandomSeed(config.SEED());
    }
    else{
        agent_1_random.SetRandomSeed(agent_1_seed);
        agent_1_weighted.SetRandomSeed(agent_1_seed);
    }
    int agent_2_seed = config.AGENT_2_SEED();
    if(agent_2_seed < 0){
        agent_2_random.SetRandomSeed(config.SEED());
        agent_2_weighted.SetRandomSeed(config.SEED());
    }
    else{    
        agent_2_random.SetRandomSeed(agent_1_seed);
        agent_2_weighted.SetRandomSeed(agent_1_seed);
    }
    
    // Set agent weights
    agent_1_weighted.SetWeight_MostForward(  config.AGENT_1_WEIGHT_MOST_FORWARD());
    agent_1_weighted.SetWeight_AvgForward(   config.AGENT_1_WEIGHT_AVG_FORWARD());
    agent_1_weighted.SetWeight_LeastForward( config.AGENT_1_WEIGHT_LEAST_FORWARD());
    agent_1_weighted.SetWeight_Aggressive(   config.AGENT_1_WEIGHT_AGGRESSIVE());
    agent_1_weighted.SetWeight_WideDefense(  config.AGENT_1_WEIGHT_WIDE_DEFENSE());
    agent_1_weighted.SetWeight_TallDefense(  config.AGENT_1_WEIGHT_TALL_DEFENSE());

    agent_2_weighted.SetWeight_MostForward(  config.AGENT_2_WEIGHT_MOST_FORWARD());
    agent_2_weighted.SetWeight_AvgForward(   config.AGENT_2_WEIGHT_AVG_FORWARD());
    agent_2_weighted.SetWeight_LeastForward( config.AGENT_2_WEIGHT_LEAST_FORWARD());
    agent_2_weighted.SetWeight_Aggressive(   config.AGENT_2_WEIGHT_AGGRESSIVE());
    agent_2_weighted.SetWeight_WideDefense(  config.AGENT_2_WEIGHT_WIDE_DEFENSE());
    agent_2_weighted.SetWeight_TallDefense(  config.AGENT_2_WEIGHT_TALL_DEFENSE());

    // Attach the specified agents
    std::string agent_1_type = config.AGENT_1_TYPE();
    if(agent_1_type == "RANDOM")
        game.AttachAgent(&agent_1_random);
    else if(agent_1_type == "TERMINAL")
        game.AttachAgent(&agent_1_terminal);
    else if(agent_1_type == "WEIGHTED")
        game.AttachAgent(&agent_1_weighted);
    else if(agent_1_type == "PUBEVAL")
        game.AttachAgent(&agent_1_pubeval);
    else{
        std::cerr << "ERROR: Unspecified type for agent 1!" << std::endl;
        exit(-1);
    }
    std::string agent_2_type = config.AGENT_2_TYPE();
    if(agent_2_type == "RANDOM")
        game.AttachAgent(&agent_2_random);
    else if(agent_2_type == "TERMINAL")
        game.AttachAgent(&agent_2_terminal);
    else if(agent_2_type == "WEIGHTED")
        game.AttachAgent(&agent_2_weighted);
    else if(agent_2_type == "PUBEVAL")
        game.AttachAgent(&agent_2_pubeval);
    else{
        std::cerr << "ERROR: Unspecified type for agent 2!" << std::endl;
        exit(-2);
    }
    
    // Set up bookkeeping 
    size_t num_iters = config.NUM_MATCHES();
    size_t wins_agent_1 = 0;
    size_t wins_agent_2 = 0;
    size_t starts_agent_1 = 0;
    size_t starts_agent_2 = 0;
    size_t avg_num_turns = 0;
    double avg_off_agent_1 = 0;
    double avg_off_agent_2 = 0;
    double avg_finished_agent_1 = 0;
    double avg_finished_agent_2 = 0;
    double avg_zero_percent = 0;
    double avg_last_percent = 0;
    double avg_possible_moves = 0;

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
        double num_steps = 0;
        double num_zeros = 0;
        double num_lasts = 0;
        size_t last_move_idx = 0;
        double possible_moves = 0;
        while(!game.GetState().game_over){
            if(game.GetState().cur_agent == 1){
                possible_moves += game.GetState().possible_moves.size(); 
                last_move_idx = game.GetState().possible_moves.size() - 1;
                int guess_idx = random.GetUInt(1, game.GetState().possible_moves.size()) - 1;
                game.Step();
                num_steps++;
                if(game.GetState().previous_move_idx == 0)
                    num_zeros += 1;
                if(game.GetState().previous_move_idx == last_move_idx)
                    num_lasts += 1;
            }   
            else
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
        avg_num_turns += game.GetState().turn_count;
        avg_zero_percent += (num_zeros / num_steps) / num_iters;
        avg_last_percent += (num_lasts / num_steps) / num_iters;
        avg_possible_moves += (possible_moves / num_steps) / num_iters;
    }
    std::cout << "Agent 1 starts: " << starts_agent_1 << std::endl;
    std::cout << "Agent 2 starts: " << starts_agent_2 << std::endl;
    std::cout << "Agent 1 wins: " << wins_agent_1 << std::endl;
    std::cout << "Agent 2 wins: " << wins_agent_2 << std::endl;
    std::cout << "Agent 1 average off: " << avg_off_agent_1 << std::endl;
    std::cout << "Agent 2 average off: " << avg_off_agent_2 << std::endl;
    std::cout << "Agent 1 average finished: " << avg_finished_agent_1 << std::endl;
    std::cout << "Agent 2 average finished: " << avg_finished_agent_2 << std::endl;
    std::cout << "Average turns: " << ((double)avg_num_turns) / num_iters << std::endl;
    std::cout << "Average % 0 moves: " << avg_zero_percent << std::endl;
    std::cout << "Average % last moves: " << avg_last_percent << std::endl;
    std::cout << "Average possible moves: " << avg_possible_moves << std::endl;
    return 0;
}
