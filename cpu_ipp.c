#include <stdio.h>
#include <stdlib.h>   // malloc, atoi
#include <string.h>   // memset, strdup
#include <stdint.h>   // uintXX_t
#include <stdbool.h>
#include <ctype.h>    // isalpha, isdigit
#include <getopt.h>

// Calculate the number of instructions per period (ipp) given the
// CPU speed and time. First draft. Will add other info later...
//
// TODO:
// 1. Handle normal CPU speeds, like 2.75. Currently integer only.
// 2. Add long option support, since it's in usage...
// 3. Need some more error checking. E.g., atoi results.
// 4. ...


#define EXIT_USAGE 3

// DEBUG from the compiler
#ifndef DEBUG
#define DEBUG 0
#endif

#define ONE_BILLION  1000000000
#define NANOSECONDS  ONE_BILLION
#define MICROSECONDS 1000000
#define MILLISECONDS 1000

struct options {
    uint64_t cpu_speed;
    uint32_t period;
    uint32_t multiplier;
};

void
usage()
{
    printf("cpu_ipp - Calculate number instructions per period\n\n");
    printf("\t--cpu-speed, -c\tCPU speed in GHz\n");
    printf("\t--period, -p\tTime period in [ns, us, ms]\n");
    printf("\n");
}

// From:
// https://www.facebook.com/notes/facebook-engineering/three-optimization-tips-for-c/10151361643253920
// Andrei Alexandrescu showed this in the above Facebook talk. I believe I am allowed to use it.
uint32_t digits10(uint64_t v)
{
    // Note: this function does not account for possible sign

    uint32_t result = 1;

    for (;;)
    {
        if (v < 10) return result;
        if (v < 100) return result + 1;
        if (v < 1000) return result + 2;
        if (v < 10000) return result + 3;

        // "Skip ahead by 4 orders of magnitude"
        v /= 10000U;
        result += 4;
    }
}

char *
reverse_str(char* s)
{
    char* ptr = NULL;
    char* str = malloc(BUFSIZ);

    uint32_t len = strlen(s);

    memset(str, 0, BUFSIZ);

    // Set ptr to the end of the string, since the beginning is
    // at the end. Just walk it back.
    ptr = s + (len-1);  // -1 for null terminator

    for (uint32_t i = 0; i < len; ++i)
    {
        str[i] = *ptr--;
    }

    str[len] = '\0';

    return str;
}

char*
add_comma(uint64_t num)
{
    char str[BUFSIZ];
    char* p   = str;

    uint32_t digits = digits10(num);

    memset(str, 0, BUFSIZ);

    // This essentially converts a number to a string, adding a comma every
    // third number. Except the last group, if it consts of 3 digits. Starting
    // with i=1 so i%3 works correctly.
    for (uint32_t i = 1; i <= digits; ++i)
    {
        *p++ = (num % 10) + '0';

        // (i < digits) makes sure the last group of digits does not consist
        // of a group of 3, which would result in ",144,000,000" (hypotheticaly)
        if (((i % 3) == 0) && (i < digits))
        {
            *p++ = ',';
        }

        num /= 10;
    }
    *p = '\0';

    return reverse_str(str);
}

bool
parse_period(char* period, struct options* opts)
{
    char * p = period;

    if (!isdigit(*p))
        return false;

    // Let the stdlib do the heavy lifting here... But: should I
    // just write this myself? Already being included/linked though.
    opts->period = atoi((const char*) period);

    // "while (isdigit(*p++)) ;" points one too far
    while (isdigit(*p))
        p++;
    
    if (isalpha(*p))
    {
        switch(*p)
        {
            case 'n':  // nanoseconds
                opts->multiplier = NANOSECONDS;
                break;
            case 'u':  // microseconds
                opts->multiplier = MICROSECONDS;
                break;
            case 'm':  // milliseconds
                opts->multiplier = MILLISECONDS;
                break;
            default:
                // Not defaulting as something was actually provided
                // but is invalid.
                return false;
        }
    }
    else
    {
        // We'll default in this case (no units provided)
        opts->multiplier = MILLISECONDS;
    }

    return true;
}

bool
parse_options(int argc, char ** argv, struct options* opts)
{
    int opt = 0;

    if (argc == 1)
    {
        usage();
        exit(EXIT_USAGE);
    }

    while ((opt = getopt(argc, argv, "hc:p:")) != -1)
    {
        switch(opt)
        {
            case 'h':
                usage();
                exit(EXIT_USAGE);
            case 'c':
                opts->cpu_speed = atoi(optarg);
                opts->cpu_speed *= ONE_BILLION;
                break;
            case 'p':
                if (!parse_period(optarg, opts))
                {
                    printf("Error parsing period, %s.\n", optarg);
                    printf("Valid values should match: '[0-9]+[nmu]s'\n");
                    exit(EXIT_FAILURE);
                }
                break;
        }
    }

    if (!opts->cpu_speed)
    {
        printf("Must provide CPU-speed.\n");
        usage();
        exit(EXIT_USAGE);
    }

    if (!opts->period)
    {
        printf("Defaulting to 1ms for period.\n\n");
        opts->period = 1;
        opts->multiplier = MILLISECONDS;
    }
    
    return true;
}


int main(int argc, char ** argv)
{
    struct options* opts = malloc(sizeof(struct options));
    memset(opts, 0, sizeof(struct options));

    parse_options(argc, argv, opts);

    if (DEBUG)
    {
        if (opts->cpu_speed)
            printf("CPU speed = %luGHz\n", opts->cpu_speed);

        if (opts->period)
        {
            printf("Time period = %d\n", opts->period);
            printf("Multiplier is %d\n", opts->multiplier);
        }
    }

    char* units = malloc(3);
    units =
        opts->multiplier == NANOSECONDS ? "ns" :
        opts->multiplier == MICROSECONDS ? "us" :
        "ms";
    units[2] = '\0';

    printf("A CPU running at %luGHz will perform %s instructions in %d%s:\n\n",
        opts->cpu_speed / ONE_BILLION,
        add_comma((opts->cpu_speed / opts->multiplier) * opts->period),
        opts->period,
        units);
    
    printf("Here are some other counts based on the given CPU speed:\n");
    printf("1ms = %s\n", add_comma(opts->cpu_speed / MILLISECONDS));
    printf("1us = %s\n", add_comma(opts->cpu_speed / MICROSECONDS));
    printf("1ns = %s\n", add_comma(opts->cpu_speed / NANOSECONDS));
    printf("\n");
    printf("10ms = %s\n", add_comma(((opts->cpu_speed / MILLISECONDS) * 10)));
    printf("10us = %s\n", add_comma(((opts->cpu_speed / MICROSECONDS) * 10)));
    printf("10ns = %s\n", add_comma(((opts->cpu_speed / NANOSECONDS) * 10)));


    exit(EXIT_SUCCESS);
}
