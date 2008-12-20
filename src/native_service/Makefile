# A minimal makefile to build the sample native service
# environment inputs:
#  BPSDK_PATH: the path to the BrowserPlus SDK

BPSDK_PATH ?= bpsdk
SRC_PATH ?= .
INCLUDE = -I${BPSDK_PATH}/include
NAME = BrowserProfiler
SRCS = ${SRC_PATH}/service.cpp ${SRC_PATH}/bptypeutil.cpp ${SRC_PATH}/sample_Darwin.cpp ${SRC_PATH}/bpstopwatch.cpp

${NAME}/lib${NAME}.so: ${SRCS} ${NAME} ${NAME}/manifest.json
	g++ -g -Wall -bundle -o $@ ${INCLUDE} ${SRCS}

${NAME}/manifest.json: ${SRC_PATH}/manifest.json
	sed -e 's/@ServiceLibrary@/lib${NAME}.so/' < $< > $@

${NAME}:
	mkdir -p ${NAME}

.PHONY: clean
clean:
	rm -rf ${NAME} *~
