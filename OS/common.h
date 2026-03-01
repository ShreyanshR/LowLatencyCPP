//
// Created by Shreyansh Rathore on 01/03/2026.
//

#ifndef LOWLEVELPROJECTS_COMMON_H
#define LOWLEVELPROJECTS_COMMON_H

#include <sys/time.h>
#include <sys/stat.h>
#include <assert.h>


double getTime() {
	struct timeval t;
	int rc = gettimeofday(&t, NULL);
	assert(rc==0);
	return (double) t.tv_sec + (double) t.tv_usec/1e6;
}

void Spin(int howLong) {
	double t = getTime();
	while ((getTime() - t) < howLong)
		;
	
}

#endif //LOWLEVELPROJECTS_COMMON_H