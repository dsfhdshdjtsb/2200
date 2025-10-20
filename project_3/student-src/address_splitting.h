#pragma once

#include "mmu.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

/**
 * --------------------------------- PROBLEM 1 --------------------------------------
 * Checkout PDF Section 3 For this Problem
 *
 * Split the virtual address into its virtual page number and offset.
 * 
 * HINT: 
 *      -Examine the global defines in pagesim.h, which will be necessary in 
 *      implementing these functions.
 * ----------------------------------------------------------------------------------
 */

static inline vpn_t get_vaddr_vpn(vaddr_t addr) {
    return (addr >> OFFSET_LEN);
}

static inline uint16_t get_vaddr_offset(vaddr_t addr) {
    return addr & ((1 << OFFSET_LEN) - 1);
}

static inline pte_t* get_page_table(pfn_t ptbr, uint8_t *memory) {
    // base of page table is physical frame number (ptbr) multiplied by page size
    return (pte_t*)(memory + ((paddr_t)ptbr * PAGE_SIZE));
}

static inline pte_t* get_page_table_entry(vpn_t vpn, pfn_t ptbr, uint8_t *memory) {
    pte_t* pt = get_page_table(ptbr, memory);
    return &pt[vpn];
}

static inline paddr_t get_physical_address(pfn_t pfn, uint16_t offset) {
    return pfn * PAGE_SIZE + offset;
}

#pragma GCC diagnostic pop
