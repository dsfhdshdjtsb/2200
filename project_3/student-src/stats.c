#include "stats.h"

/* The stats. See the definition in stats.h. */
stats_t stats;

/**
 * --------------------------------- PROBLEM 10 --------------------------------------
 * Checkout PDF section 10 for this problem
 * 
 * Calulate the total average time it takes for an access
 * 
 * HINTS:
 * 		- You may find the #defines in the stats.h file useful.
 * 		- You will need to include code to increment many of these stats in
 * 		the functions you have written for other parts of the project.
 * -----------------------------------------------------------------------------------
 */
void compute_stats() {

	if (stats.accesses == 0) {
		stats.amat = 0.0;
		return;
	}

	double base = MEMORY_ACCESS_TIME;
	double pf_component = ((double)stats.page_faults / (double)stats.accesses) * DISK_PAGE_READ_TIME;
	double wb_component = ((double)stats.writebacks / (double)stats.accesses) * DISK_PAGE_WRITE_TIME;
	stats.amat = base + pf_component + wb_component;
}
