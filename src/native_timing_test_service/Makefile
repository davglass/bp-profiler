# A minimal makefile to build the sample native service
# environment inputs:
#  BPSDK_PATH: the path to the BrowserPlus SDK

BPSDK_PATH ?= bpsdk
SRC_PATH ?= .
INCLUDE = -I${BPSDK_PATH}/include
NAME = TimingTest
SRCS = ${SRC_PATH}/service.cpp ${SRC_PATH}/bptypeutil.cpp

${NAME}/lib${NAME}.so: ${SRCS} ${SRC_PATH}/service.cpp ${NAME} ${NAME}/manifest.json
	g++ -bundle -o $@ ${INCLUDE} ${SRCS}

${NAME}/manifest.json: ${SRC_PATH}/manifest.json
	sed -e 's/@ServiceLibrary@/lib${NAME}.so/' < $< > $@

${NAME}:
	mkdir -p ${NAME}

.PHONY: clean
clean:
	rm -rf ${NAME} *~
