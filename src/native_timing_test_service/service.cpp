/**
 * Copyright 2006-2008, Yahoo!
 *  
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are
 *  met:
 *
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in
 *     the documentation and/or other materials provided with the
 *     distribution.
 *  3. Neither the name of Yahoo! nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 * 
 *  THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 *  IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *  DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 *   SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 *  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 *  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 *  IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

#include <ServiceAPI/bperror.h>
#include <ServiceAPI/bptypes.h>
#include <ServiceAPI/bpdefinition.h>
#include <ServiceAPI/bpcfunctions.h>
#include <ServiceAPI/bppfunctions.h>

#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "bptypeutil.h"

static const BPCFunctionTable * g_bpCoreFunctions;

static int
myAllocate(void ** instance, unsigned int, const BPElement * ctx)
{
    *instance = NULL;
    return 0;
}

static void
myDestroy(void * instance)
{
}

static void
myShutdown(void)
{
}

static void
myInvoke(void * instance, const char * funcName,
          unsigned int tid, const BPElement * elem)
{
    bp::Object * args = bp::Object::build(elem);
    g_bpCoreFunctions->invoke(
        tid,
        (long long) *(args->get("startedCallback")),
        bp::Null().elemPtr());
    
    g_bpCoreFunctions->postResults(
        tid, 
        bp::Null().elemPtr());

    delete args;
}

static BPArgumentDefinition s_myServiceArguments[] = {
    {
        "startedCallback",
        "a callback that will be hit immediately to allow for calibration to get a valid shared \"zero point\" between processes",
        BPTCallBack,
        BP_FALSE
    }
};

static BPFunctionDefinition s_myServiceFunctions[] = {
    {
        "youGotTime",
        "Immediately calls a callback.  Then samples for 10 seconds attaining both datestamps and relative ms granularity timing.  With some magic in JS we can compare the relative accuracy of the two methods.",
        1,
        s_myServiceArguments
    }
};

static BPCoreletDefinition s_myServiceDef = {
    "TimingTest",
    0, 0, 2,
    "A service that returns some timing samples to help us discover "
    "how to better correlate time between the browser and an external "
    "browserplus service.",
    1,
    s_myServiceFunctions
};

static const BPCoreletDefinition *
myInitialize(const BPCFunctionTable * bpCoreFunctions,
              const BPElement * parameterMap)
{
    g_bpCoreFunctions = bpCoreFunctions;
    return &s_myServiceDef;
}

static BPPFunctionTable funcTable = {
    BPP_CORELET_API_VERSION,
    myInitialize,
    myShutdown,
    myAllocate,
    myDestroy,
    myInvoke,
    NULL,
    NULL
};

const BPPFunctionTable *
BPPGetEntryPoints(void)
{
    return &funcTable;
}
