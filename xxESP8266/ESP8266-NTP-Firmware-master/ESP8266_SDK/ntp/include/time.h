//Library replacement <time.h>

#ifndef TIME_H
#define TIME_H

#define TM_MAX_DATE 4102444800ul // 1 Jan 2100

typedef	unsigned int time_t;/* seconds from 1 Jan 1970 */

struct tm {
   int tm_sec;         /* seconds,  range 0 to 59          */
   int tm_min;         /* minutes, range 0 to 59           */
   int tm_hour;        /* hours, range 0 to 23             */
   int tm_mday;        /* day of the month, range 1 to 31  */
   int tm_mon;         /* month, range 0 to 11             */
   int tm_year;        /* The number of years since 1900   */
   int tm_wday;        /* day of the week, range 0 to 6    */
   int tm_yday;        /* day in the year, range 0 to 365  */
   int tm_isdst;       /* daylight saving time             */
};

//#define tm_is_leap(y) (((((y) & 3) == 0) && (((y) % 100) != 0)) || ((((y) + 1900) % 400) == 0))
#define tm_is_leap(y) (((y) & 3) == 0) //Between 2000 and 2100 all right

#define difftime2(t1, t0) ((long)((time_t)(t1)-(time_t)(t0)))

struct tm * localtime(const time_t *); /* local time */
time_t mktime(struct tm *);

#endif