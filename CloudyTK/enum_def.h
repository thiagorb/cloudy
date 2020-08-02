/* 
 * File:   enum_def.h
 * Author: thiago
 *
 * Created on 7 de Novembro de 2012, 22:13
 */

#ifndef ENUM_DEF_H
#define	ENUM_DEF_H

#ifdef	__cplusplus
extern "C" {
#endif

#define ENUM_BEGIN(typ) enum typ {
#define ENUM(nam) nam
#define ENUM_END };

#ifdef	__cplusplus
}
#endif

#endif	/* ENUM_DEF_H */

