#include "ece_562.h"
#include "hash.h"
#include "repl_policies.h"

uint64_t ece562_BDICache::access(MemReq& req) {
    
}

void ece562_BDIDataArray::approximate(const DataLine data, DataType type) {

}

#if 0
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
#endif

// Returns the Index of the matching tag, or -1 if none found.
int32_t ece562_BDITagArray::lookup(Address lineAddr, const MemReq* req, bool updateReplacement){
    // find pointer to the first index of the line, assign to 'first'
    uint32_t set = hf->hash(0, lineAddr) & setMask;
    uint32_t first = set*assoc;

    // iterate through lines which may have lineAddr. if lineAddr found, return the index where it is located.
    for (uint32_t id = first; id < first + assoc; id++) {
        if (tagArray[id] ==  lineAddr) {
            if (updateReplacement) rp->update(id, req);
            return id;
        }
    }
    return -1;
}

// Returns candidate Index for insertion, wbLineAddr will point to its address for eviction.
int32_t ece562_BDITagArray::preinsert(Address lineAddr, const MemReq* req, Address* wbLineAddr) {
    // find pointer to the first index of the line, assign to 'first'
    uint32_t set = hf->hash(0, lineAddr) & setMask;
    uint32_t first = set*assoc;

    // find the eviction candidate by index
    uint32_t evictionIndex = rp->rankCands(req, SetAssocCands(first, first+assoc));
    
    // write to write-back line address, tell the cache where to preinsert
    *wbLineAddr = tagArray[evictionIndex];
    return evictionIndex;
}

// Returns candidate Index for insertion, wbLineAddr will point to its address for eviction, or -1 if none needed. size = size of data being inserted
int32_t ece562_BDITagArray::needEviction(Address lineAddr, const MemReq* req, uint16_t size, g_vector<uint32_t>& alreadyEvicted, Address* wbLineAddr) {
    // find pointer to the first index of the line, assign to 'first'
    uint32_t set = hf->hash(0, lineAddr) & setMask;
    uint32_t first = set*assoc;
    uint16_t occupiedSpace = 0;

    for (int id = first ; id < first + assoc ; id++) {
        bool found = false;

        // search for the line being evicted
        for (int i = 0 ; i < alreadyEvicted.size() ; i++) {
            if  (alreadyEvicted[i] == id) {
                found = true;
                break;
            }
        }

        // check if line has been compressed and if the line has been found yet
        if (segmentPointerArray[id] != -1 && found == false) {
            occupiedSpace += checkCompression(compressionEncodingArray[id], zinfo->lineSize);
        }
    }

    // lets the cache know no eviction required
    if (dataAssoc*zinfo->lineSize - occupiedSpace >= size)
        return -1;

    // find the cache a good candidate to evict 
    else {
        uint32_t candidate = rp->rank(req, SetAssocCands(first, first+assoc), alreadyEvicted);
        *wbLineAddr = tagArray[candidate];
        return candidate;
    }
}

// A boolean would work for current implementation, but makes additional deltas harder to implement.
uint16_t checkCompression(BDICompressionEncoding encoding, uint32_t lineSize) {
    switch(encoding) {
        case EIGHTDELTAONE:
        return 16;

        case NONE:
        default:
        return lineSize;
    }
}


