#include <sys.h>

extern void _isr0();
extern void _isr1();
extern void _isr2();
extern void _isr3();
extern void _isr4();
extern void _isr5();
extern void _isr6();
extern void _isr7();
extern void _isr8();
extern void _isr9();
extern void _isr10();
extern void _isr11();
extern void _isr12();
extern void _isr13();
extern void _isr14();
extern void _isr15();
extern void _isr16();
extern void _isr17();
extern void _isr18();
extern void _isr19();
extern void _isr20();
extern void _isr21();
extern void _isr22();
extern void _isr23();
extern void _isr24();
extern void _isr25();
extern void _isr26();
extern void _isr27();
extern void _isr28();
extern void _isr29();
extern void _isr30();
extern void _isr31();

struct idt_entry   idt[256];
struct idt_desc    idt_desc;

void idt_set_gate(int num, uint base, uint16_t sel, uint8_t flags) {
    idt[num].base_lo = (base & 0xFFFF);
    idt[num].base_hi = (base >> 16) & 0xFFFF;
    idt[num].sel = sel;
    idt[num].always0 = 0;
    idt[num].flags = flags;
}

void init_isr(){
    idt_set_gate(0, _isr0, 0x08, 0x8E);
    idt_set_gate(1, _isr1, 0x08, 0x8E);
    idt_set_gate(2, _isr2, 0x08, 0x8E);
    idt_set_gate(3, _isr3, 0x08, 0x8E);
    idt_set_gate(4, _isr4, 0x08, 0x8E);
    idt_set_gate(5, _isr5, 0x08, 0x8E);
    idt_set_gate(6, _isr6, 0x08, 0x8E);
    idt_set_gate(7, _isr7, 0x08, 0x8E);

    idt_set_gate(8, _isr8, 0x08, 0x8E);
    idt_set_gate(9, _isr9, 0x08, 0x8E);
    idt_set_gate(10, _isr10, 0x08, 0x8E);
    idt_set_gate(11, _isr11, 0x08, 0x8E);
    idt_set_gate(12, _isr12, 0x08, 0x8E);
    idt_set_gate(13, _isr13, 0x08, 0x8E);
    idt_set_gate(14, _isr14, 0x08, 0x8E);
    idt_set_gate(15, _isr15, 0x08, 0x8E);

    idt_set_gate(16, _isr16, 0x08, 0x8E);
    idt_set_gate(17, _isr17, 0x08, 0x8E);
    idt_set_gate(18, _isr18, 0x08, 0x8E);
    idt_set_gate(19, _isr19, 0x08, 0x8E);
    idt_set_gate(20, _isr20, 0x08, 0x8E);
    idt_set_gate(21, _isr21, 0x08, 0x8E);
    idt_set_gate(22, _isr22, 0x08, 0x8E);
    idt_set_gate(23, _isr23, 0x08, 0x8E);

    idt_set_gate(24, _isr24, 0x08, 0x8E);
    idt_set_gate(25, _isr25, 0x08, 0x8E);
    idt_set_gate(26, _isr26, 0x08, 0x8E);
    idt_set_gate(27, _isr27, 0x08, 0x8E);
    idt_set_gate(28, _isr28, 0x08, 0x8E);
    idt_set_gate(29, _isr29, 0x08, 0x8E);
    idt_set_gate(30, _isr30, 0x08, 0x8E);
    idt_set_gate(31, _isr31, 0x08, 0x8E);
}

unsigned char *fault_messages[] = {
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

void fault_handler(struct regs *r) {
    if (r->int_no < 32) {
        printf("Panic: Exception %s \n", fault_messages[r->int_no]);
        for (;;);
    }
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
