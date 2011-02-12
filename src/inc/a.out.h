#ifndef A_OUT_H
#define A_OUT_H

struct exec {
  uint      a_magic;	    /* Use macros N_MAGIC, etc for access */
  uint      a_text;	    	/* length of text, in bytes */
  uint      a_data;	    	/* length of data, in bytes */
  uint      a_bss;	    	/* length of uninitialized data area for file, in bytes */
  uint      a_syms;	    	/* length of symbol table data in file, in bytes */
  uint      a_entry;		/* start address */
  uint      a_trsize;		/* length of relocation info for text, in bytes */
  uint      a_drsize;		/* length of relocation info for data, in bytes */
}; 

#endif
