#include <iostream>
#include <ctime>

#include <ctime>

long getTimeInMinutes() {
    // Get current time
    std::time_t currentTime;
    std::time(&currentTime);

    // Convert to minutes
    return currentTime / 60;
}


#include <iostream>

int main() {
    long currentTimeInMinutes = getTimeInMinutes();
    std::cout << "Current time in minutes since epoch: " << currentTimeInMinutes << std::endl;
    return 0;
}


