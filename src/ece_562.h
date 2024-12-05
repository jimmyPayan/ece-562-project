/*
* ECE 562 Project: Cache Compression using Base Delta Immediate algorithm
* By Luke Dagnillo, Sophia Golota, Jimmy Payan, Cecilia Quevedo 
*
* This file stores declarations for the new objects needed to implement cache compression.
*
* Rather than add to many different zSim header files (like Thesaurus did), it seems like a better 
* idea to add all custom-made objects to this header and associated .cpp file
*/

#ifndef ECE_562
#define ECE_562

#include "breakdown_stats.h"
#include "cache.h"
#include "timing_cache.h"
#include "repl_policies.h"

typedef int64_t DataType;
typedef void*   DataLine;

class ece562_BDICache : public TimingCache {
    protected:
        uint32_t numTagLines;
        uint32_t numDataLines;
        //BDI_tagArray* tagArray;
        //BDI_dataArray* dataArray;

    //replacement policies
    // ReplPolicy* tagRP;
    // ReplPolicy* dataRP;
    
    public:
        // ece562_BDICache(uint32_t _numTagLines, uint32_t _numDataLines, CC* cc, ece562_BDITagArray* _tagArray, ece562_BDIDataArray* _dataArray,
        // ReplPolicy* tagRP, ReplPolicy* dataRP, uint32_t _accLat, uint32_t _invLat, uint32_t mshrs, uint32_t ways, 
        // uint32_t cands, uint32_t _domain, const g_string& _name);

        uint64_t access(MemReq& req);
};

    // public:
    //     ApproximateBDICache(uint32_t _numTagLines, uint32_t _numDataLines, CC* _cc, ApproximateBDITagArray* _tagArray, ApproximateBDIDataArray* _dataArray, ReplPolicy* tagRP, ReplPolicy* dataRP, uint32_t _accLat, uint32_t _invLat,
    //                     uint32_t mshrs, uint32_t ways, uint32_t cands, uint32_t _domain, const g_string& _name, RunningStats* _crStats, RunningStats* _evStats, RunningStats* _tutStats, RunningStats* _dutStats, Counter* _tag_hits, Counter* _tag_misses, Counter* _tag_all);

    //     uint64_t access(MemReq& req);
    //     void dumpStats();

    //     void initStats(AggregateStat* parentStat);
    //     void simulateHitWriteback(aHitWritebackEvent* ev, uint64_t cycle, HitEvent* he);

    // protected:
    //     void initCacheStats(AggregateStat* cacheStat);

class ece562_BDIDataArray {
    protected:
        // uint64_t my_llabs(int64_t x);
        // uint8_t multiBaseCompression(uint64_t* values, uint8_t size, uint8_t blimit, uint8_t bsize);
    public:
        // We can also generate bit masks here, but it will not affect the timing.
        //BDICompressionEncoding compress(const DataLine data, uint16_t* size);
        void approximate(const DataLine data, DataType type);
};

class ece562_BDITagArray {
    protected:
        bool* approximateArray;
        Address* tagArray;
        int32_t* segmentPointerArray;
        
    public:
};
// class ApproximateBDITagArray {
//     protected:
//         bool* approximateArray;
//         Address* tagArray;
//         int32_t* segmentPointerArray;    // NOTE: doesn't actually reflect segmentPointer. It's just valid or invalid.
//         BDICompressionEncoding* compressionEncodingArray;
//         ReplPolicy* rp;
//         HashFamily* hf;
//         uint32_t numLines;
//         uint32_t numSets;
//         uint32_t assoc;
//         uint32_t dataAssoc;
//         uint32_t setMask;
//         uint32_t validLines;
//         uint32_t dataValidSegments;
//     public:
//         ApproximateBDITagArray(uint32_t _numLines, uint32_t _assoc, uint32_t _dataAssoc, ReplPolicy* _rp, HashFamily* _hf);
//         ~ApproximateBDITagArray();
//         // Returns the Index of the matching tag, or -1 if none found.
//         int32_t lookup(Address lineAddr, const MemReq* req, bool updateReplacement);
//         // Returns candidate Index for insertion, wbLineAddr will point to its address for eviction.
//         int32_t preinsert(Address lineAddr, const MemReq* req, Address* wbLineAddr);
//         // Returns candidate Index for insertion, wbLineAddr will point to its address for eviction, or -1 if none needed.
//         int32_t needEviction(Address lineAddr, const MemReq* req, uint16_t size, g_vector<uint32_t>& alreadyEvicted, Address* wbLineAddr);
//         // Actually inserts
//         void postinsert(Address lineAddr, const MemReq* req, int32_t tagId, int8_t segmentId, BDICompressionEncoding compression, bool approximate, bool updateReplacement);
//         // returns compressionEncoding
//         BDICompressionEncoding readCompressionEncoding(int32_t tagId);
//         void writeCompressionEncoding(int32_t tagId, BDICompressionEncoding encoding);
//         // returns segmentPointer
//         int8_t readSegmentPointer(int32_t tagId);
//         uint32_t getValidLines();
//         uint32_t countValidLines();
//         uint32_t getDataValidSegments();
//         uint32_t countDataValidSegments();
//         void initStats(AggregateStat* parent) {}
//         void print();
// };



// Trying to explain what TagRatio does
#if 0
tagRP = new LRUReplPolicy<true>(numLines*tagRatio);
#endif
/*
* TagRP is initialized to be an LRUReplPolicy object, which accepts "numLines * TagRatio" as its argument.
* This may correspond to the max compression ratio? If that is true, since we have 8DELTA1 compression, we would have a TagRatio of 4.
* CMU paper should talk about this, I think it deals with the idea that you need additional pointers to index condensed cachelines.
* If these interpretations are correct (I think they are), then doing 8DELTA0 (cacheline has identical elements) would require TagRatio of 8.
* Even worse, 0-compression would require a TagRatio of 64, unless we handle it as an edge case.
*/

// ReplPolicy in Thesaurus is pulled from an old version of zSim which has a virtual rank function defined in LegacyReplPolicy (likely removed b/c legacy)
# if 0
uint32_t rank(const MemReq* req, SetAssocCands cands, g_vector<uint32_t>& exceptions) {panic("No"); return 0;}
#endif

/*
* TODO: Find out and explain exactly why we need DataLRUReplPolicy
* Mostly the same as LRUReplPolicy, but features an added "valid" bool pointer and proper valid assignments,
* as well as an overloaded ? "rank" function. 
*/
class ece562_DataLRUReplPolicy : public ReplPolicy {

};

#endif  // ECE_562
