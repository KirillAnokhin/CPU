#ifndef _CPU_H_
#define _CPU_H_

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>

enum CPU_TRAPS {
	TRAP_NO_TRAP = 0,
	TRAP_EXIT,
	TRAP_SYSCALL,
	TRAP_ERROR_MEMORY,
	TRAP_ERROR_INSTR,
	TRAP_ERROR_SYSCALL,
};

enum CPU_SYSCALLS {
	SYSCALL_EXIT,
	SYSCALL_OUTQ,
};

enum CPU_REGS {
#define DEF_REG(name) REG_##name,
#include "reg_list.h"
	REG_MAX,
#undef DEF_REG
};

enum CPU_CMDS {
#define DEF_CMD(name) CMD_##name,
#include "cmd_list.h"
	CMD_MAX,
#undef DEF_CMD
};

struct cpu_t
{
	void *mem_min;
	void *mem_max;
	void *rip;
	uint64_t reg[REG_MAX];
	uint64_t trap;
};

int cpu_init(struct cpu_t *cpu);
int cpu_set_rip(struct cpu_t *cpu, void *code);
int cpu_set_mem(struct cpu_t *cpu, void *mem_min, void *mem_max);
int cpu_run (struct cpu_t *cpu);

#endif // _CPU_H_
