#pragma once
#include <vector>
#include <string>
#include <map>

struct StockManagerResult {
    std::vector<std::string> buying_stocks;
    std::vector<std::string> selling_stocks;
    double reallocation_funds;
};

StockManagerResult StockManager(
    const std::map<std::string, double>& stocks, 
    std::map<std::string, double>& my_portfolio, 
    const std::string& strategy) {
    
    std::vector<std::string> buying_stocks;
    std::vector<std::string> selling_stocks;
    double reallocation_funds = 0.0;

    for (const auto& [stock, volatility] : stocks) {
        double& invested_money = my_portfolio[stock];
        double adjustment = 0.0;

        // Adjustments based on the strategy and volatility
        if (strategy == "optimistic") {
            if (volatility <= 0.003) {
                // Low volatility: Strong buy
                // Increase by 10%
                buying_stocks.push_back(stock);
            } else {
                // High volatility: Small adjustment
                // Increase by 2%
                buying_stocks.push_back(stock);
            }
        } else if (strategy == "neutral") {
            if (volatility > 0.005) {
                // High volatility: Moderate sell
                adjustment = -invested_money * 0.05; // Decrease by 5%
                reallocation_funds -= adjustment; // Add positive funds
                selling_stocks.push_back(stock);
            } else {
                // Low volatility: Slight buy
                // Increase by 3%
                buying_stocks.push_back(stock);
            }
        } else if (strategy == "conservative") {
            if (volatility > 0.005) {
                // High volatility: Strong sell
                adjustment = -invested_money * 0.1; // Decrease by 10%
                reallocation_funds -= adjustment; // Add positive funds
                selling_stocks.push_back(stock);
            } else {
                // Low volatility: Slight buy
                // Increase by 2%
                buying_stocks.push_back(stock);
            }
        }

        // Update the portfolio based on adjustment
        invested_money += adjustment;

    }


    return {buying_stocks, selling_stocks, reallocation_funds};
}
