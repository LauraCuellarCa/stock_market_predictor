#include <vector>
#include <map>
#include <string>
#include <iostream>

void portfolio_manager(
    const std::vector<std::string>& buying_stocks,
    double reallocation_funds,
    std::map<std::string, double>& my_portfolio,
    const std::string& strategy,
    const std::map<std::string, double>& stocks) {
    
    if (buying_stocks.empty() || reallocation_funds <= 0) {
        std::cout << "No funds to allocate or no stocks to buy.\n";
        return;
    }

    // Determine weights for allocation based on strategy and volatility
    std::map<std::string, double> allocation_weights;
    double total_weight = 0.0;

    for (const auto& stock : buying_stocks) {
        double volatility = stocks.at(stock);
        double weight = 0.0;

        if (strategy == "optimistic") {
            // Favor low-volatility stocks more heavily
            weight = 1.0 / (volatility + 0.001); // Inverse relation to volatility
        } else if (strategy == "neutral") {
            // Balanced approach: Assign equal weight
            weight = 1.0;
        } else if (strategy == "conservative") {
            // Favor low-volatility stocks even more aggressively
            weight = 1.0 / (volatility + 0.0005); // Stronger inverse relation
        }

        allocation_weights[stock] = weight;
        total_weight += weight;
    }

    // Allocate funds proportionally based on weights
    for (const auto& stock : buying_stocks) {
        double weight = allocation_weights[stock];
        double allocation = (weight / total_weight) * reallocation_funds;

        // Update the portfolio with the allocated funds
        my_portfolio[stock] += allocation;
    }

    // Output allocation results
    std::cout << "Funds allocated to stocks:\n";
    for (const auto& stock : buying_stocks) {
        std::cout << "- " << stock << ": $" << (allocation_weights[stock] / total_weight) * reallocation_funds
                  << " (Updated total: $" << my_portfolio[stock] << ")\n";
    }
}

