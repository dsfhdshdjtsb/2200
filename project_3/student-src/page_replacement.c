#include "types.h"
#include "pagesim.h"
#include "mmu.h"
#include "swapops.h"
#include "stats.h"
#include "util.h"

pfn_t select_victim_frame(void);

/**
 * --------------------------------- PROBLEM 7 --------------------------------------
 * Checkout PDF section 7 for this problem
 *
 * Make a free frame for the system to use. You call the select_victim_frame() method
 * to identify an "available" frame in the system (already given). You will need to
 * check to see if this frame is already mapped in, and if it is, you need to evict it.
 *
 * @return victim_pfn: a phycial frame number to a free frame be used by other functions.
 *
 * HINTS:
 *      - When evicting pages, remember what you checked for to trigger page faults
 *      in mem_access
 *      - If the page table entry has been written to before, you will need to use
 *      swap_write() to save the contents to the swap queue.
 * ----------------------------------------------------------------------------------
 */
pfn_t free_frame(void) {
    pfn_t victim_pfn;
    victim_pfn = select_victim_frame();

    // TODO: evict any mapped pages.

    if (frame_table[victim_pfn].mapped) {
        pcb_t* process = frame_table[victim_pfn].process;
        pte_t* entry = get_page_table_entry(frame_table[victim_pfn].vpn, process->saved_ptbr, mem);

        if (entry->dirty == 1) {
            swap_write(entry, mem + victim_pfn * PAGE_SIZE);
            entry->dirty = 0;
            stats.writebacks++;
        }        
        entry->valid = 0;
        entry->referenced = 0;

        frame_table[victim_pfn].mapped = 0;
        frame_table[victim_pfn].process = NULL;
        frame_table[victim_pfn].vpn = 0;
    }

    return victim_pfn;
}

/**
 * --------------------------------- PROBLEM 9 --------------------------------------
 * Checkout PDF section 7, 9, and 11 for this problem
 *
 * Finds a free physical frame. If none are available, uses either a
 * randomized, Approximate LRU, or FIFO algorithm to find a used frame for
 * eviction.
 *
 * @return The physical frame number of a victim frame.
 *
 * HINTS:
 *      - Use the global variables MEM_SIZE and PAGE_SIZE to calculate
 *      the number of entries in the frame table.
 *      - Use the global last_evicted to keep track of the pointer into the frame table
 * ----------------------------------------------------------------------------------
 */
pfn_t select_victim_frame() {
    /* See if there are any free frames first */
    size_t num_entries = MEM_SIZE / PAGE_SIZE;
    for (size_t i = 0; i < num_entries; i++) {
        if (!frame_table[i].protected && !frame_table[i].mapped)
        {
            return i;
        }
    }

    if (replacement == RANDOM) {
        /* Play Russian Roulette to decide which frame to evict */
        pfn_t unprotected_found = NUM_FRAMES;
        for (pfn_t i = 0; i < num_entries; i++) {
            if (!frame_table[i].protected) {
                unprotected_found = i;
                if (prng_rand() % 2) {
                    return i;
                }
            }
        }
        /* If no victim found yet take the last unprotected frame
           seen */
        if (unprotected_found < NUM_FRAMES) {
            return unprotected_found;
        }
    }
    else if (replacement == APPROX_LRU) {
        pfn_t victim = 0;
        uint8_t min_refs = 0xFF;

        // pfn_t start = last_evicted;
        // do {
        //     last_evicted = (last_evicted + 1) % num_entries;
        //     if(!frame_table[last_evicted].protected) {
        //         if (frame_table[last_evicted].ref_count < min_refs) {
        //             min_refs = frame_table[last_evicted].ref_count;
        //             victim = last_evicted;
        //         }
        //     }
        // } while (last_evicted != start);

        for (pfn_t i = 0; i < num_entries; i++) {
            if(!frame_table[i].protected) {
                if (frame_table[i].ref_count < min_refs) {
                    min_refs = frame_table[i].ref_count;
                    victim = i;
                }
            }
        }
        return victim;
    }
    else if (replacement == FIFO) {
        pfn_t start = last_evicted;
        do {
            last_evicted = (last_evicted + 1) % num_entries;
            if (!frame_table[last_evicted].protected) {
                return last_evicted;
            }
        } while (last_evicted != start);
    }

    // If every frame is protected, give up. This should never happen on the traces we provide you.
    panic("System ran out of memory\n");
    exit(1);
}
/**
 * --------------------------------- PROBLEM 10.2 --------------------------------------
 * Checkout PDF for this problem
 *
 * Updates the associated variables for the Approximate LRU,
 * called every time the simulator daemon wakes up.
 *
 * ----------------------------------------------------------------------------------
 */
void daemon_update(void)
{
    /** FIX ME */
    size_t num_entries = MEM_SIZE / PAGE_SIZE;
    for (pfn_t i = 0; i <num_entries; i++) {
        if (frame_table[i].protected) {
            continue;
        }
        frame_table[i].ref_count >>= 1;
        if (frame_table[i].mapped == 1) {
            pfn_t sptbr = frame_table[i].process->saved_ptbr;
            vpn_t vpn = frame_table[i].vpn;
            pte_t* pte = get_page_table_entry(vpn, sptbr, mem);
            if (pte->referenced == 1) {
                frame_table[i].ref_count |= 0x80;
            }
            pte->referenced = 0;
        }
    }
    
}
