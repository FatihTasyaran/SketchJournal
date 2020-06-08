#ifndef H_COMMON
#define H_COMMON
#include <stdint.h>


#define DEBUG
#ifdef DEBUG
#define D(x) x
#else
#define D(x)
#endif

#define TOP 100

//generater types
#define NORMAL 1
#define UNIFORM 2 
#define POISSON 3
#define EXPONENTIAL 4

//generator parameters
#define normal_dist_mean (no_unique / 2)
#define normal_dist_var 10

#define poisson_param 2

#define uniform_param_low 0
#define uniform_param_upper 1000

#define expo_param 1

//typedef unsigned stype;
typedef uint32_t stype;
typedef uint32_t dtype;

#endif
