/**
 * isr.h
 * Criado por Matheus Leme Da Silva
 */
#ifndef ISR_H
#define ISR_H
#include "kdefs.h"

extern void isr_0(void);
extern void isr_1(void);
extern void isr_2(void);
extern void isr_3(void);
extern void isr_4(void);
extern void isr_5(void);
extern void isr_6(void);
extern void isr_7(void);
extern void isr_8(void);
extern void isr_9(void);
extern void isr_10(void);
extern void isr_11(void);
extern void isr_12(void);
extern void isr_13(void);
extern void isr_14(void);
extern void isr_15(void);
extern void isr_16(void);
extern void isr_17(void);
extern void isr_18(void);
extern void isr_19(void);
extern void isr_20(void);
extern void isr_21(void);
extern void isr_22(void);
extern void isr_23(void);
extern void isr_24(void);
extern void isr_25(void);
extern void isr_26(void);
extern void isr_27(void);
extern void isr_28(void);
extern void isr_29(void);
extern void isr_30(void);
extern void isr_31(void);
extern void isr_32(void);
extern void isr_33(void);
extern void isr_34(void);
extern void isr_35(void);
extern void isr_36(void);
extern void isr_37(void);
extern void isr_38(void);
extern void isr_39(void);
extern void isr_40(void);
extern void isr_41(void);
extern void isr_42(void);
extern void isr_43(void);
extern void isr_44(void);
extern void isr_45(void);
extern void isr_46(void);
extern void isr_47(void);

void isr_init(void) {
  idt_set_gate(0, isr_0, KERNEL_CODE_SELECTOR, 0x8e);
  idt_set_gate(1, isr_1, KERNEL_CODE_SELECTOR, 0x8e);
  idt_set_gate(2, isr_2, KERNEL_CODE_SELECTOR, 0x8e);
  idt_set_gate(3, isr_3, KERNEL_CODE_SELECTOR, 0x8e);
  idt_set_gate(4, isr_4, KERNEL_CODE_SELECTOR, 0x8e);
  idt_set_gate(5, isr_5, KERNEL_CODE_SELECTOR, 0x8e);
  idt_set_gate(6, isr_6, KERNEL_CODE_SELECTOR, 0x8e);
  idt_set_gate(7, isr_7, KERNEL_CODE_SELECTOR, 0x8e);
  idt_set_gate(8, isr_8, KERNEL_CODE_SELECTOR, 0x8e);
  idt_set_gate(9, isr_9, KERNEL_CODE_SELECTOR, 0x8e);
  idt_set_gate(10, isr_10, KERNEL_CODE_SELECTOR, 0x8e);
  idt_set_gate(11, isr_11, KERNEL_CODE_SELECTOR, 0x8e);
  idt_set_gate(12, isr_12, KERNEL_CODE_SELECTOR, 0x8e);
  idt_set_gate(13, isr_13, KERNEL_CODE_SELECTOR, 0x8e);
  idt_set_gate(14, isr_14, KERNEL_CODE_SELECTOR, 0x8e);
  idt_set_gate(15, isr_15, KERNEL_CODE_SELECTOR, 0x8e);
  idt_set_gate(16, isr_16, KERNEL_CODE_SELECTOR, 0x8e);
  idt_set_gate(17, isr_17, KERNEL_CODE_SELECTOR, 0x8e);
  idt_set_gate(18, isr_18, KERNEL_CODE_SELECTOR, 0x8e);
  idt_set_gate(19, isr_19, KERNEL_CODE_SELECTOR, 0x8e);
  idt_set_gate(20, isr_20, KERNEL_CODE_SELECTOR, 0x8e);
  idt_set_gate(21, isr_21, KERNEL_CODE_SELECTOR, 0x8e);
  idt_set_gate(22, isr_22, KERNEL_CODE_SELECTOR, 0x8e);
  idt_set_gate(23, isr_23, KERNEL_CODE_SELECTOR, 0x8e);
  idt_set_gate(24, isr_24, KERNEL_CODE_SELECTOR, 0x8e);
  idt_set_gate(25, isr_25, KERNEL_CODE_SELECTOR, 0x8e);
  idt_set_gate(26, isr_26, KERNEL_CODE_SELECTOR, 0x8e);
  idt_set_gate(27, isr_27, KERNEL_CODE_SELECTOR, 0x8e);
  idt_set_gate(28, isr_28, KERNEL_CODE_SELECTOR, 0x8e);
  idt_set_gate(29, isr_29, KERNEL_CODE_SELECTOR, 0x8e);
  idt_set_gate(30, isr_30, KERNEL_CODE_SELECTOR, 0x8e);
  idt_set_gate(31, isr_31, KERNEL_CODE_SELECTOR, 0x8e);
  idt_set_gate(32, isr_32, KERNEL_CODE_SELECTOR, 0x8e);
  idt_set_gate(33, isr_33, KERNEL_CODE_SELECTOR, 0x8e);
  idt_set_gate(34, isr_34, KERNEL_CODE_SELECTOR, 0x8e);
  idt_set_gate(35, isr_35, KERNEL_CODE_SELECTOR, 0x8e);
  idt_set_gate(36, isr_36, KERNEL_CODE_SELECTOR, 0x8e);
  idt_set_gate(37, isr_37, KERNEL_CODE_SELECTOR, 0x8e);
  idt_set_gate(38, isr_38, KERNEL_CODE_SELECTOR, 0x8e);
  idt_set_gate(39, isr_39, KERNEL_CODE_SELECTOR, 0x8e);
  idt_set_gate(40, isr_40, KERNEL_CODE_SELECTOR, 0x8e);
  idt_set_gate(41, isr_41, KERNEL_CODE_SELECTOR, 0x8e);
  idt_set_gate(42, isr_42, KERNEL_CODE_SELECTOR, 0x8e);
  idt_set_gate(43, isr_43, KERNEL_CODE_SELECTOR, 0x8e);
  idt_set_gate(44, isr_44, KERNEL_CODE_SELECTOR, 0x8e);
  idt_set_gate(45, isr_45, KERNEL_CODE_SELECTOR, 0x8e);
  idt_set_gate(46, isr_46, KERNEL_CODE_SELECTOR, 0x8e);
  idt_set_gate(47, isr_47, KERNEL_CODE_SELECTOR, 0x8e);
}

#endif // ISR_H
