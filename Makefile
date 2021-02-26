# $Id: Makefile,v 1.19 2021-02-17 18:20:54-08 - - $

MKFILE      = Makefile
DEPFILE     = ${MKFILE}.dep
NOINCL      = ci clean spotless
NEEDINCL    = ${filter ${NOINCL}, ${MAKECMDGOALS}}
GMAKE       = ${MAKE} --no-print-directory

GPPWARN     = -Wall -Wextra -Wpedantic -Wshadow -Wold-style-cast
GPPOPTS     = ${GPPWARN} -fdiagnostics-color=never
COMPILECPP  = g++ -std=gnu++17 -g -O0 ${GPPOPTS}
MAKEDEPCPP  = g++ -std=gnu++17 -MM ${GPPOPTS}

MODULES     = logstream protocol sockets
EXECBINS    = cxi cxid
ALLMODS     = ${MODULES} ${EXECBINS}
SOURCELIST  = ${foreach MOD, ${ALLMODS}, ${MOD}.h ${MOD}.tcc ${MOD}.cpp}
CPPSOURCE   = ${wildcard ${MODULES:=.cpp} ${EXECBINS:=.cpp}}
ALLSOURCE   = ${wildcard ${SOURCELIST}} ${MKFILE}
CPPLIBS     = ${wildcard ${MODULES:=.cpp}}
OBJLIBS     = ${CPPLIBS:.cpp=.o}
CXIOBJS     = cxi.o ${OBJLIBS}
CXIDOBJS    = cxid.o ${OBJLIBS}
CLEANOBJS   = ${OBJLIBS} ${CXIOBJS} ${CXIDOBJS}

all: ${DEPFILE} ${EXECBINS}

cxi: ${CXIOBJS}
	${COMPILECPP} -o $@ ${CXIOBJS}

cxid: ${CXIDOBJS}
	${COMPILECPP} -o $@ ${CXIDOBJS}

%.o: %.cpp
	${COMPILECPP} -c $<

clean:
	- rm ${CLEANOBJS} core

spotless: clean
	- rm ${EXECBINS} ${DEPFILE}


dep: ${ALLCPPSRC}
	@ echo "# ${DEPFILE} created $(LC_TIME=C date)" >${DEPFILE}
	${MAKEDEPCPP} ${CPPSOURCE} >>${DEPFILE}

${DEPFILE}:
	@ touch ${DEPFILE}
	${GMAKE} dep

again: ${ALLSOURCE}
	${GMAKE} spotless dep ci all lis

ifeq (${NEEDINCL}, )
include ${DEPFILE}
endif

