#ifndef A_OUT_H
#define A_OUT_H

struct ahead {
  uint      a_magic;	    /* Use macros N_MAGIC, etc for access */
  uint      a_tsize;    	/* size of text, in bytes */
  uint      a_dsize;	   	/* size of data, in bytes */
  uint      a_bsize;	   	/* size of uninitialized data area for file, in bytes */
  uint      a_syms;	    	/* size of symbol table data in file, in bytes */
  uint      a_entry;		/* start address */
  uint      a_trsize;		/* size of relocation info for text, in bytes */
  uint      a_drsize;		/* size of relocation info for data, in bytes */
};

#define NMAGIC 0x6400CC

#endif
