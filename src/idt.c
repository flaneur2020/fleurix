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

uint irq_routines[16] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0
}; 

void set_idt_gate(int num, uint base, uint16_t sel, uint8_t flags) {
    idt[num].base_lo = (base & 0xFFFF);
    idt[num].base_hi = (base >> 16) & 0xFFFF;
    idt[num].sel = sel;
    idt[num].always0 = 0;
    idt[num].flags = flags;
}

void init_trap(){
    int i;
    for(i=0; i<32;i++){
        set_idt_gate(i, _intv[i], KERN_CS, 0x8e);
    }
    // syscall
    set_idt_gate(0x80, _intv[0x80], KERN_CS, 0x8e);
}

void init_irq(){
    // remap the irq
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

void int_common_handler(struct regs *r) {
    // trap
    if (r->int_no < 32) {
        printf("Panic: Exception %s \n", fault_messages[r->int_no]);
        for (;;);
    }
    // syscall
    // intr
}

void init_idt(){
    // init idt_desc
    idt_desc.limit = (sizeof (struct idt_entry) * 256) - 1;
    idt_desc.base = &idt;
    // init 
    // init the idt as 0
    memset(&idt, 0, sizeof(struct idt_entry) * 256);
    lidt(idt_desc);
    init_trap();
}
