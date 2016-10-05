#pragma once

/***
 * handlecrash.h
 * https://github.com/angelog/handlecrash.h
 */

#include <stdio.h>
#include <stdlib.h>

#include <time.h>
#include <execinfo.h>
#include <signal.h>
#include <err.h>

#ifndef HC_MAX_STACK_FRAMES
#define HC_MAX_STACK_FRAMES 64
#endif

#if __GNUC__ < 5 || (defined(__clang__) && (__clang_major__ <= 3 && __clang_minor__ < 8))
#define HC_REGFMT16 "%016lx"
#define HC_REGFMT8 "%08lx"
#define HC_REGFMT4 "%04lx"
#else
#define HC_REGFMT16 "%016llx"
#define HC_REGFMT8 "%08llx"
#define HC_REGFMT4 "%04llx"
#endif

void hc_handler_posix(int sig, siginfo_t* siginfo, void* context)
{
#define hc_print(fmt, ...) printf(fmt, ##__VA_ARGS__); fprintf(fpCrash, fmt, ##__VA_ARGS__);

	char fnmBuffer[128];
	sprintf(fnmBuffer, "/tmp/crash_%ld.log", time(0));

	FILE* fpCrash = fopen(fnmBuffer, "w");

	hc_print("***\n");
	hc_print("*** APPLICATION CRASH\n");
	hc_print("***\n");

	// I couldn't get these to return reliable information :(
	//hc_print("***    Process ID: %d\n", siginfo->si_pid);
	//hc_print("***       User ID: %d\n", siginfo->si_uid);
#ifdef __LP64__
	hc_print("***          Arch: x64\n");
#else
	hc_print("***          Arch: x86\n");
#endif
	hc_print("***     Dump file: %s\n", fnmBuffer);
	hc_print("***\n");

	hc_print("***        Signal: ");
	switch (sig) {
		case SIGSEGV: hc_print("SIGSEGV"); break;
		case SIGILL: hc_print("SIGILL"); break;
		default: hc_print("???"); break;
	}
	hc_print("\n");
	hc_print("***  Signal errno: %d\n", siginfo->si_errno);
	hc_print("***   Signal code: %d\n", siginfo->si_code);

	hc_print("*** Fault address: %p\n", siginfo->si_addr);
	hc_print("***\n");

	hc_print("*** Backtrace ");

	void** stackBuffer = (void**)alloca(HC_MAX_STACK_FRAMES * sizeof(void*));
	int size = backtrace(stackBuffer, HC_MAX_STACK_FRAMES);
	hc_print("(%d frames):\n", size);
	char** messages = backtrace_symbols(stackBuffer, size);

	for (int i = 0; i < size; i++) {
		hc_print("***   #%d\t%p\t%s\n", i, stackBuffer[i], messages[i]);
	}

	hc_print("***\n");

	hc_print("*** Registers:\n");

	struct ucontext* ctx = (ucontext*)context;
	#define RV(reg) ctx->uc_mcontext.gregs[reg]
	#define FPSTe(i) ctx->uc_mcontext.fpregs->_st[i].exponent
	#define FPSTsa(i) ctx->uc_mcontext.fpregs->_st[i].significand[3] << 16 | ctx->uc_mcontext.fpregs->_st[i].significand[2]
	#define FPSTsb(i) ctx->uc_mcontext.fpregs->_st[i].significand[1] << 16 | ctx->uc_mcontext.fpregs->_st[i].significand[0]

#ifdef __LP64__
	hc_print("***   RAX: " HC_REGFMT16 "     RBX: " HC_REGFMT16 "     RCX: " HC_REGFMT16 "\n", RV(REG_RAX), RV(REG_RBX), RV(REG_RCX));
	hc_print("***   RDX: " HC_REGFMT16 "     RSI: " HC_REGFMT16 "     RDI: " HC_REGFMT16 "\n", RV(REG_RDX), RV(REG_RSI), RV(REG_RDI));
	hc_print("***   RBP: " HC_REGFMT16 "      R8: " HC_REGFMT16 "      R9: " HC_REGFMT16 "\n", RV(REG_RBP), RV( REG_R8), RV( REG_R9));
	hc_print("***   R10: " HC_REGFMT16 "     R11: " HC_REGFMT16 "     R12: " HC_REGFMT16 "\n", RV(REG_R10), RV(REG_R11), RV(REG_R12));
	hc_print("***   R13: " HC_REGFMT16 "     R14: " HC_REGFMT16 "     R15: " HC_REGFMT16 "\n", RV(REG_R13), RV(REG_R14), RV(REG_R15));
	hc_print("***   RSP: " HC_REGFMT16 "\n", RV(REG_RSP));
	hc_print("***\n");
	hc_print("***   RIP: " HC_REGFMT16 "     EFL: " HC_REGFMT8 "\n", RV(REG_RIP), RV(REG_EFL));
	hc_print("***\n");
	hc_print("***    CS: " HC_REGFMT4 "    FS: " HC_REGFMT4 "      GS: " HC_REGFMT4 "\n", RV(REG_CSGSFS) & 0xFFFF, (RV(REG_CSGSFS) >> 16) & 0xFFFF, (RV(REG_CSGSFS) >> 32) & 0xFFFF);
	hc_print("***\n");
	for (int i = 0; i < 8; i += 2) {
		hc_print("***   ST(%d) %04hx %08x%08x      ST(%d) %04hx %08x%08x\n", i, FPSTe(i), FPSTsa(i), FPSTsb(i), i + 1, FPSTe(i + 1), FPSTsa(i + 1), FPSTsb(i + 1));
	}
	hc_print("***\n");
	#define FPXMMa(i) ctx->uc_mcontext.fpregs->_xmm[i].element[1] << 8 | ctx->uc_mcontext.fpregs->_xmm[i].element[0]
	#define FPXMMb(i) ctx->uc_mcontext.fpregs->_xmm[i].element[3] << 24 | ctx->uc_mcontext.fpregs->_xmm[i].element[2] << 16
	for (int i = 0; i < 16; i += 2) {
		hc_print("***   XMM%d%s: %016x%016x     XMM%d%s: %016x%016x\n", i, i < 10 ? " " : "", FPXMMa(i), FPXMMb(i), i + 1, i < 10 ? " " : "", FPXMMa(i + 1), FPXMMb(i + 1));
	}
#else
	hc_print("***   EAX: %08x   EBX: %08x   ECX: %08x   EDX: %08x\n", RV(REG_EAX), RV(REG_EBX), RV(REG_ECX), RV(REG_EDX));
	hc_print("***   ESI: %08x   EDI: %08x   EBP: %08x   ESP: %08x\n", RV(REG_ESI), RV(REG_EDI), RV(REG_EBP), RV(REG_ESP));
	hc_print("***\n");
	hc_print("***   EIP: %08x   EFL: %08x\n", RV(REG_EIP), RV(REG_EFL));
	hc_print("***\n");
	hc_print("***   CS: %04x   DS: %04x   ES: %04x   FS: %04x   GS: %04x   SS: %04x\n", RV(REG_CS), RV(REG_DS), RV(REG_ES), RV(REG_FS), RV(REG_GS), RV(REG_SS));
	hc_print("***\n");
	for (int i = 0; i < 8; i += 2) {
		hc_print("***   ST(%d) %04hx %08x%08x      ST(%d) %04hx %08x%08x\n", i, FPSTe(i), FPSTsa(i), FPSTsb(i), i + 1, FPSTe(i + 1), FPSTsa(i + 1), FPSTsb(i + 1));
	}
#endif

	hc_print("***\n");

	FILE* fpMaps = fopen("/proc/self/maps", "r");
	if (fpMaps != 0) {
		hc_print("*** Memory map:\n");

		char* line = 0;
		size_t lineLen = 0;

		while (true) {
			if (getline(&line, &lineLen, fpMaps) == -1) {
				break;
			}
			hc_print("***   %s", line);
		}
		if (line) {
			free(line);
		}

		fclose(fpMaps);
	}

	hc_print("***\n");

	fclose(fpCrash);

	struct sigaction sa;
	sa.sa_handler = SIG_DFL;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(sig, &sa, 0);
	raise(sig);

#undef hc_print
}

void hc_install()
{
	stack_t ss;
	ss.ss_sp = malloc(SIGSTKSZ);
	ss.ss_size = SIGSTKSZ;
	ss.ss_flags = 0;

	if (sigaltstack(&ss, 0) != 0) { err(1, "sigaltstack"); }

	struct sigaction sig_action;
	sig_action.sa_sigaction = hc_handler_posix;
	sigemptyset(&sig_action.sa_mask);

	#ifdef __APPLE__
	sig_action.sa_flags = SA_SIGINFO;
	#else
	sig_action.sa_flags = SA_SIGINFO | SA_ONSTACK;
	#endif

	if (sigaction(SIGSEGV, &sig_action, 0) != 0) { err(1, "sigaction"); }
	if (sigaction(SIGILL, &sig_action, 0) != 0) { err(1, "sigaction"); }
}
