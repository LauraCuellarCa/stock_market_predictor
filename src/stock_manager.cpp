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



struct PortfolioManagerResult {
    std::vector<std::map<std::string, double>> allocations;       // Allocated funds for each stock at each hour
    std::vector<std::map<std::string, double>> portfolio_values;  // Portfolio values at each hour
};

PortfolioManagerResult portfolio_manager(
    const std::vector<std::vector<std::string>>& buying_stocks,
    const std::vector<double>& reallocation_funds,
    std::map<std::string, double>& my_portfolio,
    const std::string& strategy,
    const std::map<std::string, std::vector<double>>& stocks,
    const std::map<std::string, std::vector<double>>& ticker_to_percentage_changes) {
    
    PortfolioManagerResult result;

    if (buying_stocks.empty() || reallocation_funds.empty()) {
        return result; // Empty result if no stocks to buy or funds
    }

    size_t hours = buying_stocks.size();

    for (size_t hour = 0; hour < hours; ++hour) {
        // **Update portfolio for market changes at the start of each hour**
        for (auto& [stock, value] : my_portfolio) {
            // Check if percentage changes exist for this stock
            if (ticker_to_percentage_changes.count(stock)) {
                const auto& percentage_changes = ticker_to_percentage_changes.at(stock);

                // Check if there’s a percentage change for the current hour
                if (hour < percentage_changes.size()) {
                    double percentage_change = percentage_changes[hour];
                    value *= (1.0 + (percentage_change / 100.0)); // Apply percentage change
                }
            }
        }

        // Allocation for the current hour
        std::map<std::string, double> hour_allocation;

        // Skip this hour if no buying stocks or reallocation funds
        if (buying_stocks[hour].empty() || reallocation_funds[hour] <= 0) {
            // Store current portfolio values
            result.portfolio_values.push_back(my_portfolio);
            // Even if no allocation happened, store an empty allocation
            result.allocations.push_back(hour_allocation);
            continue;
        }

        // Determine weights for allocation based on strategy and average volatility
        std::map<std::string, double> allocation_weights;
        double total_weight = 0.0;

        for (const auto& stock : buying_stocks[hour]) {
            const auto& volatility_values = stocks.at(stock);
            double avg_volatility = 0.0;

            // Calculate average volatility for the stock
            for (double vol : volatility_values) {
                avg_volatility += vol;
            }
            avg_volatility /= volatility_values.size();

            double weight = 0.0;

            if (strategy == "optimistic") {
                weight = 1.0 / (avg_volatility + 0.001); // Inverse relation to volatility
            } else if (strategy == "neutral") {
                weight = 1.0;
            } else if (strategy == "conservative") {
                weight = 1.0 / (avg_volatility + 0.0005); // Stronger inverse relation
            }

            allocation_weights[stock] = weight;
            total_weight += weight;
        }

        // Allocate funds proportionally based on weights
        for (const auto& stock : buying_stocks[hour]) {
            double weight = allocation_weights[stock];
            double allocation = (weight / total_weight) * reallocation_funds[hour];

            // Update the portfolio with the allocated funds
            my_portfolio[stock] += allocation;

            // Store the allocation result
            hour_allocation[stock] = allocation;
        }

        // Add the allocation for this hour to the result
        result.allocations.push_back(hour_allocation);

        // Store the current state of my_portfolio
        result.portfolio_values.push_back(my_portfolio);
    }

    return result;
}




int main() {

    //THIS WILL BE THE OUTPUT FROM SEBAS' VOLATILITY CODE
    std::map<std::string, std::vector<double>> stocks = {
        {"AAPL", {0.002, 0.0025, 0.003, 0.0018}},
        {"GOOGL", {0.006, 0.0055, 0.0062, 0.006}},
        {"MSFT", {0.005, 0.0052, 0.0048, 0.005}},
        {"AMZN", {0.001, 0.0011, 0.0012, 0.0009}},
        {"TSLA", {0.007, 0.0075, 0.0068, 0.0071}}
    };

    //THIS WILL NEED BE EXTRACTED FROM THE CSV FILE, MAP OF TICKER TO PRICES OVER THE TIME PERIOD
    std::map<std::string, std::vector<double>> ticker_to_prices = {
        {"AAPL", {150.0, 155.0, 157.5, 153.0, 160.0}},
        {"GOOGL", {1000.0, 1050.0, 1025.0, 1075.0, 1100.0}},
        {"MSFT", {300.0, 310.0, 305.0, 315.0, 320.0}},
        {"AMZN", {200.0, 205.0, 210.0, 215.0, 220.0}},
        {"TSLA", {500.0, 525.0, 520.0, 530.0, 540.0}}
    };

    //THIS IS AN EXAMPLE INITIAL PORTFOLIO ALLOCATION 
    std::map<std::string, double> my_portfolio = {
        {"AAPL", 10000},
        {"GOOGL", 15000},
        {"MSFT", 20000},
        {"AMZN", 12000},
        {"TSLA", 18000}
    };

    // THIS INPUT FOR THE STRATEGY WILL BE TAKEN FROM THE USER
    std::string strategy = "optimistic";

    //Print the strategy we are using
    std::cout << "\nStrategy: " << strategy << "\n";

    // Calculate percentage changes
    std::map<std::string, std::vector<double>> ticker_to_percentage_changes =
        calculate_percentage_changes(ticker_to_prices);

    // Print the initial portfolio
    std::cout << "\nInitial Portfolio:\n";
    for (const auto& [stock, value] : my_portfolio) {
        std::cout << stock << ": $" << value << "\n";
    }
    std::cout << "--------------------------\n";

    // Call stockManager and get results
    StockManagerResult stock_result = stockManager(stocks, my_portfolio, strategy);

    // Call portfolio_manager and get results
    PortfolioManagerResult portfolio_result = portfolio_manager(
        stock_result.buying_stocks,
        stock_result.reallocation_funds,
        my_portfolio,
        strategy,
        stocks,
        ticker_to_percentage_changes
    );

    // Print combined results for each hour
    size_t hours = stock_result.buying_stocks.size();
    for (size_t hour = 0; hour < hours; ++hour) {
        std::cout << "Hour " << hour + 1 << " Results:\n";

        // Print the percentage changes for each stock
        std::cout << "  Stock Price Changes:\n";
        for (const auto& [stock, percentage_changes] : ticker_to_percentage_changes) {
            double percentage_change = 0.0;
            if (hour < percentage_changes.size()) {
                percentage_change = percentage_changes[hour];
                std::cout << "    " << stock << ": ";
                if (percentage_change >= 0) {
                    std::cout << "+";
                }
                std::cout << percentage_change << "%\n";
            } else {
                // If no data for this hour, assume no change
                std::cout << "    " << stock << ": No data\n";
            }
        }

        // Stock Manager Results
        std::cout << "  Stock Manager Decisions:\n";
        std::cout << "    Buying: ";
        for (const auto& stock : stock_result.buying_stocks[hour]) {
            std::cout << stock << " ";
        }
        std::cout << "\n";

        std::cout << "    Selling: ";
        for (const auto& stock : stock_result.selling_stocks[hour]) {
            std::cout << stock << " ";
        }
        std::cout << "\n";

        std::cout << "    Funds Available for Reallocation: $" << stock_result.reallocation_funds[hour] << "\n";

        // Portfolio Manager Results
        std::cout << "  How much we bought:\n";
        if (hour < portfolio_result.allocations.size() && !portfolio_result.allocations[hour].empty()) {
            for (const auto& [stock, allocated_funds] : portfolio_result.allocations[hour]) {
                std::cout << "    - " << stock << ": $" << allocated_funds << "\n";
            }
        } else {
            std::cout << "    No funds allocated this hour.\n";
        }

                // Print the updated portfolio at the start of the hour
        std::cout << "  Your Portfolio at the end of this hour:\n";
        // Use the stored portfolio values for this hour
        if (hour < portfolio_result.portfolio_values.size()) {
            const auto& portfolio_at_hour = portfolio_result.portfolio_values[hour];
            for (const auto& [stock, value] : portfolio_at_hour) {
                std::cout << "    " << stock << ": $" << value << "\n";
            }
        } else {
            // If for some reason we don't have portfolio values for this hour, print current my_portfolio
            for (const auto& [stock, value] : my_portfolio) {
                std::cout << "    " << stock << ": $" << value << "\n";
            }
        }
        std::cout << "--------------------------\n";

    }

    // Print final portfolio
    std::cout << "\nYour Final Portfolio:\n";
    for (const auto& [stock, value] : my_portfolio) {
        std::cout << stock << ": $" << value << "\n";
    }

    return 0;
}
