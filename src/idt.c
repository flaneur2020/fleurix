#include <sys.h>

// from intv.S
extern uint _intv[256];

// lidt idt_desc
struct idt_entry   idt[256];
struct idt_desc    idt_desc;

// handlers to each int_no
// which inited as 0
static uint int_routines[256]; 

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

void idt_set_gate(uint num, uint base, ushort sel, uchar type, uchar dpl) {
    idt[num].base_lo    = (base & 0xFFFF);
    idt[num].base_hi    = (base >> 16) & 0xFFFF;
    idt[num].sel        = sel;
    idt[num].dpl        = dpl;
    idt[num].type       = type;
    idt[num].always0    = 0;
    idt[num].p          = 1;
    idt[num].sys        = 0;
}

void syst_set_gate(uint num, uint base){
    idt_set_gate(num, base, KERN_CS, STS_TG, 3);
}
void intr_set_gate(uint num, uint base){
    idt_set_gate(num, base, KERN_CS, STS_IG, 0);
}
void trap_set_gate(uint num, uint base){
    idt_set_gate(num, base, KERN_CS, STS_TG, 0);
}

void intv_init(){
    int i;
    for(i=0;  i<32; i++){ trap_set_gate(i, _intv[i]); }
    for(i=32; i<48; i++){ intr_set_gate(i, _intv[i]); }
    syst_set_gate(0x03, _intv[0x03]); // int3
    syst_set_gate(0x04, _intv[0x04]); // overflow
    syst_set_gate(0x05, _intv[0x05]); // bound
    syst_set_gate(0x80, _intv[0x80]); // syscall
}

void idt_flush(){
    asm volatile(
        "lidt %0"
        :: "m"(idt_desc));
}

/**********************************************************************/

// if you do not remap irq, a Double Fault comes along with every intrupt
// 0 -> 32
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

void int_common_handler(struct regs *r) {
    void (*handler)(struct regs *r);
    handler = int_routines[r->int_no]; 
    // trap
    if (r->int_no < 32) {
        if (handler){
            handler(r);
            return;
        }
        printf("Panic: Exception: %s \n", fault_messages[r->int_no]);
        print_regs(r);
        for(;;);
    }
    // irq
    if (r->int_no >= 32) {
        if (r->int_no >= 40) {
            outb(0xA0, 0x20);
        }
        outb(0x20, 0x20);
        if (handler){
            handler(r);
        }
    }
    // syscall
    // intr
}

void int_set_handler(int num, void (*handler)(struct regs *r)){
    int_routines[num] = handler;
}

/***********************************************************************************/

void print_regs(struct regs *r){
    printf("gs = %x, fs = %x, es = %x, ds = %x\n", r->gs, r->fs, r->es, r->ds);
    printf("edi = %x, esi = %x, ebp = %x, esp = %x \n",r->edi, r->esi, r->ebp, r->esp);
    printf("ebx = %x, edx = %x, ecx = %x, eax = %x \n",r->ebx, r->edx, r->ecx, r->eax);
    printf("int_no = %x, err_code = %x\n", r->int_no, r->err_code);
    printf("eip = %x, cs = %x, eflags = %x\n", r->eip, r->cs, r->eflags);
    printf("useresp = %x, ss = %x \n", r->useresp, r->ss);
}

/***********************************************************************************/

void idt_init(){
    // init idt_desc
    idt_desc.limit = (sizeof (struct idt_entry) * 256) - 1;
    idt_desc.base = &idt;
    // init irq
    irq_remap();
    // load intr vectors and lidt 
    intv_init();
    idt_flush();
}

