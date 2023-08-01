# Overview

This project is aimed at understanding how to use the ptrace interface to use a
tracer process to control a tracee program.

The milestones in this project are: 

- [x] Attach to a running process;
- [x] Read & save a process' registers (PEEK);
- [x] Set the registers and write data to the process' address space (POKE);
- [x] Execute a simple system call;
- [ ] Restore the registers to their previous state & continue execution;
