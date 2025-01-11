#include <iostream>
#include <vector>
#include <gnuplot-iostream.h>

void plotGraph(const std::vector<double>& data) {
    // Prepare data for plotting
    std::vector<std::pair<double, double>> plotData;
    for (size_t i = 0; i < data.size(); ++i) {
        plotData.push_back({static_cast<double>(i), data[i]});
    }

    // Initialize Gnuplot
    Gnuplot gp;
    gp << "set title 'Graph of Vector Data'\n";   // Set the title of the plot
    gp << "set xlabel 'Index'\n";                  // Label for the x-axis
    gp << "set ylabel 'Value'\n";                  // Label for the y-axis

    // Plot the data as a line plot
    gp << "plot '-' with lines title 'Data'\n";
    gp.send1d(plotData);
}

int main() {
    // Example data in vector of double type
    std::vector<double> data = {1.0, 2.5, 3.7, 2.2, 5.5, 3.0, 4.8};

    // Call function to plot the data
    plotGraph(data);

    return 0;
}
