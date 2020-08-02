#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux-x86
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/248921411/tunnel.o \
	${OBJECTDIR}/_ext/248921411/bytestream.o \
	${OBJECTDIR}/cloudy_layout.o \
	${OBJECTDIR}/_ext/248921411/cloudy_widgets.o \
	${OBJECTDIR}/_ext/248921411/socket.o \
	${OBJECTDIR}/mutex.o \
	${OBJECTDIR}/_ext/248921411/command.o \
	${OBJECTDIR}/_ext/248921411/pipe.o \
	${OBJECTDIR}/_ext/248921411/cloudy_app.o \
	${OBJECTDIR}/interface.o


# C Compiler Flags
CFLAGS=-O0

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-g

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libCloudyTK.so

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libCloudyTK.so: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.c} -shared -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libCloudyTK.so -fPIC ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/_ext/248921411/tunnel.o: ../../../Área\ de\ trabalho/Cloudy/CloudyTK/tunnel.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/248921411
	${RM} $@.d
	$(COMPILE.c) -g -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/248921411/tunnel.o ../../../Área\ de\ trabalho/Cloudy/CloudyTK/tunnel.c

${OBJECTDIR}/_ext/248921411/bytestream.o: ../../../Área\ de\ trabalho/Cloudy/CloudyTK/bytestream.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/248921411
	${RM} $@.d
	$(COMPILE.c) -g -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/248921411/bytestream.o ../../../Área\ de\ trabalho/Cloudy/CloudyTK/bytestream.c

${OBJECTDIR}/cloudy_layout.o: cloudy_layout.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/cloudy_layout.o cloudy_layout.c

${OBJECTDIR}/_ext/248921411/cloudy_widgets.o: ../../../Área\ de\ trabalho/Cloudy/CloudyTK/cloudy_widgets.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/248921411
	${RM} $@.d
	$(COMPILE.c) -g -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/248921411/cloudy_widgets.o ../../../Área\ de\ trabalho/Cloudy/CloudyTK/cloudy_widgets.c

${OBJECTDIR}/_ext/248921411/socket.o: ../../../Área\ de\ trabalho/Cloudy/CloudyTK/socket.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/248921411
	${RM} $@.d
	$(COMPILE.c) -g -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/248921411/socket.o ../../../Área\ de\ trabalho/Cloudy/CloudyTK/socket.c

${OBJECTDIR}/mutex.o: mutex.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/mutex.o mutex.c

${OBJECTDIR}/_ext/248921411/command.o: ../../../Área\ de\ trabalho/Cloudy/CloudyTK/command.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/248921411
	${RM} $@.d
	$(COMPILE.c) -g -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/248921411/command.o ../../../Área\ de\ trabalho/Cloudy/CloudyTK/command.c

${OBJECTDIR}/_ext/248921411/pipe.o: ../../../Área\ de\ trabalho/Cloudy/CloudyTK/pipe.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/248921411
	${RM} $@.d
	$(COMPILE.c) -g -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/248921411/pipe.o ../../../Área\ de\ trabalho/Cloudy/CloudyTK/pipe.c

${OBJECTDIR}/_ext/248921411/cloudy_app.o: ../../../Área\ de\ trabalho/Cloudy/CloudyTK/cloudy_app.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/248921411
	${RM} $@.d
	$(COMPILE.c) -g -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/248921411/cloudy_app.o ../../../Área\ de\ trabalho/Cloudy/CloudyTK/cloudy_app.c

${OBJECTDIR}/interface.o: interface.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/interface.o interface.c

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libCloudyTK.so

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
