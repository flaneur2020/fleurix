Fleurix
=======

About
-----

一个雏形的Unix-like内核。

37个系统调用，七千行C，二百多行汇编，在bochs之上。诚然还脱不去“玩具”的标签，不过也算完成了它的设计目标，那就是*跑起来* :)

It has：
-------
* minix v1的文件系统。原理简单，而且可以利用linux下的mkfs.minix，fsck.minix等工具。
* fork()/exec()/exit()等等。a.out的可执行格式，实现了写时复制与请求调页。
* 信号。
* 一个纯分页的内存管理系统，每个进程4gb的地址空间，共享128mb的内核地址空间。至少比Linux0.11中的段页式内存管理方式更加灵活。
* 一个简单的kmalloc()(可惜没大用上)。
* 一个简单的终端。

### Syscalls

    /* in src/inc/unistd.h */
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
    //
    static inline _SYS0(int, fork); 
    static inline _SYS2(int, exec, char*, char**);
    static inline _SYS1(int, _exit, int);
    //
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
    //
    static inline _SYS2(int, kill, int, int);
    static inline _SYS2(int, signal, int, uint);
    static inline _SYS3(int, sigaction, int, struct sigaction*, struct sigaction*);
    static inline _SYS0(int, sigreturn);
    static inline _SYS3(int, waitpid, int, int*, int);
    static inline _SYS1(int, wait, int*);
    static inline _SYS0(int, pause);


Delayed yet :(
--------------
* 没有管道
* 没有swap
* 还不是基于POSIX
* 不支持硬盘分区
* 只支持128mb的物理内存

所以在真机上可能不靠谱 :(

Compiling & Testing
-------------------

编译环境: ubuntu

工具: rake, binutils(gcc, ld, as), nasm, bochs, mkfs.minix

    git clone git@github.com:Fleurer/fleurix.git
    cd fleurix
    rake


Write your own programs under Fleurix
-------------------------------------

在usr目录下新建一个.c文件。

    rake


Contributing
------------

1. Fork it.
2. Create a branch (`git checkout -b my_markup`)
3. Commit your changes (`git commit -am "Added Snarkdown"`)
4. Push to the branch (`git push origin my_markup`)
5. Create an [Issue][1] with a link to your branch
6. Enjoy a refreshing Diet Coke and wait
