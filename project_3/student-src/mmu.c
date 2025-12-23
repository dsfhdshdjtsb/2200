#include "mmu.h"
#include "pagesim.h"
#include "address_splitting.h"
#include "swapops.h"
#include "stats.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

/* The frame table pointer. You will set this up in system_init. */
fte_t *frame_table;

/**
 * --------------------------------- PROBLEM 2 --------------------------------------
 * Checkout PDF sections 4 for this problem
 * 
 * In this problem, you will initialize the frame_table pointer. The frame table will
 * be located at physical address 0 in our simulated memory. You should zero out the 
 * entries in the frame table, in case for any reason physical memory is not clean.
 * 
 * HINTS:
 *      - mem: Simulated physical memory already allocated for you.
 *      - PAGE_SIZE: The size of one page
 * ----------------------------------------------------------------------------------
 */
void system_init(void) {
    // TODO: initialize the frame_table pointer.
    frame_table = (fte_t *) &mem[0];
    memset(frame_table, 0, sizeof(fte_t) * NUM_FRAMES);
    frame_table->protected = 1;
}


/**
 * --------------------------------- PROBLEM 5 --------------------------------------
 * Checkout PDF section 6 for this problem
 * 
 * Takes an input virtual address and performs a memory operation.
 * 
 * @param addr virtual address to be translated
 * @param access 'r' if the access is a read, 'w' if a write
 * @param data If the access is a write, one byte of data to write to our memory.
 *             Otherwise NULL for read accesses.
 * 
 * HINTS:
 *      - Remember that not all the entry in the process's page table are mapped in. 
 *      Check what in the pte_t struct signals that the entry is mapped in memory.
 * ----------------------------------------------------------------------------------
 */
uint8_t mem_access(vaddr_t address, char access, uint8_t data) {
    // TODO: translate virtual address to physical, then perform the specified operation
    stats.accesses++;
    vpn_t vpn = get_vaddr_vpn(address);
    uint16_t offset = get_vaddr_offset(address);
    pte_t *entry = get_page_table_entry(vpn, PTBR, mem );

    if (entry->valid == 0) {
        stats.page_faults++;
        page_fault(address);
    }

    paddr_t paddr = get_physical_address(entry->pfn, offset);

    entry-> referenced = 1;

    //frame_table[entry->pfn].ref_count |= 0x80;

    // Either read or write the data to the physical address depending on 'rw'
    if (access == 'r') {
        return mem[paddr];
    } else {
        entry->dirty = 1;
        mem[paddr] = data;
    }

    return 0;
}
