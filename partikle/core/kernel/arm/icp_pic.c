/*
 * $FILE: i8259.c
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

#include <config.h>
#include <bitop.h>
#include <irqs.h>
#include <arch/board.h>
#include <arch/io.h>

#define ICP_PIC_IRQS 32

#define PIC_IRQ_STAT (PIC_BASE_ADDR + 0x0)
#define PIC_IRQ_RAWSTAT  (PIC_BASE_ADDR +0x4)
#define PIC_IRQ_ENSET (PIC_BASE_ADDR + 0x8)
#define PIC_IRQ_ENCLR (PIC_BASE_ADDR + 0xC)

#define PIC_SOFT_INTSET (PIC_BASE_ADDR + 0x10)
#define PIC_SOFT_INTCLR (PIC_BASE_ADDR + 0x14)

#define PIC_FIQ_STAT (PIC_BASE_ADDR + 0x20)
#define PIC_FIQ_RAWSTAT (PIC_BASE_ADDR + 0x24)
#define PIC_FIQ_ENSET (PIC_BASE_ADDR + 0x28)
#define PIC_FIQ_ENCLR (PIC_BASE_ADDR + 0x2c)

//--------------------//
// icp_pic_enable_irq //
//--------------------//

static void icp_pic_enable_irq (unsigned int irq) {
  io_write (PIC_IRQ_ENSET, (1<<irq));
}

//---------------------//
// icp_pic_disable_irq //
//---------------------//

static void icp_pic_disable_irq (unsigned int irq) {
  io_write (PIC_IRQ_ENCLR, (1<<irq));
}

//--------------//
// pic_get_eirq //
//--------------//

unsigned int pic_get_eirq (void) {
  int eirq = _ffs(io_read (PIC_IRQ_STAT));

  return (eirq!=-1)?eirq:0;
}


//--------------------------//
// icp_pic_mask_and_ack_irq //
//--------------------------//

/*static void icp_pic_mask_and_ack_irq (unsigned int irq) {
  }*/

void init_pic (void) {
  int irq;
  
  io_write (PIC_IRQ_ENCLR, ~0);

  for (irq = 0; irq < ICP_PIC_IRQS; irq ++) {
    hw_irq_ctrl [irq] = (hw_irq_ctrl_t){
      .enable = icp_pic_enable_irq,
      .disable = icp_pic_disable_irq,
      .ack = icp_pic_disable_irq,
      .end = icp_pic_enable_irq,
    };
  }
}
