/** $lic$
 * Copyright (C) 2012-2015 by Massachusetts Institute of Technology
 * Copyright (C) 2010-2013 by The Board of Trustees of Stanford University
 *
 * This file is part of zsim.
 *
 * zsim is free software; you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, version 2.
 *
 * If you use this software in your research, we request that you reference
 * the zsim paper ("ZSim: Fast and Accurate Microarchitectural Simulation of
 * Thousand-Core Systems", Sanchez and Kozyrakis, ISCA-40, June 2013) as the
 * source of the simulator in any publications that use this software, and that
 * you send us a citation of your work.
 *
 * zsim is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "cache.h"
#include "hash.h"

#include "event_recorder.h"
#include "timing_event.h"
#include "zsim.h"
#include "pin.H"
#include "ece_562.h"


#include <iostream>
#include <fstream>
#include <iomanip>  // For std::setw and std::setfill
#include <string>
#include <random>
#include <sstream>


Cache::Cache(uint32_t _numLines, CC* _cc, CacheArray* _array, ReplPolicy* _rp, uint32_t _accLat, uint32_t _invLat, const g_string& _name)
    : cc(_cc), array(_array), rp(_rp), numLines(_numLines), accLat(_accLat), invLat(_invLat), name(_name) {}

const char* Cache::getName() {
    return name.c_str();
}

void Cache::setParents(uint32_t childId, const g_vector<MemObject*>& parents, Network* network) {
    cc->setParents(childId, parents, network);
}

void Cache::setChildren(const g_vector<BaseCache*>& children, Network* network) {
    cc->setChildren(children, network);
}

void Cache::initStats(AggregateStat* parentStat) {
    AggregateStat* cacheStat = new AggregateStat();
    cacheStat->init(name.c_str(), "Cache stats");
    initCacheStats(cacheStat);
    parentStat->append(cacheStat);
}

void Cache::initCacheStats(AggregateStat* cacheStat) {
    cc->initStats(cacheStat);
    array->initStats(cacheStat);
    rp->initStats(cacheStat);
}

// ideally you use this to generate a random UUI() to add to the end of the output to make unique ones everytime
// but don't want to set that in here otherwise it will create a file everytime this is called/accessed :'(

uint64_t Cache::access(MemReq& req) {

    // allocating space in memory for data. the data will be the size of the line size
    DataLine data = gm_calloc<uint8_t>(zinfo->lineSize);
    //        DataType type = ZSIM_FLOAT; // comment out for now; only using 64-bit ints
    PIN_SafeCopy(data, (void*)(req.lineAddr << lineBits), zinfo->lineSize);

    uint64_t respCycle = req.cycle;
    bool skipAccess = cc->startAccess(req); //may need to skip access due to races (NOTE: may change req.type!)

	// temp for subtraction later
	uint64_t temp = respCycle;
	std::ofstream outputFile("cache_access_output.txt", std::ios::app); // Create an output file stream
	if (likely(!skipAccess)) {
        bool updateReplacement = (req.type == GETS) || (req.type == GETX);
        int32_t lineId = array->lookup(req.lineAddr, &req, updateReplacement);
        respCycle += accLat;
	temp = respCycle;
 	// making the file. nameing it too and making it so it can keep being written to.
//    	std::ofstream outputFile("cache_access_output.txt", std::ios::app); // Create an output file stream

	if (outputFile.is_open()) { // Check if the file opened successfully
		if(name.compare("l2-0") == 0){
        // this is just outing the name. lol we can see which cache is acessing
    	// this line is outputing the line address in hex. 0x is just so it has that before the hex number
    	// std: hex is what is putting it in hex.
        outputFile << name.c_str() << " is accessing address: 0x" << std::hex << (req.lineAddr << lineBits) << std::endl; // Write data to the file

//	outputFile << "access latency is " << std::dec << accLat << " cycles." << std::endl;
   	 // this is outputing the line size. no manipulation of it. just line size.
        outputFile << "line size: " << std::dec << (zinfo->lineSize) << std::endl;

        // this is making a byte data array. basically, taking the existing data
        uint8_t* byteData = (uint8_t*)data;
        for (size_t i = 0; i < zinfo->lineSize; ++i) {
            // writing it out in hex.
            outputFile << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(byteData[i]) << " ";
        }
        outputFile << std::endl;  // End the line after the loop
}
    }
    else {
        std::cerr << "Error opening the file!" << std::endl;
    }

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

	uint64_t access_lat = respCycle - temp;
//	if (outputFile.is_open()) {
//		outputFile << "access latency is " << std::dec << access_lat << " cycles." << std::endl;
//}
//else {
//	std::cerr << "Error opening the file!" << std::endl;
//}
    assert_msg(respCycle >= req.cycle, "[%s] resp < req? 0x%lx type %s childState %s, respCycle %ld reqCycle %ld",
            name.c_str(), req.lineAddr, AccessTypeName(req.type), MESIStateName(*req.state), respCycle, req.cycle);
    return respCycle;
}

void Cache::startInvalidate() {
    cc->startInv(); //note we don't grab tcc; tcc serializes multiple up accesses, down accesses don't see it
}

uint64_t Cache::finishInvalidate(const InvReq& req) {
    int32_t lineId = array->lookup(req.lineAddr, nullptr, false);
    assert_msg(lineId != -1, "[%s] Invalidate on non-existing address 0x%lx type %s lineId %d, reqWriteback %d", name.c_str(), req.lineAddr, InvTypeName(req.type), lineId, *req.writeback);
    uint64_t respCycle = req.cycle + invLat;
    trace(Cache, "[%s] Invalidate start 0x%lx type %s lineId %d, reqWriteback %d", name.c_str(), req.lineAddr, InvTypeName(req.type), lineId, *req.writeback);
    respCycle = cc->processInv(req, lineId, respCycle); //send invalidates or downgrades to children, and adjust our own state
    trace(Cache, "[%s] Invalidate end 0x%lx type %s lineId %d, reqWriteback %d, latency %ld", name.c_str(), req.lineAddr, InvTypeName(req.type), lineId, *req.writeback, respCycle - req.cycle);

    return respCycle;
}
