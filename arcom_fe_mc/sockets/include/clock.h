// clock.h
// Get relative BIOS clock time from 0x40:0x6c

unsigned long Clock(void);

// The clock units are 2^16 (65536) per hour
// For a close approximation using integer arithmetic,
// multiply the clock value by 5 and divide by 91 to get the time in seconds
// 65536/3600 = 18.20444, 91/5 = 18.2, 0.024% error

#if !defined(_Windows) && !defined(_OSE)
#define CLOCK_MULTIPLIER   5
#define CLOCK_DIVISOR      91
#define CLOCK_DIVISOR1     91
#define CLOCK_DIVISOR2     1
#else
#define CLOCK_MULTIPLIER   1
#define CLOCK_DIVISOR      1000
#define CLOCK_DIVISOR1     100
#define CLOCK_DIVISOR2     10
#endif
