#include "mmu.h"
#include "pagesim.h"
#include "swapops.h"
#include "stats.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

/**
 * --------------------------------- PROBLEM 6 --------------------------------------
 * Checkout PDF section 7 for this problem
 * 
 * Page fault handler.
 * 
 * When the CPU encounters an invalid address mapping in a page table, it invokes the 
 * OS via this handler. Your job is to put a mapping in place so that the translation 
 * can succeed.
 * 
 * @param addr virtual address in the page that needs to be mapped into main memory.
 * 
 * HINTS:
 *      - You will need to use the global variable current_process when
 *      altering the frame table entry.
 *      - Use swap_exists() and swap_read() to update the data in the 
 *      frame as it is mapped in.
 * ----------------------------------------------------------------------------------
 */
void page_fault(vaddr_t address) {
    // TODO: Get a new frame, then correctly update the page table and frame table
    vpn_t vpn = get_vaddr_vpn(address);
    //uint16_t offset = get_vaddr_offset(address);

    pte_t *entry = get_page_table_entry(vpn, PTBR, mem );

    pfn_t npfn = free_frame();
    if (swap_exists(entry)) {
        swap_read(entry, mem + npfn * PAGE_SIZE);
    } else {
        memset(mem + npfn * PAGE_SIZE, 0, PAGE_SIZE);
    }

    frame_table[npfn].mapped = 1;
    frame_table[npfn].process = current_process;
    //frame_table[npfn].ref_count = 0;
    frame_table[npfn].vpn = vpn;
    frame_table[npfn].protected = 0;

    entry->pfn = npfn;
    entry->referenced = 0; // will be set to 1 by function call
    entry->dirty = 0;
    entry->valid = 1;
}

#pragma GCC diagnostic pop
