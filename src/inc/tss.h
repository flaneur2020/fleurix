struct tss_desc {
    uint        link, esp0;
    ushort      ss0, __1;
    uint        esp1;
    ushort      ss1, __2;
    uint        esp2;
    ushort      ss2, __3;
    uint        cr3, eip, eflags;
    uint        eax, ecx, edx, ebx, esp, ebp, esi, edi;
    ushort      es, __4;
    ushort      cs, __5;
    ushort      ss, __6;
    ushort      ds, __7;
    ushort      fs, __8;
    ushort      gs, __9;
    ushort      ldt,__10;
    ushort      trap, iomb;
} __attribute__((packed));

extern struct tss_desc tss;
