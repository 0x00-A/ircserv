#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <iostream>

using std::string;

std::string parseInfo(std::string marker, string endMarker, std::string& response)
{
        // std::string marker = "\"description\":\"";
    size_t MarkerPos = response.find(marker);
    if (MarkerPos != std::string::npos) {
        size_t start = MarkerPos + marker.size();
        size_t end = response.find(endMarker, start);
        std::string info = response.substr(start, end - start);
        // std::cout << "info: " << info << std::endl;
        return (info);
    } else {
        // std::cout << "info not found in response" << std::endl;
        return ("");
    }
}

int main() {
    int sock = 0;
    char buffer[1024] = {0};
    struct sockaddr_in serv_addr;
    // std::string serverIP = "api.openweathermap.org/data/2.5/weather?q=London,uk&APPID=851bb946327ee4bdc5230fe57cd6439f";
    std::string serverIP = "37.139.20.5";
    std::string request = 
        "GET /data/2.5/weather?q=Casablanca,MA,uk&APPID=851bb946327ee4bdc5230fe57cd6439f HTTP/1.1\r\n"
        "Host: 37.139.20.5\r\n"
        "Connection: close\r\n\r\n";

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cout << "Socket creation error" << std::endl;
        return -1;
    }

    // Convert IP address string to network byte order integer
    uint32_t ipInt = inet_addr(serverIP.c_str());
    if (ipInt == INADDR_NONE) {
        std::cerr << "Invalid IP address" << std::endl;
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(80);
    serv_addr.sin_addr.s_addr = ipInt;

    // if(inet_pton(AF_INET, serverIP.c_str(), &serv_addr.sin_addr)<=0) {
    //     std::cout << "Invalid address/ Address not supported" << std::endl;
    //     return -1;
    // }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cout << "Connection Failed" << std::endl;
        return -1;
    }

    send(sock, request.c_str(), request.size(), 0);
    ssize_t rr = read(sock, buffer, 1024);

    buffer[rr] = 0;
    std::cout << buffer << std::endl;

    send(sock, request.c_str(), request.size(), 0);
    rr = read(sock, buffer, 1024);

    buffer[rr] = 0;
    std::cout << "rec::: \n" << buffer << std::endl;

    std::string response = buffer;

    /*
    Location: London, GB
    Coordinates: Latitude 51.5085, Longitude -0.1257
    Weather: Overcast clouds
    Temperature: 282.23 Kelvin (approximately 9.08 degrees Celsius)
    Feels Like: 279 Kelvin (approximately 5.85 degrees Celsius)
    Minimum Temperature Today: 281.27 Kelvin (approximately 8.12 degrees Celsius)
    Maximum Temperature Today: 283.15 Kelvin (approximately 10 degrees Celsius)
    Pressure: 1000 hPa
    Humidity: 90%
    Visibility: 10000 meters
    Wind Speed: 6.69 m/s
    Wind Direction: 250 degrees
    Cloudiness: 100%
    */

   /*
    Longitude: "\"lon\":"
    Latitude: "\"lat\":"        // Coordinates: Latitude 51.5085, Longitude -0.1257
    Weather ID: "\"id\":" (within the "weather" array)
    Weather Main: "\"main\":\"" (within the "weather" array)
    Weather Description: "\"description\":\"" (within the "weather" array)
    Temperature: "\"temp\":" (within the "main" object)
    Feels Like Temperature: "\"feels_like\":" (within the "main" object)
    Minimum Temperature: "\"temp_min\":" (within the "main" object)
    Maximum Temperature: "\"temp_max\":" (within the "main" object)
    Pressure: "\"pressure\":" (within the "main" object)
    Humidity: "\"humidity\":" (within the "main" object)
    Visibility: "\"visibility\":"
    Wind Speed: "\"speed\":" (within the "wind" object)
    Wind Direction: "\"deg\":" (within the "wind" object)
    Cloudiness: "\"all\":" (within the "clouds" object)
    Country: "\"country\":\"" (within the "sys" object)
    Sunrise Time: "\"sunrise\":" (within the "sys" object)
    Sunset Time: "\"sunset\":" (within the "sys" object)
    City Name: "\"name\":\""
   */

    std::string res, reply="";

    std::cout << std::endl;
    
    reply += "Location: " + parseInfo("\"name\":\"", "\"", response) + ", " + parseInfo("\"country\":\"", "\"", response); 
    std::cout << "Location: Casablanca, MA" << std::endl;
    
    reply = "Coordinates: Longitude " + parseInfo("\"lon\":", "\"", response) + " Latitude " + parseInfo("\"lat\":", "}", response);
    std::cout << reply << std::endl;

    res = parseInfo("\"main\":\"", "\"", response);
    std::cout << "Weather Main: " << res << std::endl;

    res = parseInfo("\"description\":\"", "\"", response);
    std::cout << "Weather Description: " << res << std::endl;
    
    res = parseInfo("\"temp\":", ",", response);
    std::cout << "Temperature: " << res + "k" << std::endl;

    res = parseInfo("\"humidity\":", ",", response);
    std::cout << "Humidity: " << res + "%" << std::endl;

    res = parseInfo("\"speed\":", ",", response);
    std::cout << "Wind Speed: " << res + " m/s" << std::endl;

    return 0;
}
