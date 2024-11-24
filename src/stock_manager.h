#pragma once
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <utility> // For std::pair


std::map<std::string, std::vector<double>> calculate_percentage_changes(
    const std::map<std::string, std::vector<double>>& ticker_to_prices) {
    
    // Map to store percentage changes for each ticker
    std::map<std::string, std::vector<double>> ticker_to_percentage_changes;

    // Iterate through each ticker and its price vector
    for (const auto& [ticker, prices] : ticker_to_prices) {
        std::vector<double> percentage_changes;

        // Calculate percentage changes for this ticker
        for (size_t i = 1; i < prices.size(); ++i) {
            double prev_price = prices[i - 1];
            double curr_price = prices[i];

            if (prev_price != 0) { // Avoid division by zero
                double percentage_change = ((curr_price - prev_price) / prev_price) * 100.0;
                percentage_changes.push_back(percentage_change);
            } else {
                percentage_changes.push_back(0.0); // No change if previous price is zero
            }
        }

        // Add this ticker's percentage changes to the map
        ticker_to_percentage_changes[ticker] = percentage_changes;
    }

    return ticker_to_percentage_changes;
}


// Define this struct in the same file or include it via a header file.
struct StockManagerResult {
    std::vector<std::vector<std::string>> buying_stocks;   // List of stocks to buy at each hour
    std::vector<std::vector<std::string>> selling_stocks;  // List of stocks to sell at each hour
    std::vector<double> reallocation_funds;                // Funds freed up at each hour
};



StockManagerResult stockManager(
    const std::map<std::string, std::vector<double>>& stocks,
    std::map<std::string, double>& my_portfolio,
    const std::string& strategy) {
    
    StockManagerResult result;

    // Determine the maximum number of hours based on any stock's volatility vector
    size_t max_hours = 0;
    for (const auto& [stock, volatility_values] : stocks) {
        max_hours = std::max(max_hours, volatility_values.size());
    }

    // Process each hour
    for (size_t hour = 0; hour < max_hours; ++hour) {
        std::vector<std::string> buying_stocks_hour;
        std::vector<std::string> selling_stocks_hour;
        double reallocation_funds_hour = 0.0;

        for (const auto& [stock, volatility_values] : stocks) {
            double& invested_money = my_portfolio[stock];
            double adjustment = 0.0;

            // Get the volatility for the current hour, defaulting to the last value if out of bounds
            double avg_volatility = hour < volatility_values.size() ? volatility_values[hour] : volatility_values.back();

            // Adjustments based on the strategy and average volatility
            if (strategy == "optimistic") {                                    //MAYE WE WILL HAVE TO UPDATE THE PERCENTAGES BASED ON THE OUPUTES WE OBSERVE FROM THE VOLATILITY CODE
                if (avg_volatility <= 0.003) {
                    buying_stocks_hour.push_back(stock); // Strong buy
                } else {
                    buying_stocks_hour.push_back(stock); // Small buy
                }
            } else if (strategy == "neutral") {
                if (avg_volatility > 0.005) {
                    adjustment = -invested_money * 0.05; // Moderate sell
                    reallocation_funds_hour -= adjustment; // Add positive funds
                    selling_stocks_hour.push_back(stock);
                } else {
                    buying_stocks_hour.push_back(stock); // Slight buy
                }
            } else if (strategy == "conservative") {
                if (avg_volatility > 0.005) {
                    adjustment = -invested_money * 0.1; // Strong sell
                    reallocation_funds_hour -= adjustment; // Add positive funds
                    selling_stocks_hour.push_back(stock);
                } else {
                    buying_stocks_hour.push_back(stock); // Slight buy
                }
            }

            // Update the portfolio based on adjustment
            invested_money += adjustment;
        }

        // Save results for this hour
        result.buying_stocks.push_back(buying_stocks_hour);
        result.selling_stocks.push_back(selling_stocks_hour);
        result.reallocation_funds.push_back(reallocation_funds_hour);
    }

    return result;
}