#include "time.h"

#define DAY (24 * 60 * 60)
#define HOUR (60 * 60)
#define MIN 60

string readable_seconds_delta(time_t value)
{
    int days = value / DAY;
    int seconds_in_day = value % DAY;
    int hours = seconds_in_day / HOUR;
    int seconds_in_hour = seconds_in_day % HOUR;
    int minutes = seconds_in_hour / MIN;
    int seconds = seconds_in_hour % MIN;

    char buf[64];
    snprintf(buf, sizeof(buf), "%d days %.2d:%.2d:%.2d", days, hours, minutes, seconds);
    return string(buf);
}

#if defined(__UNITTEST__)
#include <iostream>
int main()
{
    auto result = readable_seconds_delta(DAY + HOUR + 120);
    std::cout << result << std::endl;
}
#endif
