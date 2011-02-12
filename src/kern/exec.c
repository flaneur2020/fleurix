#include <param.h>
#include <x86.h>
#include <proto.h>
#include <proc.h>
#include <unistd.h>
//
#include <page.h>
#include <vm.h>
//
#include <buf.h>
#include <conf.h>
#include <hd.h>
//
#include <tty.h>
//
#include <super.h>
#include <inode.h>
#include <file.h>

/*
 * exec.c
 *
 * this file indicated the implementation of exec(char* path, char **argv).
 * exec() create a NEW address space, which overlapped the old forked address 
 * space, make it demand-reading.
 * */

int do_exec(char *path, char *argv){
}
