#include <sys.h>

extern uint _intv[256];

struct idt_entry   idt[256];
struct idt_desc    idt_desc;

void set_idt_gate(int num, uint base, uint16_t sel, uint8_t flags) {
    idt[num].base_lo = (base & 0xFFFF);
    idt[num].base_hi = (base >> 16) & 0xFFFF;
    idt[num].sel = sel;
    idt[num].always0 = 0;
    idt[num].flags = flags;
}

void set_trap_gate(uint num, uint addr){
}

void set_sysm_gate(uint num, uint addr){
}

void set_intr_gate(uint num, uint addr){
}

void init_isr(){
    int i;
    for(i=0; i<32;i++){
        set_idt_gate(i, _intv[i], 0x08, 0x8E);
    }
}

char *fault_messages[] = {
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Into Detected Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",

    "Double Fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment Not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Unknown Interrupt",

    "Coprocessor Fault",
    "Alignment Check",
    "Machine Check",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",

    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
};

void int_handler(struct regs *r) {
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
    init_isr();
}
