/*
HOW TO COMPILE: g++ -o rtl_sdr_test rtl_sdr_test.cpp -lrtlsdr -std=c++11
*/

#include <fstream> // Required for std::ofstream
#include <rtl-sdr.h>
#include <iostream>
#include <vector>
#include <stdexcept>
#include <map>

class RtlSdr {
private:
    rtlsdr_dev_t *device;    // Device handle
    int deviceIndex;         // Device index
    bool isOpen;             // Device open flag

public:
    // Constructor: initialize with device index
    RtlSdr(int index = 0) : device(nullptr), deviceIndex(index), isOpen(false) {}

    // Open the RTL-SDR device
    void open() {
        if (rtlsdr_open(&device, deviceIndex) != 0) {
            throw std::runtime_error("Failed to open RTL-SDR device.");
        }
        isOpen = true;
    }

    // Close the RTL-SDR device
    void close() {
        if (isOpen) {
            rtlsdr_close(device);
            isOpen = false;
        }
    }

    // Set the center frequency of the device (in Hz)
    void setFrequency(uint32_t frequency) {
        if (!isOpen) {
            throw std::runtime_error("Device is not open.");
        }
        if (rtlsdr_set_center_freq(device, frequency) < 0) {
            throw std::runtime_error("Failed to set frequency.");
        }
    }

    // Set the sample rate (in Hz)
    void setSampleRate(uint32_t sampleRate) {
        if (!isOpen) {
            throw std::runtime_error("Device is not open.");
        }
        if (rtlsdr_set_sample_rate(device, sampleRate) < 0) {
            throw std::runtime_error("Failed to set sample rate.");
        }
    }

    // Set the gain mode (0 = auto gain, 1 = manual gain)
    void setGainMode(int gainMode) {
        if (!isOpen) {
            throw std::runtime_error("Device is not open.");
        }
        if (rtlsdr_set_tuner_gain_mode(device, gainMode) < 0) {
            throw std::runtime_error("Failed to set gain mode.");
        }
    }

    // Set the manual gain (in dB)
    void setGain(int gain) {
        if (!isOpen) {
            throw std::runtime_error("Device is not open.");
        }
        if (rtlsdr_set_tuner_gain(device, gain) < 0) {
            throw std::runtime_error("Failed to set gain.");
        }
    }

    int resetBuffer() {
	    return (rtlsdr_reset_buffer(device));
    }

    // Read samples from the device into a buffer (returns the number of bytes read)
    int readSamples(std::vector<uint8_t>& buffer) {
        if (!isOpen) {
            throw std::runtime_error("Device is not open.");
        }
        int bytesRead = 0;
        if (rtlsdr_read_sync(device, buffer.data(), buffer.size(), &bytesRead) != 0) {
            //throw std::runtime_error("Failed to read samples.");
        }
        return bytesRead;
    }

    // Destructor: ensures the device is closed when the object is destroyed
    ~RtlSdr() {
        close();
    }
};

/*
void saveVectorToFile(const std::vector<uint8_t>& buffer, const std::string& filename) {
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Error: Could not open file " << filename << " for writing.\n";
        return;
    }
    file.write(reinterpret_cast<const char*>(buffer.data()), buffer.size());
    if (!file) {
        std::cerr << "Error: Could not write to file " << filename << ".\n";
    }
    file.close();
}
*/

void saveVectorToFile(const std::vector<uint8_t>& buffer, const std::string& filename) {
    // Open file in binary mode with append flag
    std::ofstream file(filename, std::ios::binary | std::ios::app);
    if (!file) {
        std::cerr << "Error: Could not open file " << filename << " for writing.\n";
        return;
    }
    file.write(reinterpret_cast<const char*>(buffer.data()), buffer.size()); // Append buffer to file
    if (!file) {
        std::cerr << "Error: Could not write to file " << filename << ".\n";
    }
    file.close(); // Close the file
}


void plotWithGnuplot(const std::vector<uint8_t>& data) {
    // Create a temporary data file
    const char* tempFileName = "data.tmp";
    std::ofstream tempFile(tempFileName);

    if (!tempFile) {
        std::cerr << "Error: Unable to create temporary data file.\n";
        return;
    }

    // Write data to the temporary file
    for (size_t i = 0; i < data.size(); ++i) {
        tempFile << i << " " << static_cast<int>(data[i]) << "\n";
    }
    tempFile.close();

    // Open a pipe to gnuplot
    FILE* gnuplotPipe = popen("gnuplot -persistent", "w");
    if (!gnuplotPipe) {
        std::cerr << "Error: Unable to open gnuplot.\n";
        return;
    }

    // Send gnuplot commands
    fprintf(gnuplotPipe, "set title 'Array Data Plot (uint8_t)'\n");
    fprintf(gnuplotPipe, "set xlabel 'Index'\n");
    fprintf(gnuplotPipe, "set ylabel 'Value'\n");
    fprintf(gnuplotPipe, "set yrange [0:255]\n"); // uint8_t values are between 0 and 255
    fprintf(gnuplotPipe, "plot '%s' with linespoints title 'Data'\n", tempFileName);

    // Close the pipe
    pclose(gnuplotPipe);

    // Optional: Remove the temporary data file
    std::remove(tempFileName);
}

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
    fprintf(gnuplotPipe, "plot '%s' with linespoints title 'Frequency'\n", tempFileName);

    // Close the pipe
    pclose(gnuplotPipe);

    // Optional: Remove the temporary data file
    std::remove(tempFileName);
}

// Main function for testing the RtlSdr class
int main() {
    try {
        RtlSdr dev0(0); // Open the first RTL-SDR device
        dev0.open();

        dev0.setFrequency(98000000);  // Set frequency to 100 MHz
        dev0.setSampleRate(2048000);  // Set sample rate to 2.048 MSPS
        dev0.setGainMode(1);          // Set auto gain mode

        // Read 16k samples from the device
        std::vector<uint8_t> buffer(4096);  // 16k sample buffer
		int res = dev0.resetBuffer();

    	int count = 0;
    	const int maxIterations = 4;
        std::string filename = "samples_1_49.uint8";

        dev0.setGainMode(1);          // Set rf gain mode manual
        dev0.setGain(1);
    	while (count < maxIterations) {
        	int bytesRead = dev0.readSamples(buffer);
        	std::cout << "Iteration " << count + 1 << ": Read " << bytesRead << " bytes of data." << std::endl;
	
        	// Append iteration number to the filename
        	// std::string filename = "samples_" + std::to_string(count + 1) + ".uint8";
        	saveVectorToFile(buffer, filename);
        	std::cout << "Data saved to " << filename << "\n";
	
        	++count; // Increment the loop counter
        	if (count%2) dev0.setGain(490);
        	else dev0.setGain(1);

            plotWithGnuplot(buffer);
            plotHistogramWithGnuplot(buffer);    	}

        dev0.close();
        
    } 
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
