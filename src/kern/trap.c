#include <param.h>
#include <x86.h>
#include <kern.h>
#include <proc.h>

// from hwint.S
extern uint _hwint[256];

// lidt idt_desc
struct gate_desc   idt[256];
struct idt_desc    idt_desc;

// handlers to each int_no
// which inited as 0
static uint int_routines[256] = {0, }; 

static char *fault_messages[] = {
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

/****************************************************************************/

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

static inline void set_syst_gate(uint nr, uint base){
    idt_set_gate(nr, base, KERN_CS, STS_TRG, 3);
}
static inline void set_intr_gate(uint nr, uint base){
    idt_set_gate(nr, base, KERN_CS, STS_IG, 0);
}
static inline void set_trap_gate(uint nr, uint base){
    idt_set_gate(nr, base, KERN_CS, STS_TRG, 0);
}

void hwint_init(){
    int i;
    for(i=0;  i<32; i++){ set_trap_gate(i, _hwint[i]); }
    for(i=32; i<48; i++){ set_intr_gate(i, _hwint[i]); }
    set_syst_gate(0x03, _hwint[0x03]); // int3
    set_syst_gate(0x04, _hwint[0x04]); // overflow
    set_syst_gate(0x05, _hwint[0x05]); // bound
    set_syst_gate(0x80, _hwint[0x80]); // syscall
    // Each handler handled in his file.  
    set_hwint(0x80, &do_syscall);      // in syscall.c
}

void flush_idt(struct idt_desc idtd){
    asm volatile(
        "lidt %0"
        :: "m"(idtd));
}

/**********************************************************************/

// if you do not remap irq, a Double Fault comes along with every intrupt
// 0  -> 32
// 15 -> 47
static void irq_remap(){
    outb(0x20, 0x11);
    outb(0xA0, 0x11);
    outb(0x21, 0x20);
    outb(0xA1, 0x28);
    outb(0x21, 0x04);
    outb(0xA1, 0x02);
    outb(0x21, 0x01);
    outb(0xA1, 0x01);
    outb(0x21, 0x0);
    outb(0xA1, 0x0);
}

/**********************************************************************/

void hwint_common(struct trap_frame *tf) {
    void (*handler)(struct trap_frame *tf);
    handler = int_routines[tf->int_no]; 
    if (current!=NULL) {
        current->p_trap = tf;
    }
    // trap
    if (tf->int_no < 32) {
        if (handler){
            handler(tf);
            return;
        }
        printf("Exception: %s \n", fault_messages[tf->int_no]);
        debug_regs(tf);
        for(;;);
    }
    // irq, syscall and blah~
    if (tf->int_no >= 32) {
        if (tf->int_no >= 40) {
            outb(0xA0, 0x20);
        }
        outb(0x20, 0x20);
        if (handler){
            handler(tf);
        }
    }
    // restore the trap frame
    if (current!=NULL) {
        current->p_trap = tf;
    }
}

void set_hwint(int nr, void (*handler)(struct trap_frame *tf)){
    int_routines[nr] = handler;
}

/***********************************************************************************/

void debug_regs(struct trap_frame *tf){
    printf("gs = %x, fs = %x, es = %x, ds = %x\n", tf->gs, tf->fs, tf->es, tf->ds);
    printf("edi = %x, esi = %x, ebp = %x, esp = %x \n",tf->edi, tf->esi, tf->ebp, tf->esp);
    printf("ebx = %x, edx = %x, ecx = %x, eax = %x \n",tf->ebx, tf->edx, tf->ecx, tf->eax);
    printf("int_no = %x, err_code = %x\n", tf->int_no, tf->err_code);
    printf("eip = %x, cs = %x, eflags = %x\n", tf->eip, tf->cs, tf->eflags);
    printf("esp3 = %x, ss3 = %x \n", tf->esp3, tf->ss3);
    uint cr2, kern_ss;
    asm("mov %%cr2, %%eax":"=a"(cr2));
    printf("cr2 = %x, ", cr2);
    asm("mov %%ss, %%eax":"=a"(kern_ss));
    printf("kern_ss = %x\n", kern_ss);
}

/***********************************************************************************/

void idt_init(){
    // init idt_desc
    idt_desc.limit = (sizeof (struct gate_desc) * 256) - 1;
    idt_desc.base = &idt;
    // init irq
    irq_remap();
    // load intr vectors and lidt 
    hwint_init();
    flush_idt(idt_desc);
}

