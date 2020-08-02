/* 
 * File:   command_def.h
 * Author: thiago
 *
 * Created on 7 de Novembro de 2012, 22:15
 */

#ifndef COMMAND_DEF_H
#define	COMMAND_DEF_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "enum_def.h"

ENUM_BEGIN(command_id)
    ENUM(SU_DRAW_IMAGE),
    ENUM(SU_CHANGE_COLOR),
    ENUM(SU_CHANGE_CLIP_RECT),
    ENUM(SU_DRAW_RECT),
    ENUM(SU_FILL_RECT),
    ENUM(SU_REFRESH),
    ENUM(SU_SET_DRAW_POSITION),
    ENUM(SU_DRAW_STRING),
    ENUM(CMD_INIT),
    ENUM(CMD_CLICK),
    ENUM(CMD_KEYPRESS),
    ENUM(CMD_DATA),
    ENUM(CMD_CALLBACK),
    ENUM(CMD_ACK),
    ENUM(APP_NOT_FOUND),
    ENUM(APP_START),
    ENUM(APP_MINIMIZE),
    ENUM(APP_EXEC),
    ENUM(APP_SHOW),
    ENUM(APP_END),
    ENUM(APP_INPUT_DATA),
    ENUM(APP_SYSCALL),
    ENUM(DO_LOGIN),
    ENUM(LOAD_IMAGE),
    ENUM(SET_CALLBACK),
    ENUM(APP_RESIZE),
    ENUM(APP_REPAINT),
    ENUM(LOGIN),
    ENUM(TOTAL_COMMANDS)
ENUM_END

#ifdef	__cplusplus
}
#endif

#endif	/* COMMAND_DEF_H */

