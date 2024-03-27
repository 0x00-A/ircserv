#include <iostream>
#include <ctime>

// Function to get the current hour
struct tm *getCurrentTime() 
{
    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    return timeinfo;
}

int main() 
{
    struct tm *timeinfo = getCurrentTime();
    std::cout << "Current hour: " << timeinfo->tm_min << std::endl;
    return 0;
}


