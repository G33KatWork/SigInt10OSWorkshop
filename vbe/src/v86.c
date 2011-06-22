#include <v86.h>
#include <gdt.h>
#include <tss.h>
#include <lib.h>
#include <print.h>
#include <idt.h>
#include <interrupts.h>
#include <eflags.h>
#include <ports.h>

tss kerntss;
tss v86tss;

void* realmodeCode;

void v86_init()
{
    print_string_static("v86_init()\n");
    
    //clear tss
    memset(&v86tss, 0, sizeof(v86tss));
    memset(&kerntss, 0, sizeof(kerntss));
    
    //register TSS for kernel
    gdt_set_descriptor(
        3,
        (uint32_t)&kerntss,
        sizeof(kerntss)-1,
        I86_GDT_DESC_PRESENT | I86_GDT_DESC_RING0 | I86_GDT_SYSDESC_TSS32,
        0
    );
    
    //register TSS for v86
    gdt_set_descriptor(
        4,
        (uint32_t)&v86tss,
        sizeof(v86tss)-1,
        I86_GDT_DESC_PRESENT | I86_GDT_DESC_RING0 | I86_GDT_SYSDESC_TSS32,
        0
    );
    
    //flush gdt
    //gdt_install();
    
    //set task register to kernel
    //asm volatile ("ltr 0x18");        //FFFFFFFFUUUUUUUUU!
    asm volatile (
        "mov $0x18, %eax\r\n"
        "ltr %ax\r\n"
    );
    
    //setup an HLT instruction as a return point for the BIOS interrupt
    realmodeCode = (void*)REALMODECODE;
    *((uint8_t*)realmodeCode) = 0xF4;   //HLT
}

static inline void v86_push16(uint16_t x)
{
   v86tss.esp = (uint16_t)(v86tss.esp - 2);
   *(uint16_t*)(v86tss.ss * 16 + v86tss.esp) = x;
}

static inline void v86_push32(uint32_t x)
{
   v86tss.esp = (uint16_t)(v86tss.esp - 4);
   *(uint32_t*)(v86tss.ss * 16 + v86tss.esp) = x;
}

static inline uint16_t v86_pop16(void)
{
   uint16_t result = *(uint16_t*)(v86tss.ss * 16 + v86tss.esp);
   v86tss.esp = (uint16_t)(v86tss.esp + 2);
   return result;
}

static inline uint32_t v86_pop32(void)
{
   uint32_t result = *(uint32_t*)(v86tss.ss * 16 + v86tss.esp);
   v86tss.esp = (uint16_t)(v86tss.esp + 4);
   return result;
}

static void v86_start_execution(void)
{
    interrupts_disable();
    
    //register the kernel task as handler for GPF, we'll handle this fault down below
    idt_set_gate(
        13,
        0,
        0x18,
        I86_IDT_ATTR_PRESENT | I86_IDT_ATTR_TASK_GATE | I86_IDT_ATTR_PRIV_KERNEL
    );
    
    //jump to v86 task
    print_string_static("Jumping to v86 task\n");
    asm volatile ("ljmp $0x20, $0x0");
    
    //if a GPF is thrown, we end up here
    //since the cpu switches back to the kernel task where we handle
    //the faulting instruction and jump back to the v86 task
    print_string_static("GPF occured\n");
    int errorCode;
    asm volatile ("pop %%eax" : "=a"(errorCode));
    
    int o32 = 0, a32 = 0;
    for(;;)
    {
        uint8_t* faultLoc = (uint8_t*)((v86tss.cs << 4) + v86tss.eip);
        switch(*faultLoc)
        {
            case 0x66:      //O32
                print_string_static("O32\n");
                o32 = 1;
                v86tss.eip = (uint16_t)(v86tss.eip + 1);
                continue;
            
            case 0x67:      //A32
                print_string_static("A32\n");
                a32 = 1;
                v86tss.eip = (uint16_t)(v86tss.eip + 1);
                continue;
            
            case 0x9C:      //PUSHF
                print_string_static("PUSHF\n");
                if(o32)
                    v86_push32(v86tss.eflags);
                else
                    v86_push16(v86tss.eflags);
                v86tss.eip = (uint16_t)(v86tss.eip + 1);
                break;
            
            case 0x9D:      //POPF
                print_string_static("POPF\n");
                if(o32)
                    v86tss.eflags = (v86_pop32() & EFLAGS_REAL) | EFLAGS_VM;
                else
                    v86tss.eflags = (v86_pop16() & EFLAGS_REAL) | EFLAGS_VM;
                v86tss.eip = (uint16_t)(v86tss.eip + 1);
                break;
            
            case 0xCC:      //INT 3
            case 0xCD:      //INT x
                print_string_static("INT x\n");
                v86_push16(v86tss.eflags);
                v86_push16(v86tss.cs);
                v86_push16(v86tss.eip + (faultLoc[0] - 0xCB));
                v86tss.eip = *(uint16_t*)(faultLoc[0] == 0xCC ? 12 : faultLoc[1] * 4);
                v86tss.cs  = *(uint16_t*)(faultLoc[0] == 0xCC ? 14 : faultLoc[1] * 4 + 2);
                v86tss.eflags &= ~(EFLAGS_IF | EFLAGS_TF);
                break;
            
            case 0xCF:      //IRET
                print_string_static("IRET\n");
                v86tss.eip    = v86_pop16();
                v86tss.cs     = v86_pop16();
                v86tss.eflags = (v86_pop16() & EFLAGS_REAL) | EFLAGS_VM;
                break;
                
            case 0xE4:      //IN AL, imm8
                print_string_static("IN AL, imm8\n");
                v86tss.eax = (v86tss.eax & 0xFFFFFF00) | inb(faultLoc[1]);
                v86tss.eip = (uint16_t)(v86tss.eip + 2);
                break;
            
            case 0xE5:      //IN (E)AX, imm8
                print_string_static("IN (E)AX, imm8\n");
                if(o32)
                    v86tss.eax = ind(faultLoc[1]);
                else
                    v86tss.eax = inw(faultLoc[1]);
                v86tss.eip = (uint16_t)(v86tss.eip + 2);
                break;
            
            case 0xE6:      //OUT imm8, AL
                print_string_static("OUT imm8, AL\n");
                outb(faultLoc[1], (uint8_t)(v86tss.eax & 0xFF));
                v86tss.eip = (uint16_t)(v86tss.eip + 2);
                break;
            
            case 0xE7:      //OUT imm8, (E)AX
                print_string_static("OUT imm8, (E)AX\n");
                if(o32)
                    outd(faultLoc[1], v86tss.eax);
                else
                    outw(faultLoc[1], (uint16_t)(v86tss.eax & 0xFFFF));
                v86tss.eip = (uint16_t)(v86tss.eip + 2);
                break;
            
            case 0xEC:      //IN AL, DX
                print_string_static("IN AL, DX\n");
                v86tss.eax = (v86tss.eax & 0xFFFFFF00) | inb((uint16_t)v86tss.edx);
                v86tss.eip = (uint16_t)(v86tss.eip + 1);
                break;
            
            case 0xED:      //IN (E)AX, DX
                print_string_static("IN (E)AX, DX\n");
                if(o32)
                    v86tss.eax = ind((uint16_t)v86tss.edx);
                else
                    v86tss.eax = inw((uint16_t)v86tss.edx);
                v86tss.eip = (uint16_t)(v86tss.eip + 1);
                break;
            
            case 0xEE:      //OUT DX, AL
                print_string_static("OUT DX, AL\n");
                outb((uint16_t)v86tss.edx, (uint8_t)(v86tss.eax & 0xFF));
                v86tss.eip = (uint16_t)(v86tss.eip + 1);
                break;
            
            case 0xEF:      //OUT DX, (E)AX
                print_string_static("OUT DX, (E)AX\n");
                if(o32)
                    outd((uint16_t)v86tss.edx, v86tss.eax);
                else
                    outw((uint16_t)v86tss.edx, (uint16_t)(v86tss.eax & 0xFFFF));
                v86tss.eip = (uint16_t)(v86tss.eip + 1);
                break;
            
            default:
                print_string_static("Unknown fault opcode for v86: ");
                print_integer(*faultLoc, 16);
                print_string_static("\n");
            
            case 0xF4:      //HLT
                print_string_static("HLT\n");
                //we are done, reset GPF handler
                idt_set_gate(
                    13,
                    (uint32_t)isr13,
                    0x08,
                    I86_IDT_ATTR_PRESENT | I86_IDT_ATTR_32BIT_INT_GATE | I86_IDT_ATTR_PRIV_KERNEL
                );
                interrupts_enable();
                return;
            
            case 0xFA:      //CLI
                print_string_static("CLI\n");
                v86tss.eflags &= ~EFLAGS_IF;
                v86tss.eip = (uint16_t)(v86tss.eip + 1);
                break;
            
            case 0xFB:      //STI
                print_string_static("STI\n");
                v86tss.eflags &= ~EFLAGS_IF;
                v86tss.eip = (uint16_t)(v86tss.eip + 1);
                break;
        }
        
        print_string_static("Jumping back to v86 task\n");
        asm volatile ("iret");
        print_string_static("GPF occured\n");
        asm volatile ("pop %%eax" : "=a"(errorCode));
        o32 = a32 = 0;
    }
}

void v86_bioscall(uint8_t intNum, struct v86_biosArguments arguments, struct v86_biosArguments* results)
{
    memset(&v86tss, 0, sizeof(v86tss));
    
    v86tss.eflags = EFLAGS_VM;    //v86 mode enabled
    
    //set interrupt vector segment and ip
    v86tss.cs = *((uint16_t*)((intNum << 2) + 2));
    v86tss.eip = *(uint16_t*)(intNum << 2);
    
    print_string_static("Executing INT at 0x");
    print_integer(v86tss.cs, 16);
    print_string_static(":0x");
    print_integer(v86tss.eip, 16);
    print_string_static("\n");
    
    //setup realmode stack
    v86tss.ss = (uint16_t)(REALMODESTACK_END >> 4);
    v86tss.esp = REALMODESTACK_TOP;
    
    print_string_static("Realmode stack is at 0x");
    print_integer(v86tss.ss, 16);
    print_string_static(":0x");
    print_integer(v86tss.esp, 16);
    print_string_static("\n");
    
    v86tss.ds = arguments.ds;
    v86tss.es = arguments.es;
    
    v86tss.eax = arguments.eax;
    v86tss.ebx = arguments.ebx;
    v86tss.ecx = arguments.ecx;
    v86tss.edx = arguments.edx;
    v86tss.esi = arguments.esi;
    v86tss.edi = arguments.edi;
    v86tss.ebp = arguments.ebp;
    
    print_string_static("Realmode return point is at 0x");
    print_integer((uint16_t)((uint32_t)realmodeCode) >> 4, 16);
    print_string_static(":0x0\n");
    
    //setup return stackframe
    v86_push16((uint16_t)(v86tss.eflags & 0xFFFF));         //EFLAGS
    v86_push16((uint16_t)((uint32_t)realmodeCode) >> 4);    //CS
    v86_push16(0);                                          //IP
    
    v86_start_execution();
    
    if(results)
    {
        results->ds = v86tss.ds;
        results->es = v86tss.es;
        
        results->eax = v86tss.eax;
        results->ebx = v86tss.ebx;
        results->ecx = v86tss.ecx;
        results->edx = v86tss.edx;
        results->esi = v86tss.esi;
        results->edi = v86tss.edi;
        results->ebp = v86tss.ebp;
    }
}
