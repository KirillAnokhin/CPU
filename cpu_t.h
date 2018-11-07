#ifndef _CPU_T_H_
#define _CPU_T_H_

#include <stddef.h>
#include <stdint.h>

enum CMD_CODES
{
	#define DEF_CMD(name)	CMD_##name,
	#include "cmd_list.h"
	CMD_MAX,
	#undef DEF_CMD
};

enum REG_CODES
{
	#define DEF_REG(name)	REG_##name,
	#include "reg_list.h"
	REG_MAX,
	#undef DEF_REG
};

enum TRAP_CODES
{
	TRAP_NO_TRAP = 0,
	TRAP_EXIT,
	TRAP_SYSCALL,
	TRAP_ERROR_MEMORY,
	TRAP_ERROR_INSTR,
	TRAP_ERROR_SYSCALL,
};

enum SYSCALL_CODES
{
	SYSCALL_EXIT,
	SYSCALL_OUTQ,
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
int cpu_set_rsp(struct cpu_t *cpu, void *rsp);
int cpu_set_mem(struct cpu_t *cpu, void *mem_min, void *mem_max);
int cpu_run (struct cpu_t *cpu);

long code_asm (char    *text, uint8_t **code_p);
long code_dasm(uint8_t *code, char **text_p, long size);

#endif // _CPU_T_H_
