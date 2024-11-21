#include "volatility_formula.h"
#include "portfolio_manager.h"
#include "stock_manager.h"
#include <iostream>
#include <cmath>
#include <map>
#include <vector>
#include <numeric>
#include <string>
#include <utility> // For std::pair
#include <tuple>
#include <limits>
#include <algorithm>
#include <cctype>


/* FUNCTION TO INITIALISE GAME AND VARIABLES initial_investment, strategy and months*/
std::tuple<float, int, std::string> startgame() {
    std::cout << "Welcome to Stock Shock. Today is 1st of January of 2023. Let's test your investment skills.\n";
    std::cout << "You will have a series of decisions to make which will affect how your money behaves, so choose wisely!\n";

    // initial investment
    float initial_investment = 20000;
    std::cout << "\nFirst off, how much money would you like to invest? (Enter a positive number or type 'you choose'): ";
    std::string input;
    getline(std::cin, input);
    if (input != "you choose") {
        try {
            initial_investment = std::stof(input);
            if (initial_investment <= 0) throw std::invalid_argument("Must be positive");
        } catch (std::exception&) {
            std::cout << "Invalid input. Using default of 20,000 euros." << std::endl;
            initial_investment = 20000;
        }
    }

    // number of months
    int months = 12;
    std::cout << "\nHow many months would you like to test? (1-12 or type 'you choose'): ";
    getline(std::cin, input);
    if (input != "you choose") {
        try {
            months = std::stoi(input);
            if (months < 1 || months > 12) throw std::out_of_range("Must be between 1 and 12");
        } catch (std::exception&) {
            std::cout << "Invalid input. Using default of 12 months." << std::endl;
            months = 12;
        }
    }

    // investment strategy
    std::string strategy = "Neutral";
    std::cout << "\nWhat investment strategy would you like to use? (Optimistic, Neutral, Conservative, or type 'you choose'):\n";
    std::cout << "Optimistic: Go big or go home! High-stakes investing with volatile stocks and emerging markets.\n";
    std::cout << "Neutral: A balanced strategy with growth stocks and blue-chip names for steady growth.\n";
    std::cout << "Conservative: Low-volatility stocks and steady dividends, focusing on stable companies.\n";
    getline(std::cin, input);
    std::transform(input.begin(), input.end(), input.begin(),
                   [](unsigned char c){ return std::tolower(c); });
    if (input == "you choose" || (input != "optimistic" && input != "conservative")) {
        strategy = "neutral";
    } else if (input == "optimistic") {
        strategy = "optimistic";
    } else if (input == "conservative") {
        strategy = "conservative";
    }

    return std::make_tuple(initial_investment, months, strategy);
}




/*
Function that returns a map with they key as the ticker(string) and the value as the 
volatility(double) for the given time period: Hourly in this case
*/
std::map<std::string, double> ticker_to_vol_hourly(std::map<std::string, std::vector<double>> input_map) {
    
    std::map<std::string, double> ticker_vol_map;

    for (const auto& pair : input_map) {
        const std::string& ticker = pair.first;
        const std::vector<double>& prices = pair.second;

        if (prices.size() < 6) {
            std::cout << " Not enough data for " << ticker << std::endl;
            continue;
        }

        // Use only the first 6 values
        std::vector<double> first_six_prices(prices.begin(), prices.begin() + 6);

        // Calculation of Volatility
        std::vector<double> log_returns = logarithmic_return_function(first_six_prices);
        double avg_return = average_return(log_returns);
        double vol = volatility(log_returns, avg_return);

        // Adding the value to the initial map for the past volatility calculations
        ticker_vol_map[ticker] = vol;


        // Print Statements incase you want to checkout the values for yourself
        

        std::cout << "\n Ticker: " << ticker << std::endl;
        // std::cout << " Average return: " << avg_return << std::endl;

        // std::cout << "\n Log Returns: " << std::endl;
        // std::cout << " Size: " << log_returns.size() << std::endl;

        // // Prints the log returns
        // for (auto& i : log_returns) {
        //     std::cout << " " << (std::pow(i  - avg_return, 2.0)) << "\n";

        // }

        // std::cout << std::endl;
        // std::cout << " Variances: " << iter_variance(log_returns, avg_return)  << std::endl;
        std::cout << " Volatility: " << vol << std::endl;
        
    
    }

    return ticker_vol_map;
}


std::map<std::string, double> true_volatility(std::map<std::string, std::vector<double>> input_map, std::map<std::string, double> standard_ticker_vol_map){
    
    std::cout << "\n-----------------------------------\n";

    std::map<std::string, double> true_volatility_output;
            
    for (const auto& pair : standard_ticker_vol_map) {
        const std::string& ticker = pair.first;
        const std::vector<double>& prices = input_map[ticker];

        if (prices.size() > 6) {
            double current_volatility = pair.second;
            double lambda = 0.94;

            for (size_t i = 5; i < prices.size()-1; ++i) {
                double old_price = prices[i];
                double new_price = prices[i + 1];
                current_volatility = update_volatility(current_volatility, new_price, old_price, lambda);
            //     std::cout << "\nTicker: " << ticker << std::endl;
            // std::cout << "Current Volatility (Updated): " << current_volatility << std::endl;
            }

            std::cout << "\nTicker: " << ticker << std::endl;
            std::cout << "Current Volatility (Updated): " << current_volatility << std::endl;
            true_volatility_output[ticker] = current_volatility;

        } else {
            std::cout << ticker << ": Not enough data" << std::endl;
        }
    }

    return true_volatility_output;
};



// Function to print a std::map
void printMap(const std::map<std::string, double>& myMap, const std::string& title) {
    std::cout << title << std::endl;
    for (const auto& [key, value] : myMap) {
        std::cout << key << ": " << value << std::endl;
    }
    std::cout << std::endl;
}


//Function with the main code testing each portion of the volatility calculation
int main() {

    std::map<std::string, std::vector<double>> standard_volatility = {
        {"NVDA", {100, 200, 300, 400, 500, 600, 220, 500, 200, 400, 700, 100, 900}},
        {"MSFT", {100, 200, 300, 400, 500, 600, 220, 500, 200, 400, 700, 100, 900}},
        {"TSLA", {100, 200, 300, 400, 500, 600, 220, 500, 200, 400, 700, 100, 900}},
        {"AAPL", {}},
        {"GOOG", {}},
        {"META", {}},
        {"AVGO", {}},
    };
        std::map<std::string, double> my_portfolio = {
        {"AAPL", 10000},
        {"GOOGL", 15000},
        {"MSFT", 20000},
        {"AMZN", 12000},
        {"TSLA", 18000},
        {"META", 14000},
        {"NFLX", 16000},
        {"NVDA", 11000},
        {"ADBE", 13000},
        {"INTC", 12500}
    };

    //INIT GAME
    float initial_investment;
    int months;
    std::string strategy;
    std::tie(initial_investment, months, strategy) = startgame();
    std::cout << "Investment: " << initial_investment << " Euros, Duration: " << months << " months, Strategy: " << strategy << std::endl;

    // GET VOLATILITY MAP
    std::map<std::string, double> output = ticker_to_vol_hourly(standard_volatility);
    std::map<std::string, double> true_vol = true_volatility(standard_volatility, output);

    // Print the maps
    printMap(output, "Hourly Volatility:");
    printMap(true_vol, "True Volatility:");

    // Call the function
    StockManagerResult result = StockManager(true_vol, my_portfolio, strategy);

    // Output results from StockManager
    std::cout << "Buying stocks:\n";
    if (result.buying_stocks.empty()) {
        std::cout << "No stocks to buy.\n";
    } else {
        for (const auto& stock : result.buying_stocks) {
            std::cout << "- " << stock << "\n";
        }
    }

    std::cout << "\nSelling stocks:\n";
    if (result.selling_stocks.empty()) {
        std::cout << "No stocks to sell.\n";
    } else {
        for (const auto& stock : result.selling_stocks) {
            std::cout << "- " << stock << "\n";
        }
    }

    std::cout << "\nReallocation funds: $" << result.reallocation_funds << "\n";

    // Allocate reallocation funds using portfolio_manager
    portfolio_manager(result.buying_stocks, result.reallocation_funds, my_portfolio, strategy, true_vol);

    // Output the final portfolio after allocation
    std::cout << "\nFinal portfolio:\n";
    for (const auto& [stock, value] : my_portfolio) {
        std::cout << stock << ": $" << value << "\n";
    }

    return 0;
}




