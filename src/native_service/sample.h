/*
 *  sample the cpu usage and resident memory of a running process 
 *
 */
#ifndef __SAMPLE_H__

bool get_sample(int pid, int &residentMem, int &cpuUsage);

#endif
