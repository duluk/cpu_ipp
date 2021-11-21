#ifndef __CPU_IPP__
#define __CPU_IPP__

// Just having fun.
#define EXIT_USAGE 3

// DEBUG from the compiler
#ifndef DEBUG
#define DEBUG 0
#endif

#define ONE_BILLION  1000000000
#define NANOSECONDS  ONE_BILLION
#define MICROSECONDS 1000000
#define MILLISECONDS 1000

#define UNITS_SIZE 3

struct options {
    uint64_t cpu_speed;
    uint32_t period;
    uint32_t multiplier;
    bool     extra_info;
    bool     terse;
};

// Prototypes
char* add_comma(uint64_t num);
uint32_t digits10(uint64_t v);
bool parse_period(char* period, struct options* opts);
bool parse_options(int argc, char ** argv, struct options* opts);
char* reverse_str(char* s);
void show_extra_info(struct options* opts);
void usage(void);


#endif /* __CPU_IPP__ */
