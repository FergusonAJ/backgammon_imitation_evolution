////    Specifies configuration options that can be changed at run time. 
////    Part of a term project for MSU's CSE 841 - Artificial Intelligence
////    Austin Ferguson - 2019

#ifndef BACKGAMMON_CONFIG_H
#define BACKGAMMON_CONFIG_H

// Empirical
#include "config/config.h"
EMP_BUILD_CONFIG(BackgammonTestConfig,
    // General Group
    GROUP(GENERAL, "General settings"),
    VALUE(SEED, int, 1001, "Random number seed (-1 to use current time)"),
    VALUE(NUM_MATCHES, int, 10000, "How many matches to put the agents through."),

    // Agent 1 Group
    GROUP(AGENT_1, "Agent 1 Settings"),
    VALUE(AGENT_1_TYPE, std::string, "RANDOM", "Determines which class of agent to use for agent 1"
        "  (Options: RANDOM, TERMINAL, WEIGHTED, PUBEVAL)"),
    VALUE(AGENT_1_SEED, int, -1, "Random seed to use for agent 1 (-1 for draw from emp random)"),
    VALUE(AGENT_1_WEIGHT_MOST_FORWARD, double, 0.0, "(If weighted agent) Weight for Most Forward"),
    VALUE(AGENT_1_WEIGHT_AVG_FORWARD, double, 0.0, "(If weighted agent) Weight for Average Forward"),
    VALUE(AGENT_1_WEIGHT_LEAST_FORWARD, double, 0.0, "(If weighted agent) Weight for Least Forward"),
    VALUE(AGENT_1_WEIGHT_AGGRESSIVE, double, 0.0, "(If weighted agent) Weight for Aggressiveness"),
    VALUE(AGENT_1_WEIGHT_WIDE_DEFENSE, double, 0.0, "(If weighted agent) Weight for Wide Defense"),
    VALUE(AGENT_1_WEIGHT_TALL_DEFENSE, double, 0.0, "(If weighted agent) Weight for Tall Defense"),
    
    // Agent 2 Group
    GROUP(AGENT_2, "Agent 2 Settings"),
    VALUE(AGENT_2_TYPE, std::string, "RANDOM", "Determines which class of agent to use for agent 2"
        "  (Options: RANDOM, TERMINAL, WEIGHTED, PUBEVAL)"),
    VALUE(AGENT_2_SEED, int, -1, "Random seed to use for agent 2 (-2 for draw from emp random)"),
    VALUE(AGENT_2_WEIGHT_MOST_FORWARD, double, 0.0, "(If weighted agent) Weight for Most Forward"),
    VALUE(AGENT_2_WEIGHT_AVG_FORWARD, double, 0.0, "(If weighted agent) Weight for Average Forward"),
    VALUE(AGENT_2_WEIGHT_LEAST_FORWARD, double, 0.0, "(If weighted agent) Weight for Least Forward"),
    VALUE(AGENT_2_WEIGHT_AGGRESSIVE, double, 0.0, "(If weighted agent) Weight for Aggressiveness"),
    VALUE(AGENT_2_WEIGHT_WIDE_DEFENSE, double, 0.0, "(If weighted agent) Weight for Wide Defense"),
    VALUE(AGENT_2_WEIGHT_TALL_DEFENSE, double, 0.0, "(If weighted agent) Weight for Tall Defense")
)

#endif
