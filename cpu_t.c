#include "cpu_t.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>

/// Debug macro
#ifdef NDEBUG
#define __DEBUG_EXEC(code) ;
#else
#define __DEBUG_EXEC(code) code
#endif

/// Definitions of cpu-cmds
#define DEF_CMD(name)	\
static inline void cpu_cmd_##name(struct cpu_t*);
#include "cmd_list.h"
#undef DEF_CMD


/// Set instr pointer
int cpu_set_rip(struct cpu_t *cpu, void *code)
{
	cpu->rip = code;
	return 0;
}

/// Set stack pointer
int cpu_set_rsp(struct cpu_t *cpu, void *rsp)
{
	cpu->reg[REG_rsp] = (uint64_t) rsp;
	return 0;
}

/// Set memory bounds
int cpu_set_mem(struct cpu_t *cpu, void *mem_min, void *mem_max)
{
	cpu->mem_min = mem_min;
	cpu->mem_max = mem_max;
	return 0;
}

/// Get byte from rip
static inline uint8_t cpu_rip_byte(struct cpu_t *cpu)
{
	__DEBUG_EXEC(
	if (cpu->rip > cpu->mem_max || cpu->rip < cpu->mem_min) {
		cpu->trap = TRAP_ERROR_MEMORY;
		assert(0);
		return 0;
	})
	register uint8_t cmd = *(uint8_t*) cpu->rip;
	cpu->rip = (uint8_t*) cpu->rip + 1;
	return cmd;
}

/// Get qword from rip
static inline uint64_t cpu_rip_qword(struct cpu_t *cpu)
{
	__DEBUG_EXEC(
	if (cpu->rip > cpu->mem_max || cpu->rip < cpu->mem_min) {
		cpu->trap = TRAP_ERROR_MEMORY;
		assert(0);
		return 0;
	})
	register uint64_t cmd = *(uint64_t*) cpu->rip;
	cpu->rip = (uint64_t*) cpu->rip + 1;
	return cmd;
}

/// Protected write for cpu
static inline void cpu_write(struct cpu_t *cpu, void *ptr, uint64_t val)
{
	__DEBUG_EXEC(
	if (ptr > cpu->mem_max || ptr < cpu->mem_min) {
		cpu->trap = TRAP_ERROR_MEMORY;
		assert(0);
	})
	*(uint64_t*) ptr = val;
}

/// Protected read for cpu
static inline uint64_t cpu_read(struct cpu_t *cpu, void *ptr)
{
	__DEBUG_EXEC(
	if (ptr > cpu->mem_max || ptr < cpu->mem_min) {
		cpu->trap = TRAP_ERROR_MEMORY;
		assert(0);
		return 0;
	})
	return *(uint64_t*) ptr;
}

/// Shell for cpu-push (not a cmd)
static inline void cpu_push(struct cpu_t *cpu, uint64_t val)
{
	cpu->reg[REG_rsp] -= sizeof(uint64_t);
	cpu_write(cpu, (void*) cpu->reg[REG_rsp], val);
}

/// Shell for cpu-pop (not a cmd)
static inline uint64_t cpu_pop(struct cpu_t *cpu)
{
	register uint64_t val = cpu_read(cpu, (void*) cpu->reg[REG_rsp]);
	cpu->reg[REG_rsp] += sizeof(uint64_t);
	return val;
}

/// Syscall handler
int cpu_syscall(struct cpu_t *cpu)
{
	uint64_t tmp = 0;
	switch (cpu->reg[REG_rax]) {
	case SYSCALL_EXIT:
		cpu->trap = TRAP_EXIT;
		break;
	case SYSCALL_OUTQ:
		tmp = cpu_pop(cpu);
		printf("\n%llu\n", tmp);
		cpu_push(cpu, tmp);
		cpu->trap = TRAP_NO_TRAP;
		break;
	// other syscalls here
	default:
		cpu->trap = TRAP_ERROR_SYSCALL;
		assert(0);
		return -1;
	}
	return 0;
}

int cpu_run(struct cpu_t *cpu)
{
	while (1) {
		// Handle cpu-traps
		switch (cpu->trap) {
		case TRAP_NO_TRAP:
			break;
		case TRAP_EXIT:
			return 0;
		case TRAP_SYSCALL:
			if (cpu_syscall(cpu) == -1) {
				assert(0);
				return -1;
			}
			continue;
		default:
			fprintf(stderr, "Unknown trap: %llu\n", (unsigned long) cpu->trap);
			assert(0);
			return -1;
		}

		#ifdef NDEBUG
		#define DEF_CMD(name)				\
		case CMD_##name:				\
			cpu_cmd_##name(cpu);			\
			break;
		#else
		#define DEF_CMD(name)				\
		case CMD_##name:				\
			fprintf(stderr, "Exec: " #name "\n");	\
			cpu_cmd_##name(cpu);			\
			break;
		#endif

		uint8_t cmd = cpu_rip_byte(cpu);
		if (cpu->trap == TRAP_ERROR_MEMORY) {
			assert(0);
			return -1;
		}

		switch (cmd) {
		#include "cmd_list.h"
		#undef DEF_CMD
		default:
			cpu->trap = TRAP_ERROR_INSTR;
			assert(0);
			return -1;
		}
	}
	return -1;
}





static inline void cpu_cmd_nop(struct cpu_t *cpu)
{
	// nop
}

static inline void cpu_cmd_syscall(struct cpu_t *cpu)
{
	cpu->trap = TRAP_SYSCALL;
}

static inline void cpu_cmd_pushq(struct cpu_t *cpu)
{
	cpu_push(cpu, cpu_rip_qword(cpu));
}

static inline void cpu_cmd_pushr(struct cpu_t *cpu)
{
	cpu_push(cpu, cpu->reg[cpu_rip_byte(cpu)]);
}
