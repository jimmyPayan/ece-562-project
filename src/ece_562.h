/*
* ECE 562 Project: Cache Compression using Base Delta Immediate algorithm
* By Luke Dagnillo, Sophia Golota, Jimmy Payan, Cecilia Quevedo 
*
* This file stores declarations for the new objects needed to implement cache compression.
*/

#ifndef ECE_562_BDI_CACHE_
#define ECE_562_BDI_CACHE_

#include "breakdown_stats.h"
#include "cache.h"
#include "timing_cache.h" // Added by JP

typedef int64_t DataType;
typedef void*   DataLine;


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
    int32_t* segmentPointerArray;// NOTE: doesn't actually reflect segmentPointer. It's just valid or invalid.
    //BDICompressionEncoding* compressionEncodingArray;
    ReplPolicy* rp;
    HashFamily* hf;
    uint32_t numLines;
    uint32_t numSets;
    uint32_t assoc;
    uint32_t dataAssoc;
    uint32_t setMask;
    uint32_t validLines;
    uint32_t dataValidSegments;

public:
    ece562_BDITagArray(uint32_t _numLines, uint32_t _assoc, uint32_t _dataAssoc, ReplPolicy* _rp, HashFamily* _hf);
    ~ece562_BDITagArray();

    // Returns the Index of the matching tag, or -1 if none found.
    int32_t lookup(Address lineAddr, const MemReq* req, bool updateReplacement);

    // Returns candidate Index for insertion, wbLineAddr will point to its address for eviction.
    int32_t preinsert(Address lineAddr, const MemReq* req, Address* wbLineAddr);

    // Returns candidate Index for insertion, wbLineAddr will point to its address for eviction, or -1 if none needed.
    int32_t needEviction(Address lineAddr, const MemReq* req, uint16_t size, g_vector<uint32_t>& alreadyEvicted, Address* wbLineAddr);

    // Actually inserts
    //void postinsert(Address lineAddr, const MemReq* req, int32_t tagId, int8_t segmentId, BDICompressionEncoding compression, bool approximate, bool updateReplacement);

    // returns compressionEncoding
    //BDICompressionEncoding readCompressionEncoding(int32_t tagId);

    //void writeCompressionEncoding(int32_t tagId, BDICompressionEncoding encoding);

    // returns segmentPointer
    //int8_t readSegmentPointer(int32_t tagId);

    uint32_t getValidLines();
    uint32_t countValidLines();
    uint32_t getDataValidSegments();
    uint32_t countDataValidSegments();
    void initStats(AggregateStat* parent) {}
    void print();
};



class ece562_BDICache : public TimingCache {
    protected:
        uint32_t numTagLines;
        uint32_t numDataLines;
        
        ece562_BDITagArray* tagArray;
        ece562_BDIDataArray* dataArray;

        //replacement policies
        ReplPolicy* tagRP;
        ReplPolicy* dataRP;
    
    public:
        // BDI_Cache(uint32_t _numTagLines, uint32_t _numDataLines, CC* cc, BDI_tagArray* _tagArray, BDI_dataArray* _dataArray,
        // ReplPolicy* tagRP, ReplPolicy* dataRP, uint32_t _accLat, uint32_t _invLat, uint32_t mshrs, uint32_t ways, 
        // uint32_t cands, uint32_t _domain, const g_string& _name);

        uint64_t access(MemReq& req);

        ece562_BDICache(uint32_t _numTagLines, uint32_t _numDataLines, CC* _cc, ece562_BDITagArray* _tagArray, ece562_BDIDataArray* _dataArray, ReplPolicy* tagRP, ReplPolicy* dataRP,
            uint32_t _accLat, uint32_t _invLat, uint32_t mshrs, uint32_t ways, uint32_t cands, uint32_t _domain, const g_string& _name);

        void dumpStats();

        //void initStats(AggregateStat* parentStat);
        //void simulateHitWriteback(aHitWritebackEvent* ev, uint64_t cycle, HitEvent* he);

protected:
    void initCacheStats(AggregateStat* cacheStat);

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
#endif  // ECE_562_BDI_CACHE_
