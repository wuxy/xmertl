/*
 * $FILE: pic.c
 *
 * The PC's PIC
 *
 * $VERSION: 1.0
 *
 * Author: Miguel Masmano <mimastel@doctor.upv.es>
 *
 * $LICENSE:  
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 */

#include <irqs.h>
#include <arch/io.h>
#include <arch/pic.h>

#define CTLR2_IRQ 0x2

#define master_pic_iobase 0x20
#define slave_pic_iobase 0xa0
#define off_icw 0
#define off_ocw 1

#define master_icw master_pic_iobase + off_icw
#define master_ocw master_pic_iobase + off_ocw
#define slave_icw slave_pic_iobase + off_icw
#define slave_ocw slave_pic_iobase + off_ocw

#define LAST_IRQ_IN_MASTER 7

#define ICW_TEMPLATE 0x10
#define EDGE_TRIGGER 0x00
#define ADDR_INTRVL8 0x00
#define CASCADE_MODE 0x00
#define ICW4_NEEDED 0x01

#define SLAVE_ON_IR2 0x04
#define I_AM_SLAVE_2 0x02

#define SNF_MODE_DIS 0x00
#define NONBUFD_MODE 0x00
#define AUTO_EOI_MOD 0x02
#define NRML_EOI_MOD 0x00
#define I8086_EMM_MOD 0x01

#define PICM_ICW1 (ICW_TEMPLATE | EDGE_TRIGGER | ADDR_INTRVL8 | \
  CASCADE_MODE | ICW4_NEEDED)
#define PICM_ICW3 SLAVE_ON_IR2
#define PICM_ICW4 (SNF_MODE_DIS | NONBUFD_MODE | NRML_EOI_MOD | I8086_EMM_MOD)

#define PICS_ICW1 (ICW_TEMPLATE | EDGE_TRIGGER | ADDR_INTRVL8 | \
  CASCADE_MODE | ICW4_NEEDED)
#define PICS_ICW3 I_AM_SLAVE_2
#define PICS_ICW4 (SNF_MODE_DIS | NONBUFD_MODE | NRML_EOI_MOD | I8086_EMM_MOD)

#define NON_SPEC_EOI 0x20

static unsigned char c_master_mask = 0xFF, c_slave_mask = 0xFF;

static void pic_enable_irq (unsigned int irq) {
  if (irq <= LAST_IRQ_IN_MASTER) {
    c_master_mask = c_master_mask & ~(1 << irq);
    outb_p (c_master_mask, master_ocw);
  } else {
    c_master_mask = c_master_mask & ~(1 << CTLR2_IRQ);
    c_slave_mask = c_slave_mask & ~(1 << (irq - LAST_IRQ_IN_MASTER - 1));
    outb_p (c_master_mask, master_ocw);
    outb_p (c_slave_mask, slave_ocw);
  }
}

static void pic_disable_irq (unsigned int irq) {
  if (irq <= LAST_IRQ_IN_MASTER) {
    c_master_mask = c_master_mask | (1 << irq);
    outb_p (c_master_mask, master_ocw);
  } else {
    c_slave_mask = c_slave_mask | (1 << (irq - LAST_IRQ_IN_MASTER - 1));
    if (!c_slave_mask) {
      c_master_mask = c_master_mask | (1 << CTLR2_IRQ);
      outb_p (c_master_mask, master_ocw);
    }
    outb_p (c_slave_mask, slave_ocw);
  }
}

static void pic_mask_and_ack_irq (unsigned int irq) {
  if (irq <= LAST_IRQ_IN_MASTER) {
    c_master_mask = c_master_mask | (1 << irq);
    outb_p (c_master_mask, master_ocw);
    outb_p (NON_SPEC_EOI, master_icw);
  } else {
    c_slave_mask = c_slave_mask | (1 << (irq - LAST_IRQ_IN_MASTER - 1));
    if (!c_slave_mask) {
      c_master_mask = c_master_mask | (1 << CTLR2_IRQ);
      outb_p (c_master_mask, master_ocw);
    }
    outb_p (c_slave_mask, slave_ocw);
    outb_p (NON_SPEC_EOI, slave_icw);
    outb_p (NON_SPEC_EOI, master_icw);
  }
}

// initialise both PICs

void init_pic (unsigned char master_base, unsigned char slave_base) {
  int irq;

  // initialise the master
  outb_p (PICM_ICW1, master_icw);
  outb_p (master_base, master_ocw);
  outb_p (PICM_ICW3, master_ocw);
  outb_p (PICM_ICW4, master_ocw);

  // initialise the slave
  outb_p (PICS_ICW1, slave_icw);
  outb_p (slave_base, slave_ocw);
  outb_p (PICS_ICW3, slave_ocw);
  outb_p (PICS_ICW4, slave_ocw);
  
  // ack any bogus intrs
  outb_p (NON_SPEC_EOI, master_icw);
  outb_p (NON_SPEC_EOI, slave_icw);
  
  // disable all the PIC's IRQ lines
  outb_p (c_master_mask, master_ocw);
  outb_p (c_slave_mask, slave_ocw);
  
  
  for (irq = 0; irq < PIC_IRQS; irq ++) {
    hw_irq_ctrl [irq] = (hw_irq_ctrl_t){
      .enable = pic_enable_irq,
      .disable = pic_disable_irq,
      .ack = pic_mask_and_ack_irq,
      .end = pic_enable_irq
    };
  }
}
