/*BEGIN_LEGAL 
Intel Open Source License 

Copyright (c) 2002-2012 Intel Corporation. All rights reserved.
 
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.  Redistributions
in binary form must reproduce the above copyright notice, this list of
conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.  Neither the name of
the Intel Corporation nor the names of its contributors may be used to
endorse or promote products derived from this software without
specific prior written permission.
 
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE INTEL OR
ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
END_LEGAL */
#include <iostream>
#include <fstream>
#include "pin.H"

ofstream OutFile;

// Counters
static UINT64 icountMemRead = 0;
static UINT64 icountMemRead2 = 0;
static UINT64 icountMemWrite = 0;
static UINT64 icountMemOp = 0;
static UINT64 icountMemCall = 0;
static ADDRINT lastIp = 0;
static ADDRINT lastReadAddr = 0;
static ADDRINT lastWriteAddr = 0;
static const ADDRINT mask(~(16 - 1));
static UINT64 errors = 0;

VOID PIN_FAST_ANALYSIS_CALL readMemoryFunc(
    ADDRINT memea_orig,ADDRINT memea_callback,THREADID threadIndex, string *dis,CONTEXT *ctxt, ADDRINT ip)
{

    if (ip != lastIp)
    {
        OutFile << "read analysis missing: "  << hex << ip << " " << *dis << endl;
        errors++;
    }

    if (memea_orig != lastReadAddr)
    {
       OutFile << "read orig address incorrect: " << hex << memea_orig << " " << *dis << endl;
        errors++;
    }

    if ((memea_callback & 0xf) != 0)
    {
        OutFile << "read address not 16 aligned: " << hex << memea_callback << " " << *dis << endl;
        errors++;
    }

    if ((memea_orig & mask) != memea_callback)
    {
        OutFile << "read addresses incorrect: " << hex << memea_orig << " " << memea_callback << " " << *dis << endl;
        errors++;
    }

    icountMemRead++;
}

VOID PIN_FAST_ANALYSIS_CALL read2MemoryFunc(ADDRINT memea_orig,ADDRINT memea_callback,
                                            ADDRINT memea2_orig,ADDRINT memea2_callback,
                                            THREADID threadIndex, string *dis,
                                            CONTEXT *ctxt, ADDRINT ip)
{
    if (ip != lastIp)
    {
        OutFile << "read2 analysis missing: "  << " " << *dis << endl;
        errors++;
    }

    if ((memea_callback & 0xf) != 0)
    {
        OutFile << "read2 first address not 16 aligned: " << hex << memea_callback << " " << *dis << endl;
        errors++;
    }

    if ((memea_orig & mask) != memea_callback)
    {
        OutFile << "read2 first addresses incorrect: " << hex << memea_orig << " " << memea_callback << " " << *dis << endl;
        errors++;
    }

    if ((memea2_callback & 0xf) != 0)
    {
        OutFile << "read2 second address not 16 aligned: " << hex << memea2_callback << " " << *dis << endl;
        errors++;
    }

    if ((memea_orig & mask) != memea_callback)
    {
        OutFile << "read2 second addresses incorrect: " << hex << memea2_orig << " " << memea2_callback << " " << *dis << endl;
        errors++;
    }

    icountMemRead2++;
}

VOID PIN_FAST_ANALYSIS_CALL writeMemoryFunc(
    THREADID threadIndex,ADDRINT memea_orig,ADDRINT memea_callback,ADDRINT ip,CONTEXT *ctxt,string *dis)
{

    if (ip != lastIp)
    {
        OutFile << "write analysis missing: " << " " << *dis << endl;
        errors++;
    }

    if (memea_orig != lastWriteAddr)
    {
        OutFile << "write orig address incorrect: " << hex << memea_orig << " " << ip << " " << *dis << endl;
        errors++;
    }

    if ((memea_callback & 0xf) != 0)
    {
        OutFile << "write address not 16 aligned: " << hex << memea_callback << " " << *dis << endl;
        errors++;
    }

    if ((memea_orig & mask) != memea_callback)
    {
        OutFile << "write addresses incorrect: " << hex << memea_orig << " " << memea_callback << " " << *dis << endl;
        errors++;
    }

    icountMemWrite++;
}

VOID PIN_FAST_ANALYSIS_CALL opMemoryFunc(
    ADDRINT memea_orig,ADDRINT memea_callback,ADDRINT ip, CONTEXT *ctxt,
    string *dis)
{

    if (ip != lastIp)
    {
        OutFile << "op analysis missing: " << " " << *dis << endl;
        errors++;
    }

    if ((memea_callback & 0xf) != 0)
    {
        OutFile << "op address not 16 aligned: " << hex << memea_callback << " " << *dis << endl;
        errors++;
    }

    if ((memea_orig & mask) != memea_callback)
    {
        OutFile << "op addresses incorrect: " << hex << memea_orig << " " << memea_callback << " "  << ip << " " << *dis << endl;
        errors++;
    }

    icountMemOp++;
}

ADDRINT PIN_FAST_ANALYSIS_CALL memoryCallback(PIN_MEM_TRANS_INFO* memTransInfo) 
{
    icountMemCall++;

    lastIp = memTransInfo->ip;

    if (memTransInfo->memOpType == PIN_MEMOP_STORE )
        lastWriteAddr = memTransInfo->addr;
    else
        lastReadAddr = memTransInfo->addr;

    //OutFile << "callback addresses: " << hex << (memTransInfo->addr)  << " " << (memTransInfo->addr & mask)   << endl;

    return (memTransInfo->addr & mask);
}
    
// Pin calls this function every time a new instruction is encountered
VOID Instruction(INS ins, VOID *v)
{
    string *disptr = new string(INS_Disassemble(ins));

    // reads
    if (INS_IsMemoryRead(ins)) {
        INS_InsertCall(ins, 
            IPOINT_BEFORE, 
            (AFUNPTR)readMemoryFunc, 
            IARG_FAST_ANALYSIS_CALL,
            IARG_MEMORYREAD_EA,
            IARG_MEMORYREAD_PTR,
            IARG_THREAD_ID,
            IARG_PTR, disptr,
            IARG_CONTEXT,
            IARG_INST_PTR,
            IARG_END);
    }

    // writes
    if (INS_IsMemoryWrite(ins)) {
        INS_InsertCall(ins, 
            IPOINT_BEFORE, 
            (AFUNPTR)writeMemoryFunc, 
            IARG_FAST_ANALYSIS_CALL,
            IARG_THREAD_ID,
            IARG_MEMORYWRITE_EA,
            IARG_MEMORYWRITE_PTR ,
            IARG_INST_PTR,
            IARG_CONTEXT,
            IARG_PTR, disptr, 
            IARG_END);
    }

    UINT32 memOperands = INS_MemoryOperandCount(ins);
    if (!INS_IsVgather(ins) && memOperands)
    {
        // OPs
        for (UINT32 memOp = 0; memOp < memOperands; memOp++)
        {
            if (INS_MemoryOperandIsRead(ins, memOp) ||  
                INS_MemoryOperandIsWritten(ins, memOp)) {
                INS_InsertCall(ins, 
                    IPOINT_BEFORE, 
                    (AFUNPTR)opMemoryFunc, 
                    IARG_FAST_ANALYSIS_CALL,
                    IARG_MEMORYOP_EA, memOp,
                    IARG_MEMORYOP_PTR, memOp,
                    IARG_INST_PTR ,
                    IARG_CONTEXT,
                    IARG_PTR, disptr, 
                    IARG_END);
            }
        }
    }

    // READ2
    if (INS_HasMemoryRead2(ins)) {
        INS_InsertCall(ins, 
            IPOINT_BEFORE, 
            (AFUNPTR)read2MemoryFunc, 
            IARG_FAST_ANALYSIS_CALL,
            IARG_MEMORYREAD_EA,
            IARG_MEMORYREAD_PTR,
            IARG_MEMORYREAD2_EA,
            IARG_MEMORYREAD2_PTR,
            IARG_THREAD_ID,
            IARG_PTR, disptr, 
            IARG_CONTEXT,
            IARG_INST_PTR,
            IARG_END);
    }

}


KNOB<string> KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool",
    "o", "memaddrcall.out", "specify output file name");

// This function is called when the application exits
VOID Fini(INT32 code, VOID *v)
{
    // Write to a file since cout and cerr maybe closed by the application
    OutFile.setf(ios::showbase);
    OutFile << "Count Mem Reads " << icountMemRead << endl;
    OutFile << "Count Mem Read2s " << icountMemRead2 << endl;
    OutFile << "Count Mem Writes " << icountMemWrite << endl;
    OutFile << "Count Mem Ops "    << icountMemOp << endl;
    OutFile << "Count Mem callbacks " << icountMemCall << endl;
    OutFile << "Errors " << errors << endl;
    OutFile.close();

    // If we have errors then terminate abnormally 
    if (errors)
    {
        cout << "Test memory_addr_callback is terminated cause found " << errors << " errors " << endl;
        PIN_ExitProcess(errors);
    }
}

/* ===================================================================== */
/* Print Help Message                                                    */
/* ===================================================================== */

INT32 Usage()
{
    cerr << "This tool tests memory address translation callback" << endl;
    cerr << endl << KNOB_BASE::StringKnobSummary() << endl;
    return -1;
}

/* ===================================================================== */
/* Main                                                                  */
/* ===================================================================== */

int main(int argc, char * argv[])
{
    // Initialize pin
    if (PIN_Init(argc, argv)) return Usage();

    OutFile.open(KnobOutputFile.Value().c_str());

    // Register Instruction to be called to instrument instructions
    INS_AddInstrumentFunction(Instruction, 0);

    // Register memory callback
    PIN_AddMemoryAddressTransFunction(memoryCallback);

    // Register Fini to be called when the application exits
    PIN_AddFiniFunction(Fini, 0);

    // Start the program, never returns
    PIN_StartProgram();
    
    return errors;
}
