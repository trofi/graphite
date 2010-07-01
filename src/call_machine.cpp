// This call threaded interpreter implmentation for machine.h
// Author: Tim Eves

// Build either this interpreter or the direct_machine implementation.
// The call threaded interpreter is portable across compilers and 
// architectures as well as being useful to debug (you can set breakpoints on 
// opcodes) but is slower that the direct threaded interpreter by a factor of 2

#include <cassert>
#include <graphiteng/SlotHandle.h>
#include "Machine.h"
#include "Segment.h"
#include "XmlTraceLog.h"


#define registers           const byte * & dp, vm::Machine::stack_t * & sp, vm::Machine::stack_t * const sb,\
                            regbank & reg

// These are required by opcodes.h and should not be changed
#define STARTOP(name)	    bool name(registers) REGPARM(4);\
			                bool name(registers) { \
                                STARTTRACE(name,reg.is);
#define ENDOP		            ENDTRACE; \
                                return (sp - sb)/Machine::STACK_MAX==0; \
                            }

#define EXIT(status)        push(status); ENDTRACE return false

// This is required by opcode_table.h
#define do_(name)           instr(name)


using namespace vm;

struct regbank  {
    Segment       & seg;
    slotref         is, isf, isl;
    const slotref   isb;
    const instr * & ip;
    Position        endPos;
};

typedef bool        (* ip_t)(registers);

// Pull in the opcode definitions
// We pull these into a private namespace so these otherwise common names dont
// pollute the toplevel namespace.
namespace {
#define seg     reg.seg
#define is      reg.is
#define isf     reg.isf
#define isl     reg.isl
#define isb     reg.isb
#define ip      reg.ip
#define endPos  reg.endPos

#include "opcodes.h"

#undef seg
#undef is
#undef isf
#undef isl
#undef isb
#undef ip
#undef endPos
}

Machine::stack_t  Machine::run(const instr   * program,
                               const byte    * data,
                               Segment &       seg,
                               slotref &       islot_idx,
                               slotref         iStart,
                               status_t &      status)
{
    assert(program != 0);

    // Declare virtual machine registers
    const instr   * ip = program-1;
    const byte    * dp = data;
    stack_t       * sp      = _stack + Machine::STACK_GUARD,
            * const sb = sp;
    regbank         reg = {seg, islot_idx, -1, -1, iStart, ip, Position()};

    // Run the program        
    while ((reinterpret_cast<ip_t>(*++ip))(dp, sp, sb, reg)) {}
    const stack_t ret = sp == _stack+STACK_GUARD+1 ? *sp-- : 0;

    check_final_stack(sp, status);
    islot_idx = reg.is;
    return ret;
}

// Pull in the opcode table
namespace {
#include "opcode_table.h"
}

const opcode_t * Machine::getOpcodeTable() throw()
{
    return opcode_table;
}


