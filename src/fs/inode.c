#include <param.h>
#include <x86.h>
#include <proto.h>
#include <proc.h>
// 
#include <buf.h>
#include <conf.h>
//
#include <super.h>
#include <inode.h>
#include <fs.h>

struct inode inode[NINODE];

/*
 * this file indicated some operation on inode. such as
 * iget -> get an locked inode.
 * iput -> release an locked inode.
 *
 * */

/* get an (locked) inode via an number 
 * if the inode is in cache, return it right now.
 *
 * note: you may compare this code with getblk, the idea here
 * is common used among everywhere on resource allocation.
 * it returns ONLY locked inode just as B_BUSY in getblk, just 
 * to prevent other processes' accessing.
 *
 * TODO: didn't consider mount yet.
 * */
void iget(ushort dev, uint num){
    struct buf *bp;
    struct inode *ip;

_loop:
    for(ip=&inode[0]; ip<&inode[NINODE]; ip++){
        if (ip->i_dev==dev && ip->i_num==num) {
            // if found but locked.
            if (ip->i_flag & I_LOCK) {
                ip->i_flag |= I_WANTED;
                sleep(ip, PINOD);
                goto _loop;
            }
            // TODO: consider mountint.
            // if found straightly
            ip->i_count++;
            ip->i_flag |= I_LOCK;
            return ip;
        }
        // if not on target, but got a free slot. time to read disk
        if (ip->i_count==0) {
            ip->i_dev = dev;
            ip->i_num = num;
            ip->i_flag = I_LOCK;
            ip->i_count++;
            readi(ip);
            return ip;
        }
    }
    // if not found (no free slot) 
    printf("inode table overflow.\n");
    return NULL;
}

/* release an inode */
void iput(){
}

/* read/write a inode from disk */
void readi(struct inode *ip){
    struct super *sp;
    struct inode *tip;
    struct buf *bp;
    uint lba;

    sp = get_super(ip->i_dev);
    if (sp==NULL){
        panic("error on reading a super");
    }
    lba = 2 + (sp->s_nimap_blk) + (sp->s_nzmap_blk) + (ip->i_num-1)/NINO_PER_BLK;
    bp = bread(ip->i_dev, lba);
    if (bp==NULL) {
        panic("error on reading an inode");
    }
    tip = (struct inode*)bp->b_addr;
    memcpy(ip, &tip[(ip->i_num-1)%NINO_PER_BLK], sizeof(struct d_inode));
}

void writei(){
}

/* lock/unlock */
void lock_inode(){
}

void unlock_inode(){
}

void dump_inode(struct inode *ip){
    printf("i_mode:%x\n", ip->i_mode);
    printf("i_uid:%x\n", ip->i_uid);
    printf("i_size:%x\n", ip->i_size);
    printf("i_time:%x\n", ip->i_time);
    printf("i_gid:%x\n", ip->i_gid);
    printf("i_nlinks:%x\n", ip->i_nlinks);
    printf("i_count:%x\n", ip->i_count);
    printf("i_dev:%x\n", ip->i_dev);
    printf("i_num:%x\n", ip->i_num);
    printf("i_flag:%x\n", ip->i_flag);
}
