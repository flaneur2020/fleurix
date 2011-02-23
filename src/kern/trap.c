#include <param.h>
#include <x86.h>
#include <proto.h>
#include <proc.h>

// from hwint.S
extern uint _hwint[256];

// lidt idt_desc
struct gate_desc   idt[256];
struct idt_desc    idt_desc;

// handlers to each int_no
// which inited as 0
static uint hwint_routines[256] = {0, }; 

static char *fault_str[] = {
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Into Detected Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",
    //
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment Not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Unknown Interrupt",
    //
    "Coprocessor Fault",
    "Alignment Check",
    "Machine Check",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    //
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
};

/* ------------------------------------------------------------ */

/*
 * Remap the irq and initialize the IRQ mask. 
 *
 * note:If you do not remap irq, a Double Fault comes 
 * along with every interrupt.
 * After initialization, register 0xA1 and 0x21 are the 
 * hi/lo bytes of irq mask, respectively. 
 * */
void irq_init(){
    // hard coded, don't touch.
    outb(PIC1, 0x11);
    outb(PIC2, 0x11);
    outb(PIC1+1, IRQ0); // offset 1, 0-7
    outb(PIC2+1, IRQ0+8); // offset 2, 7-15
    outb(PIC1+1, 4); 
    outb(PIC2+1, 2);
    outb(PIC1+1, 0x01);
    outb(PIC2+1, 0x01);
    // Initialize IRQ mask has interrupt 2 enabled.
    outb(PIC1+1, 0xFF);
    outb(PIC2+1, 0xFF);
    irq_enable(2);
}

void irq_eoi(int nr){
    outb(PIC1, PIC_EOI);
    if (nr >= 40) {
        outb(PIC2, PIC_EOI);
    }
}

void irq_enable(uchar irq){
    ushort irq_mask = (inb(PIC2+1)<<8) + inb(PIC1+1);
    irq_mask &= ~(1<<irq);
    outb(PIC1+1, irq_mask);
    outb(PIC2+1, irq_mask >> 8);
}

/* ------------------------------------------------------------------- */

void idt_set_gate(uint nr, uint base, ushort sel, uchar type, uchar dpl) {
    idt[nr].base_lo    = (base & 0xFFFF);
    idt[nr].base_hi    = (base >> 16) & 0xFFFF;
    idt[nr].sel        = sel;
    idt[nr].dpl        = dpl;
    idt[nr].type       = type;
    idt[nr].always0    = 0;
    idt[nr].p          = 1;
    idt[nr].sys        = 0;
}

static inline void syst_gate(uint nr, uint base){
    idt_set_gate(nr, base, KERN_CS, STS_TRG, RING3);
}
static inline void intr_gate(uint nr, uint base){
    idt_set_gate(nr, base, KERN_CS, STS_IG, RING0);
}
static inline void trap_gate(uint nr, uint base){
    idt_set_gate(nr, base, KERN_CS, STS_TRG, RING0);
}

void hwint_init(){
    int i;
    for(i=0;  i<32; i++)
        trap_gate(i, _hwint[i]);
    for(i=32; i<48; i++) 
        intr_gate(i, _hwint[i]);
    syst_gate(0x03, _hwint[0x03]); // int3
    syst_gate(0x04, _hwint[0x04]); // overflow
    syst_gate(0x05, _hwint[0x05]); // bound
    syst_gate(0x80, _hwint[0x80]); // syscall
    // Each handler handled in his file.  
    set_hwint(0x80, &do_syscall);      // in syscall.c
}

void lidt(struct idt_desc idtd){
    asm volatile(
        "lidt %0"
        :: "m"(idtd));
}

/* ------------------------------------------------------------------- */

/*
 * The comman handler for all IRQ request as a dispatcher. Each irq 
 * handler were held inside the array *hwint_routines*.
 *
 * note: While an IRQ were recieved, we have to notice the 8259 chip
 * that End of Interrupt via a PIC_EOI. If the IRQ came from the Master 
 * PIC, it is sufficient to issue this command only to the Master PIC; 
 * however if the IRQ came from the Slave PIC, it is necessary to issue 
 * the command to both PIC chips.
 * */
void hwint_common(struct trap *tf) {
    void (*func)(struct trap *tf);

    func = hwint_routines[tf->int_no]; 
    // trap
    if (tf->int_no < 32) {
        if (func){
            func(tf);
        }
        else {
            printf("hwint_common: unhandled exception: %s \n", fault_str[tf->int_no]);
            debug_regs(tf);
            for(;;);
        }
    }
    // irq, syscall and blah~
    else {
        if (func)
            func(tf);
        irq_eoi(tf->int_no);
    }
    // on sheduling 
    // if the re-schedule flag is set, make an task swtch.
    // and make sure only swtch on returning to user mode,
    // thus to keep the kernel nonpremtive.
    setpri(cu);
    if ((tf->cs & 3)==RING3) {
        swtch();
    }
}

void set_hwint(int nr, void (*func)(struct trap *tf)){
    hwint_routines[nr] = func;
}

/***********************************************************************************/

void debug_regs(struct trap *tf){
    printf("gs = %x, fs = %x, es = %x, ds = %x\n", tf->gs, tf->fs, tf->es, tf->ds);
    printf("edi = %x, esi = %x, ebp = %x \n",tf->edi, tf->esi, tf->ebp);
    printf("ebx = %x, edx = %x, ecx = %x, eax = %x \n",tf->ebx, tf->edx, tf->ecx, tf->eax);
    printf("int_no = %x, err_code = %x\n", tf->int_no, tf->err_code);
    printf("eip = %x, cs = %x, eflags = %x\n", tf->eip, tf->cs, tf->eflags);
    printf("esp = %x, ss = %x \n", tf->esp, tf->ss);
    uint cr2, kern_ss;
    asm("mov %%cr2, %%eax":"=a"(cr2));
    printf("cr2 = %x, ", cr2);
    asm("mov %%ss, %%eax":"=a"(kern_ss));
    printf("kern_ss = %x\n", kern_ss);
}

/***********************************************************************************/

void idt_init(){
    // init idt_desc
    idt_desc.limit = (sizeof(struct gate_desc) * 256) - 1;
    idt_desc.base = &idt;
    // init irq
    irq_init();
    // load intr vectors and lidt 
    hwint_init();
    lidt(idt_desc);
}

