#include <stdio.h>
#include <time.h>

int main() {
    // Получаем текущее время в формате UTC
    time_t utc_time;
    struct tm *pst_time;
    
    // Получаем текущее время
    time(&utc_time);
    
    // Преобразуем время в структуру tm в формате UTC
    pst_time = gmtime(&utc_time);
    
    // Вычитаем 8 часов для получения PST (UTC-8)
    pst_time->tm_hour -= 7;
    
    // Обрабатываем корректировку времени (если ушло в предыдущий день)
    mktime(pst_time);

    printf("Current date and time in Pacific Standard Time: %02d-%02d-%04d %02d:%02d:%02d\n",
           pst_time->tm_mday, pst_time->tm_mon + 1, pst_time->tm_year + 1900,
           pst_time->tm_hour, pst_time->tm_min, pst_time->tm_sec);

    return 0;
}

