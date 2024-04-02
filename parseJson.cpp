#include <string>
#include <iostream>

int main() {
    std::string response = "{\"coord\":{\"lon\":-0.1257,\"lat\":51.5085},\"weather\":[{\"id\":804,\"main\":\"Clouds\",\"description\":\"overcast clouds\",\"icon\":\"04d\"}],\"base\":\"stations\",\"main\":{\"temp\":282.23,\"feels_like\":279,\"temp_min\":281.27,\"temp_max\":283.15,\"pressure\":1000,\"humidity\":90},\"visibility\":10000,\"wind\":{\"speed\":6.69,\"deg\":250},\"clouds\":{\"all\":100},\"dt\":1712043205,\"sys\":{\"type\":2,\"id\":2075535,\"country\":\"GB\",\"sunrise\":1712035959,\"sunset\":1712082897},\"timezone\":3600,\"id\":2643743,\"name\":\"London\",\"cod\":200}";

    std::string tempMarker = "\"temp\":";
    size_t tempPos = response.find(tempMarker);
    if (tempPos != std::string::npos) {
        size_t start = tempPos + tempMarker.size();
        size_t end = response.find(",", start);
        std::string tempStr = response.substr(start, end - start);
        double temp = atof(tempStr.c_str());
        std::cout << "Temperature: " << temp << "K" << std::endl;
    } else {
        std::cout << "Temperature not found in response" << std::endl;
    }

    return 0;
}
