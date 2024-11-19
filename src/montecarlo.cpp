#include <iostream>
#include <cmath>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <map>
#include <vector>
#include <numeric>




// Function that returns the average of a vector of doubles
// Return datatype: double 
double average(std::vector<double>& return_list) {
    double average = std::accumulate(return_list.begin(), return_list.end(), 0.0) / return_list.size();
    return average;
};

// Function that returns a vector of the difference between a vector of 
// log_return for that time period and the average return for the same time period
double iter_variance(const std::vector<double>& log_return_for_time_period, double average_return_for_time_period) {
        double variance_result = 0.0;
        int count = 0;
        for (std::vector<double>::const_iterator it = log_return_for_time_period.begin(); it != log_return_for_time_period.end(); ++it) {
            // std::cout<< *it - average_return_for_time_period<< std::endl;
            variance_result = std::pow((*it - average_return_for_time_period), 2.0) + variance_result;

            count++;
        };
        return variance_result;

};

// Function that returns the logarithmic return for a given time period
std::vector<double> logarithmic_return_function(std::vector<double>& price) {
    std::vector<double> r_t_list;
    
    // Using a regular iterator
    for (std::vector<double>::iterator it = price.begin(); it != price.end()-1; ++it) {
        double r_t = log(*(it + 1) / *it);
        // std::cout << *(it + 1) - *it << " ";
        r_t_list.push_back(r_t);
    
    }
    std::cout << std::endl;
    return r_t_list; // Assuming you want to return the average of r_t_list
};



// Function that returns the average return for a given time period
double average_return(std::vector<double> r_average_list) {
    double r_bar = average(r_average_list);
    return r_bar;
};


// Function that returns the volatility for a given time period: Hourly in this case
double volatility(std::vector<double>& log_return, double average) {
    double variance = iter_variance(log_return, average)/log_return.size();
    double volatility = std::sqrt(variance);
    return volatility;
};


//Function with the main code testing each portion of the volatility calculation
int main() {
    std::vector<double> price_list = {
    47.66699981689453, 48.11602020263672, 48.10852813720703, 48.185997009277344, 
    48.1879997253418, 47.88399887084961, 48.185001373291016, 47.49448776245117, 
    47.751502990722656, 47.773990631103516, 47.62432861328125, 47.69300079345703, 
    47.652000427246094, 47.56999969482422, 47.95499801635742, 48.305660247802734, 
    48.26900100708008, 48.279998779296875, 48.14400100708008, 48.19586944580078, 
    47.99399948120117, 48.95199966430664, 49.38750076293945, 49.04125213623047, 
    49.15599822998047, 49.209938049316406, 48.868499755859375, 49.09000015258789, 
    51.32600021362305, 51.28200149536133
    };

    // Functions
    std::vector<double> log_returns = logarithmic_return_function(price_list);

    // std::cout << "Log Returns: \n";
    // for (double value : log_returns) {
    //     std::cout << value << " \n";
    // };
    // std::cout << std::endl;
    

    std::cout << "Average Return: \n" << average_return(log_returns) << std::endl;

    std::cout << std::endl;
    

    std::cout << "Variance: \n" << iter_variance(log_returns, average_return(log_returns)) << std::endl;

    std::cout << std::endl;

    std::cout << "Volatility: \n";
    std::cout << volatility(log_returns, average(log_returns));
    
    std::cout << std::endl;
    return 0;
}