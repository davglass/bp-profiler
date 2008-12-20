/**
 * OSX implementation of attaining memory and cpu usage from a running
 * process.  yes, we fork ps for now.  A problem with using mach
 * calls directly comes from the fact that you must be priviledged
 * to call task_for_pid().  ps(1) is setuid.  crazy man.
 */

#include <stdio.h>
#include <sstream>
#include <string>
bool
get_sample(int pid, long long &residentMem, float &cpuUsage)
{
    std::stringstream ss;
    ss << "ps -opcpu,rss -xwwc -p " << pid;

    FILE * f = popen(ss.str().c_str(), "r");
    if (f == NULL) return false;

    {
        char buf[1024];
        buf[0] = 0;
        (void) fgets(buf, sizeof(buf), f);
        (void) fgets(buf, sizeof(buf), f);

        // now parse
        char * p = buf;
        while (isspace(*p)) p++;
        if (*p < '0' || *p > '9') return false;
        
        cpuUsage = atof(p);
        while (isdigit(*p) || *p == '.') p++;        
        while (isspace(*p)) p++;        
        if (*p < '0' || *p > '9') return false;
        residentMem = atol(p) * 1024;
    }
    
    pclose(f);

    return true;
}
