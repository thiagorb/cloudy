#ifndef SYSCALL_H
#define	SYSCALL_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include "app.h"
    
enum syscall_code{
    GET_APPS_CONFIG,
    ADD_APP_STATE_CHANGE_LISTENER,
    
    LAST_SYSCALL_CODE
};

void syscall_caller(struct variant *syscall_data, struct app *app);
void syscall_initialize();

#ifdef	__cplusplus
}
#endif

#endif	/* SYSCALL_H */

