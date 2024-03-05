#include "common.h"

static inline char * str_flag(err_code flag)
{

    switch (flag) 
    {    
        case err_null: return "null pointer passed";
        case err_alloc: return "couldn't allocate memory";
        case err_val: return "couldn't reallocate memory";
        case err_thstart: return "couldn't start thread";     
        default : return "unknown error";
    }
}//ok

void er_report( FILE * flux, const char * repport_msg, const char * error_msg ,err_code flag)
{
        /*
        flux -> not null 
        repport_msg -> not null 
        error_msg -> not null 
        flag -> 
        the real repport function of the project 
        every variant of it will rely on it 
        */
        if(! flux){
                fprintf(stderr,"er_report : NULL was passed for flux\n");
            
        }else if(!repport_msg){
                fprintf(stderr,"er_report : NULL was passed for repport_msg\n");
             
        }else if(!error_msg){
                fprintf(stderr,"er_report : NULL was passed for error_msg\n");
        
        }else{
            fprintf(flux," %s : %s at %s\n",repport_msg, str_flag(flag), error_msg);
    }
}

void er_error( const char * msg , err_code flag)
{//could be a macro 
    er_report(stdout, "error", msg, flag);
}

void er_warning(const char * msg , err_code flag)
{//could be a macro
    er_report(stdout, "warning", msg, flag);
}
