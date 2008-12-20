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
#include "sample.h"

static const BPCFunctionTable * g_bpCoreFunctions;

struct ServiceContext
{
    int pid;
};

static int
myAllocate(void ** instance, unsigned int, const BPElement * ctx)
{
    bp::Object * o = bp::Object::build(ctx);
    ServiceContext * myCtx = new ServiceContext;
    
    // extrace the client's process ID, available in 2.1.14 and later
    if (o->has("clientPid", BPTInteger))
    {
        myCtx->pid = (int) ((bp::Integer *) o->get("clientPid"))->value();
    }
    else
    {
        myCtx->pid = -1;
    }
    
    *instance = (void*) myCtx;
    delete o;

    return 0;
}

static void
myDestroy(void * instance)
{
    delete (ServiceContext *) instance;
}

static void
myShutdown(void)
{
}

static void
myInvoke(void * instance, const char * funcName,
          unsigned int tid, const BPElement * elem)
{
    ServiceContext * myCtx = (ServiceContext *) instance;

    if (!strcmp(funcName, "start")) {
        g_bpCoreFunctions->postError(tid, BPE_NOT_IMPLEMENTED, NULL);
    } else if (!strcmp(funcName, "stop")) {
        g_bpCoreFunctions->postError(tid, BPE_NOT_IMPLEMENTED, NULL);
    } else if (!strcmp(funcName, "sample")) {        
        if (myCtx->pid < 0) {
            g_bpCoreFunctions->postError(
                tid, "updateRequired",
                "this platform version is too old, 2.1.14 or greater required");
        } else {
            // attain a sample.
            bp::Map m;
            float cpu;
            long long mem;
            if (!get_sample(myCtx->pid, mem, cpu)) {
                g_bpCoreFunctions->postError(
                    tid, "samplingError",
                    "couldn't sample your browser process, internal error");
            } else {
                m.add("cpu", new bp::Double(cpu));
                m.add("mem", new bp::Integer(mem));
                g_bpCoreFunctions->postResults(tid, m.elemPtr());
            }
            // 'm' will recursively release all held memory.
        }
    }
}

static BPArgumentDefinition s_startArgs[] = {
    {
        "callback",
        "XXX",
        BPTCallBack,
        BP_FALSE
    }
};

static BPFunctionDefinition s_myServiceFunctions[] = {
    {
        "start",
        "start taking samples.  (XXX)",
        sizeof(s_startArgs)/sizeof(s_startArgs[0]),
        s_startArgs
    },
    {
        "stop",
        "Stop taking samples.  Passes back the array of all samples.",
        0,
        NULL
    },
    {
        "sample",
        "Returns a single sample, return value is as documented in top level "
        "service documentation",
        0,
        NULL
    }
};

static BPCoreletDefinition s_myServiceDef = {
    "BrowserProfiler",
    0, 1, 0,
    "A service that analyzes the memory and cpu usage of a web browser.  "
    "The service can take 1 sample or multiple samples at a specified interval.  "
    "When sampling at intervals, at most 1,000 samples are taken.  If you provide "
    "a callback function, your javascript will be called after every sample is taken.  "
    "If no callback is provided, all samples are stored in an array and returned after start() "
    "completes or stop() is called.\n"
    "The sample object is a map with the following keys (most values are floats):\n"
    "[sample] - the sample number (1-1,000)\n"
    "[time]   - the offset time in seconds of when the sample was taken\n"
    "[cpu]    - the percentage CPU the client browser is using\n"
    "[mem]    - the resident memory the client browser is using\n",
    sizeof(s_myServiceFunctions)/sizeof(s_myServiceFunctions[0]),
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
