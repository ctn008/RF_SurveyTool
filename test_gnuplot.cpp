#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <cstdint>

void plotHistogramWithGnuplot(const std::vector<uint8_t>& data) {
    // Count occurrences of each value
    std::map<uint8_t, int> frequency;
    for (uint8_t value : data) {
        frequency[value]++;
    }

    // Create a temporary data file for gnuplot
    const char* tempFileName = "histogram_data.tmp";
    std::ofstream tempFile(tempFileName);

    if (!tempFile) {
        std::cerr << "Error: Unable to create temporary data file.\n";
        return;
    }

    // Write the frequency data to the file
    for (const auto& [value, count] : frequency) {
        tempFile << static_cast<int>(value) << " " << count << "\n";
    }
    tempFile.close();

    // Open a pipe to gnuplot
    FILE* gnuplotPipe = popen("gnuplot -persistent", "w");
    if (!gnuplotPipe) {
        std::cerr << "Error: Unable to open gnuplot.\n";
        return;
    }

    // Send gnuplot commands
    fprintf(gnuplotPipe, "set title 'Histogram of Values'\n");
    fprintf(gnuplotPipe, "set xlabel 'Value (0-255)'\n");
    fprintf(gnuplotPipe, "set ylabel 'Frequency'\n");
    fprintf(gnuplotPipe, "set style fill solid\n");
    fprintf(gnuplotPipe, "plot '%s' using 1:2 with boxes title 'Frequency'\n", tempFileName);

    // Close the pipe
    pclose(gnuplotPipe);

    // Optional: Remove the temporary data file
    std::remove(tempFileName);
}

int main() {
    // Example input vector of uint8_t
    std::vector<uint8_t> data = {1, 1, 1, 0, 20, 10, 50, 20, 20, 255, 0, 0, 10, 50, 2, 55, 170, 171, 171, 173, 241, 241, 241, 241, 241, 255};

    // Call the function to plot the histogram
    plotHistogramWithGnuplot(data);

    return 0;
}
