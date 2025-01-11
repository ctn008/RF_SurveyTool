#include "matplotlibcpp.h" // Ensure this header is in your include path
#include <vector>
#include <cmath>

namespace plt = matplotlibcpp;

int main() {
    // Generate data for the signal
    int n = 1000; // Number of points
    std::vector<double> x(n), y(n);
    for (int i = 0; i < n; ++i) {
        x[i] = i * 0.01; // Time values
        y[i] = std::sin(x[i]) * std::exp(-0.01 * x[i]); // Damped sine wave
    }

    // Plot the signal
    plt::figure_size(800, 600); // Set figure size
    plt::plot(x, y, "r-");      // Plot with a red line
    plt::title("Damped Sine Wave");
    plt::xlabel("Time (s)");
    plt::ylabel("Amplitude");
    plt::grid(true);            // Add grid lines

    // Show the plot
    plt::show();

    return 0;
}
