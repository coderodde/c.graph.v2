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
CND_PLATFORM=GNU-Linux
CND_DLIB_EXT=so
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/bidirectional_breadth_first_search.o \
	${OBJECTDIR}/breadth_first_search.o \
	${OBJECTDIR}/directed_graph_node.o \
	${OBJECTDIR}/directed_graph_weight_function.o \
	${OBJECTDIR}/list.o \
	${OBJECTDIR}/main.o \
	${OBJECTDIR}/my_assert.o \
	${OBJECTDIR}/queue.o \
	${OBJECTDIR}/unordered_map.o \
	${OBJECTDIR}/unordered_set.o \
	${OBJECTDIR}/utils.o


# C Compiler Flags
CFLAGS=-m32

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/c.graph.v2

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/c.graph.v2: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.c} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/c.graph.v2 ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/bidirectional_breadth_first_search.o: bidirectional_breadth_first_search.c
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -std=c89 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/bidirectional_breadth_first_search.o bidirectional_breadth_first_search.c

${OBJECTDIR}/breadth_first_search.o: breadth_first_search.c
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -std=c89 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/breadth_first_search.o breadth_first_search.c

${OBJECTDIR}/directed_graph_node.o: directed_graph_node.c
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -std=c89 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/directed_graph_node.o directed_graph_node.c

${OBJECTDIR}/directed_graph_weight_function.o: directed_graph_weight_function.c
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -std=c89 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/directed_graph_weight_function.o directed_graph_weight_function.c

${OBJECTDIR}/list.o: list.c
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -std=c89 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/list.o list.c

${OBJECTDIR}/main.o: main.c
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -std=c89 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.c

${OBJECTDIR}/my_assert.o: my_assert.c
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -std=c89 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/my_assert.o my_assert.c

${OBJECTDIR}/queue.o: queue.c
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -std=c89 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/queue.o queue.c

${OBJECTDIR}/unordered_map.o: unordered_map.c
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -std=c89 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/unordered_map.o unordered_map.c

${OBJECTDIR}/unordered_set.o: unordered_set.c
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -std=c89 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/unordered_set.o unordered_set.c

${OBJECTDIR}/utils.o: utils.c
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -std=c89 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/utils.o utils.c

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
