#include "cache.h"
#include "hash.h"
#include "event_recorder.h"
#include "timing_event.h"
#include "zsim.h"

#include "ece_562.h"
#include <iostream>
#include <fstream>
#include <iomanip>  // For std::setw and std::setfill
#include <string>
#include <random>
#include <sstream>
#include <pin.H>

uint64_t ece562_SimpleBDICache::access(MemReq& req) {
    uint64_t respCycle = req.cycle;
    bool skipAccess = cc->startAccess(req); //may need to skip access due to races (NOTE: may change req.type!)
    if (likely(!skipAccess)) {
                // begin ECE 562 logging
                // allocating space in memory for data. the data will be the size of the line size
                DataLine data = gm_calloc<uint8_t>(zinfo->lineSize);
                // DataType type = ZSIM_FLOAT; // comment out for now; only using 64-bit ints
                PIN_SafeCopy(data, (void*)(req.lineAddr << lineBits), zinfo->lineSize);

                // make & name file, open file stream. 
                std::ofstream outputFile("cache_access_output.txt", std::ios::app);

                if (outputFile.is_open()) { // Check if the file opened successfully

                // this line is outputing the line address in hex. 0x is just so it has that before the hex number
                // std: hex is what is putting it in hex.
                    outputFile << "Accessing address: 0x" << std::hex << (req.lineAddr << lineBits) << std::endl; // Write data to the file

                // this is outputing the line size. no manipulation of it. just line size.
                    outputFile << std::hex << "line size: " << (zinfo->lineSize) << std::endl;

                    // this is making a byte data array. basically, taking the existing data
                    uint8_t* byteData = (uint8_t*)data;
                    for (size_t i = 0; i < zinfo->lineSize; ++i) {
                        // writing it out in hex.
                        outputFile << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(byteData[i]) << " ";
                    }
                    outputFile << std::endl;  // End the line after the loop

                }
                else {
                    std::cerr << "Error opening the file!" << std::endl;
                }
                // end ECE 562 logging    

        bool updateReplacement = (req.type == GETS) || (req.type == GETX);
        int32_t lineId = array->lookup(req.lineAddr, &req, updateReplacement);
        respCycle += accLat;

        if (lineId == -1 && cc->shouldAllocate(req)) {
            //Make space for new line
            Address wbLineAddr;
            lineId = array->preinsert(req.lineAddr, &req, &wbLineAddr); //find the lineId to replace
            trace(Cache, "[%s] Evicting 0x%lx", name.c_str(), wbLineAddr);

            //Evictions are not in the critical path in any sane implementation -- we do not include their delays
            //NOTE: We might be "evicting" an invalid line for all we know. Coherence controllers will know what to do
            cc->processEviction(req, wbLineAddr, lineId, respCycle); //1. if needed, send invalidates/downgrades to lower level

            array->postinsert(req.lineAddr, &req, lineId); //do the actual insertion. NOTE: Now we must split insert into a 2-phase thing because cc unlocks us.
        }
        // Enforce single-record invariant: Writeback access may have a timing
        // record. If so, read it.
        EventRecorder* evRec = zinfo->eventRecorders[req.srcId];
        TimingRecord wbAcc;
        wbAcc.clear();
        if (unlikely(evRec && evRec->hasRecord())) {
            wbAcc = evRec->popRecord();
        }

        respCycle = cc->processAccess(req, lineId, respCycle);

        // Access may have generated another timing record. If *both* access
        // and wb have records, stitch them together
        if (unlikely(wbAcc.isValid())) {
            if (!evRec->hasRecord()) {
                // Downstream should not care about endEvent for PUTs
                wbAcc.endEvent = nullptr;
                evRec->pushRecord(wbAcc);
            } else {
                // Connect both events
                TimingRecord acc = evRec->popRecord();
                assert(wbAcc.reqCycle >= req.cycle);
                assert(acc.reqCycle >= req.cycle);
                DelayEvent* startEv = new (evRec) DelayEvent(0);
                DelayEvent* dWbEv = new (evRec) DelayEvent(wbAcc.reqCycle - req.cycle);
                DelayEvent* dAccEv = new (evRec) DelayEvent(acc.reqCycle - req.cycle);
                startEv->setMinStartCycle(req.cycle);
                dWbEv->setMinStartCycle(req.cycle);
                dAccEv->setMinStartCycle(req.cycle);
                startEv->addChild(dWbEv, evRec)->addChild(wbAcc.startEvent, evRec);
                startEv->addChild(dAccEv, evRec)->addChild(acc.startEvent, evRec);

                acc.reqCycle = req.cycle;
                acc.startEvent = startEv;
                // endEvent / endCycle stay the same; wbAcc's endEvent not connected
                evRec->pushRecord(acc);
            }
        }
    }

    cc->endAccess(req);

    assert_msg(respCycle >= req.cycle, "[%s] resp < req? 0x%lx type %s childState %s, respCycle %ld reqCycle %ld",
            name.c_str(), req.lineAddr, AccessTypeName(req.type), MESIStateName(*req.state), respCycle, req.cycle);
    return respCycle;
}

void ece562_BDIDataArray::approximate(const DataLine data, DataType type) {

}