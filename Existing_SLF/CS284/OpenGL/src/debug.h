#ifndef DEBUG_H_
#define DEBUG_H_

// debugging off:
//#define DEBUGGING 0
// debugging on:
#define DEBUGGING 1

#if DEBUGGING
#define DEBUGOUT(x) (cout << x << endl)
#else
#define DEBUGOUT(x) 
#endif

#endif