#include "cpu.h"
#include <assert.h>

int main()
{
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
	return 0;
}
