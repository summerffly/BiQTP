﻿/*
 * File:        Calculator.cpp
 * Author:      summer@SummerLab
 * CreateDate:  2024-03-18
 * LastEdit:    2024-03-18
 * Description: Calculator for MarketData
 */

#include <unistd.h>
#include <iostream>
#include <string>
#include <string.h>
#include <ctime>
#include <chrono>
#include <unordered_map>

#include <curl/curl.h>
#include <rapidjson/document.h>
#include <spdlog/spdlog.h>
#include <spdlog/async.h>
#include <spdlog/sinks/basic_file_sink.h>

#include "Macro.h"
#include "MDRing.h"
#include "Calculator.h"

extern std::unordered_map<std::string, int> symbol2idxUMap;
extern MDRing mdring[TOTAL_SYMBOL];
extern std::shared_ptr<spdlog::logger> sptrAsyncLogger;


//##################################################//
//   Constructor
//##################################################//
Calculator::Calculator()
{
    calTime = std::chrono::steady_clock::now();
    nowTime = std::chrono::steady_clock::now();
}

//##################################################//
//   Destructor
//##################################################//
Calculator::~Calculator()
{
    // ~
}

//##################################################//
//   线程运行实体
//##################################################//
void Calculator::Run()
{
    struct timespec time_to_sleep;
    time_to_sleep.tv_sec  = 0;
    time_to_sleep.tv_nsec = 1000*10;   // 10us

	while( true )
	{
        nowTime = std::chrono::steady_clock::now();
        std::chrono::milliseconds elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(nowTime - calTime);
        if( elapsed_time >= std::chrono::milliseconds(1) )
        {
            calTime = std::chrono::steady_clock::now();
            CalculateLastPrice();
        }

        int result = nanosleep(&time_to_sleep, NULL);
        if( result != 0 )
        {
            sptrAsyncLogger->error("Calculator::Run() nanosleep() failed !");
        }
	}
}

//##################################################//
//   ~
//##################################################//
void Calculator::CalculateLastPrice()
{
    for(const auto& symbol_iter:symbol2idxUMap)
    {
        mdring[symbol_iter.second].CalMovingAverage();
        mdring[symbol_iter.second].CalADRatio();
    }
}
