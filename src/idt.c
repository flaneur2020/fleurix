#include <sys.h>

extern uint _intv[256];

struct idt_entry   idt[256];
struct idt_desc    idt_desc;

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

// which inited as 0
uint int_routines[256]; 

/****************************************************************************/

void idt_set_gate(int num, uint base, ushort sel, uchar flags) {
    idt[num].base_lo = (base & 0xFFFF);
    idt[num].base_hi = (base >> 16) & 0xFFFF;
    idt[num].sel = sel;
    idt[num].always0 = 0;
    idt[num].flags = flags;
}

void intv_install(){
    int i;
    for(i=0; i<256;i++){
        idt_set_gate(i, _intv[i], KERN_CS, 0x8e);
    }
    // syscall
    idt_set_gate(0x80, _intv[0x80], KERN_CS, 0x8e);
}

/**********************************************************************/

// if you do not remap irq, a Double Fault comes along with every intrupt
void irq_remap(){
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

void int_set_handler(int num, void (*handler)(struct regs *r)){
    int_routines[num] = handler;
}

/**********************************************************************/

void int_common_handler(struct regs *r) {
    void (*handler)(struct regs *r);
    handler = int_routines[r->int_no]; 
    // trap
    if (r->int_no < 32) {
        if (handler){
            handler(r);
        }
        else {
            printf("Panic: Unhandled Exception: %s \n", fault_messages[r->int_no]);
            print_regs(r);
            for(;;);
        }
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

/***********************************************************************************/
// helpers
void print_regs(struct regs *r){
    printf("gs = %x, fs = %x, es = %x, ds = %x\n", r->gs, r->fs, r->es, r->ds);
    printf("edi = %x, esi = %x, ebp = %x, esp = %x \n",r->edi, r->esi, r->ebp, r->esp);
    printf("ebx = %x, edx = %x, ecx = %x, eax = %x \n",r->ebx, r->edx, r->ecx, r->eax);
    printf("int_no = %x, err_code = %x\n", r->int_no, r->err_code);
    printf("eip = %x, cs = %x, eflags = %x\n", r->eip, r->cs, r->eflags);
    printf("useresp = %x, ss = %x \n", r->useresp, r->ss);
}

void idt_init(){
    // init idt_desc
    idt_desc.limit = (sizeof (struct idt_entry) * 256) - 1;
    idt_desc.base = &idt;
    // init irq
    irq_remap();
    memsetw(int_routines, 0, sizeof(uint)*256);
    // load it 
    intv_install();
    lidt(&idt_desc);
}

