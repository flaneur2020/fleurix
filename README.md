Fleurix
=======

About
-----

A prototype Unix-like kernel.

It contains 37 system calls, 7000 lines of C, over 200 lines of assembly, developed in a bochs environment. Admittedly, it can't yet shed the label of being a "toy", but it has accomplished its design goal, which is to *get it running* :)

It has:
-------

- A basic process management with `fork()`, `exec()`, `exit()`, `wait()`, signal handlings, and so on. Support a.out executable format.
- A pure paging memory management with copy-on-write and demand paging. Support 4GB of address space for each process, sharing 128MB of kernel address space.
- A simple minixv1 fs implementation. It's simple, and it can takes advantage of tools under Linux such as mkfs.minix, fsck.minix, etc.
- A simple `kmalloc()`.
- A simple terminal.

Syscalls
--------

```c
static inline _SYS0(int, debug);
static inline _SYS2(int, access, char*, int);
static inline _SYS3(int, open, char*, int, int);
static inline _SYS2(int, creat, char*, int);
static inline _SYS1(int, close, int);
static inline _SYS3(int, fcntl, int, int, int);
static inline _SYS3(int, mknod, char*, int, int);
static inline _SYS3(int, write, int, char*, int);
static inline _SYS3(int, read, int, char*, int);
static inline _SYS3(int, lseek, int, int, int);
static inline _SYS1(int, chdir, char*);
static inline _SYS1(int, chroot, char*);
static inline _SYS1(int, dup, int);
static inline _SYS2(int, dup2, int, int);
static inline _SYS2(int, link, char*, char*);
static inline _SYS1(int, unlink, char*);
static inline _SYS2(int, stat, char*, struct stat*);
static inline _SYS2(int, fstat, int, struct stat*);
static inline _SYS0(int, fork);
static inline _SYS2(int, exec, char*, char**);
static inline _SYS1(int, _exit, int);
static inline _SYS1(int, nice, int);
static inline _SYS0(int, getpid);
static inline _SYS0(int, getppid);
static inline _SYS0(int, getuid);
static inline _SYS0(int, getgid);
static inline _SYS0(int, geteuid);
static inline _SYS0(int, getegid);
static inline _SYS0(int, getpgrp);
static inline _SYS0(int, setpgrp);
static inline _SYS2(int, setreuid, int, int);
static inline _SYS2(int, setregid, int, int);
static inline _SYS2(int, kill, int, int);
static inline _SYS2(int, signal, int, uint);
static inline _SYS3(int, sigaction, int, struct sigaction*, struct sigaction*);
static inline _SYS0(int, sigreturn);
static inline _SYS3(int, waitpid, int, int*, int);
static inline _SYS1(int, wait, int*);
static inline _SYS0(int, pause);
```

Delayed yet :(
--------------

- not have pipes.
- not support swap.
- not POSIX yet.
- not support hard disk partitioning.
- only supports up to 128MB of physical memory.

Compiling & Testing
-------------------

tools: rake, binutils(gcc, ld, as), nasm, bochs, mkfs.minix

```
git clone git@github.com:Fleurer/fleurix.git
cd fleurix
rake
```

Write your own programs under Fleurix
-------------------------------------

Create a new .c file in the usr/ directory, then execute:

```
rake
```

Contributing
------------

1. Fork it.
2. Create a branch (`git checkout -b my_markup`)
3. Commit your changes (`git commit -am "Added Snarkdown"`)
4. Push to the branch (`git push origin my_markup`)
5. Create an Issue with a link to your branch
6. Enjoy a refreshing Diet Coke and wait
