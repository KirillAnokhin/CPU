#include "cpu_t.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int main()
{
	/*
	char *buf = calloc(1000, 1);
	buf[0] = CMD_pushq;
	*(uint64_t*) (buf + 1) = 123;
	buf[9] = CMD_syscall;

	struct cpu_t cpu = {};
	cpu.reg[REG_rax] = SYSCALL_OUTQ;
	cpu.reg[REG_rsp] = buf + 100;

	cpu_set_rip(&cpu, buf);
	cpu_set_mem(&cpu, buf, buf + 900);
	int tmp = cpu_run(&cpu);
	assert(tmp == 0);
	*/

	char *buf = calloc(1024, 1);
	buf[3] = CMD_syscall; 
	struct cpu_t cpu = {};
	cpu.rip = buf;
	cpu.reg[REG_rsp] = (uint64_t)(buf + 1024);
	cpu_set_mem(&cpu, buf, buf + 1024);
	int tmp = cpu_run(&cpu);

	printf("programm terminated with status %d\n", tmp);
	return 0;
}
