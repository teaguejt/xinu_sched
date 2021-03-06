/* clkhandler.c - clkhandler */

#include <xinu.h>

/*-----------------------------------------------------------------------
 * clkhandler - high level clock interrupt handler
 *-----------------------------------------------------------------------
 */
void	clkhandler()
{

	static uint32 count1000 = 1000;	/* variable to count 1000ms */
	volatile struct am335x_timer1ms *csrptr = 0x44E31000;
					/* Pointer to timer CSR	    */
    struct procent *prptr;      /* jteague6: pointer to current process */

	/* If there is no interrupt, return */

	if((csrptr->tisr & AM335X_TIMER1MS_TISR_OVF_IT_FLAG) == 0) {
		return;
	}

	/* Acknowledge the interrupt */

	csrptr->tisr = AM335X_TIMER1MS_TISR_OVF_IT_FLAG;

	/* Decrement 1000ms counter */

	count1000--;

	/* After 1 sec, increment clktime */

	if(count1000 == 0) {
		clktime++;
		count1000 = 1000;
	}

	/* check if sleep queue is empty */

	if(!isempty(sleepq)) {

		/* sleepq nonempty, decrement the key of */
		/* topmost process on sleepq		 */

		if((--queuetab[firstid(sleepq)].qkey) == 0) {

			wakeup();
		}
	}

	/* Decrement the preemption counter */
	/* Reschedule if necessary	    */

	if((--preempt) == 0) {
#if 0
        /* jteague6 - SJF scheduler: If the current process is not the null
         * process and its priority is not 1, decrement its priority to show
         * that it is a longer-running process. */
        prptr = &proctab[currpid];
        if( currpid != 0 && prptr->prprio > 1 ) {
            --prptr->prprio;
        }
#endif
#if 1
        /* jteague6 - Random scheduler: use the process' current priority as
         * a seed for srand and get a new random number for its priority before
         * calling resched() */
        prptr = &proctab[currpid];
        if( currpid != 0 ) {
            srand( (uint32)prptr->prprio );
            prptr->prprio = rand();
        }
#endif
		preempt = QUANTUM;
		resched();
	}
}
