/*
* ECE 562 Project: Cache Compression using Base Delta Immediate algorithm
* By Luke Dagnillo, Sophia Golota, Jimmy Payan, Cecilia Quevedo 
*
* This file is an altered copy of "timing_cache.h" from zsim.
* It is being altered to support cache compression. 
*/

#ifndef ECE_562_BDI_CACHE_
#define ECE_562_BDI_CACHE_

#include "breakdown_stats.h"
#include "cache.h"
#include "timing_cache.h" // Added by JP


class HitEvent;
class MissStartEvent;
class MissResponseEvent;
class MissWritebackEvent;
class ReplAccessEvent;
class TimingEvent;

class BDI_Cache : public TimingCache {
    private:

    public:
};

#endif  // ECE_562_BDI_CACHE_
