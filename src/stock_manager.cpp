#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <utility> // For std::pair

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


int main() {
    std::map<std::string, double> stocks = {
        {"AAPL", 0.002},
        {"GOOGL", 0.006},
        {"MSFT", 0.005},
        {"AMZN", 0.001},
        {"TSLA", 0.007},
        {"META", 0.008},
        {"NFLX", 0.003},
        {"NVDA", 0.002},
        {"ADBE", 0.004},
        {"INTC", 0.001}
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

    std::string strategy = "conservative"; // Change to "optimistic", "neutral", or "conservative"

    // Call the function
    StockManagerResult result = StockManager(stocks, my_portfolio, strategy);

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
    portfolio_manager(result.buying_stocks, result.reallocation_funds, my_portfolio, strategy, stocks);

    // Output the final portfolio after allocation
    std::cout << "\nFinal portfolio:\n";
    for (const auto& [stock, value] : my_portfolio) {
        std::cout << stock << ": $" << value << "\n";
    }

    return 0;
}
