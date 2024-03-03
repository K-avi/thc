#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

typedef enum err_code{
    err_ok = 0 , err_null , 
    err_alloc , err_val , 
}err_code;

extern void er_report( FILE * flux, const char * repport_msg, const char * error_msg , err_code flag);
/*
    flux -> not null 
    repport_msg -> not null 
    error_msg -> not null 
    flag -> 

    reports an error in the flux given. 

    Some wrappers are defined around this function to avoid code redundancy
*/

//some wrappers arround the er_report function ; I recommand sticking to the def_err/war_handlers 
//bc passing a code block to a macro is very much not safe lol
#define error_handler(cond, msg,flag,handler_code) if((cond)){er_report(stderr, "error", (msg), (flag)); {handler_code;} return (flag);}
/*
    the error_handler macro function checks if cond==TRUE, reports the error 
    associated with flag if it's the case and executes hanlder_code before returning the value of flag.
*/

#define warning_handler(cond,msg,flag, handler_code) if((cond)){er_report(stderr, "warning", (msg), (flag)); {handler_code;}}
/*
same as error_hanler but doesn't return anything
*/
//I really like these macros I just think they're neat the C preprocessor is crazy for this ngl 
/*
    the default macro functions are safer / easier to use. I recommand sticking to them. 
    they don't execute any code and just report the error / warning if cond is true.
*/
#define def_err_handler(cond,msg,flag) error_handler(cond,msg,flag,;)
#define def_war_handler(cond,msg,flag) warning_handler(cond,msg,flag,;)

#endif 
