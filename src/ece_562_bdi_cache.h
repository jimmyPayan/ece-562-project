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
        uint64_t lastAccCycle, lastFreeCycle;
        uint32_t numMSHRs, activeMisses;
        g_vector<TimingEvent*> pendingQueue;

        // Stats
        CycleBreakdownStat profOccHist;
        Counter profHitLat, profMissRespLat, profMissLat;

        uint32_t domain;

        // For zcache replacement simulation (pessimistic, assumes we walk the whole tree)
        uint32_t tagLat, ways, cands;

        PAD();
        lock_t topLock;
        PAD();

    public:
        TimingCache(uint32_t _numLines, CC* _cc, CacheArray* _array, ReplPolicy* _rp, uint32_t _accLat, uint32_t _invLat, uint32_t mshrs,
                uint32_t tagLat, uint32_t ways, uint32_t cands, uint32_t _domain, const g_string& _name);
        void initStats(AggregateStat* parentStat);

        uint64_t access(MemReq& req);

        void simulateHit(HitEvent* ev, uint64_t cycle);
        void simulateMissStart(MissStartEvent* ev, uint64_t cycle);
        void simulateMissResponse(MissResponseEvent* ev, uint64_t cycle, MissStartEvent* mse);
        void simulateMissWriteback(MissWritebackEvent* ev, uint64_t cycle, MissStartEvent* mse);
        void simulateReplAccess(ReplAccessEvent* ev, uint64_t cycle);

    private:
        uint64_t highPrioAccess(uint64_t cycle);
        uint64_t tryLowPrioAccess(uint64_t cycle);
};

#endif  // ECE_562_BDI_CACHE_
