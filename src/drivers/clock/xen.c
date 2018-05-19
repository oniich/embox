/**
 * @file
 *
 * @data
 * @author
 */

#include <hal/clock.h>
#include <kernel/time/clock_source.h>
#include <kernel/irq.h>
#include <kernel/panic.h>
#include <xen/event_channel.h>
#include <xen_hypercall-x86_32.h>

#include <embox/unit.h>

#include <math.h>
#include <time.h>
#include <xen/xen.h>

EMBOX_UNIT_INIT(xen_clock_init);

extern shared_info_t xen_shared_info;
#define NANOSECONDS(tsc) (tsc << xen_shared_info.vcpu_info[0].time.tsc_shift) * xen_shared_info.vcpu_info[0].time.tsc_to_system_mul
#define RDTSC(x)     asm volatile ("RDTSC":"=A"(tsc))

static int integratorcp_clock_setup(struct time_dev_conf * conf) {
	return 0;
}

static struct time_event_device xen_event_device = {
	.name = "xen event device",
	.config = integratorcp_clock_setup,
	.event_hz = 1000,
	.irq_nr = -1
};

static cycle_t xen_tcd_read(void) {
	return 0;
}

static struct time_counter_device xen_tcd = {
	.cycle_hz = 1000,
	.read = xen_tcd_read,
};

static struct clock_source xen_cs = {
	.name = "xen clock source",
	.event_device = &xen_event_device,
	.counter_device = &xen_tcd,
	.read = clock_source_read /* attach default read function */
};

uint64_t mygettimeofday()
{
	uint64_t tsc;
	uint32_t version, wc_version;
	uint32_t seconds, nanoseconds, system_time;
	uint64_t old_tsc;
	do
	{
		do
		{
			wc_version = xen_shared_info.wc_version;
			version = xen_shared_info.vcpu_info[0].time.version;
		} while((version & 1) == 1 || (wc_version & 1) == 1);
		seconds = xen_shared_info.wc_sec;
		nanoseconds = xen_shared_info.wc_nsec;
		system_time = xen_shared_info.vcpu_info[0].time.system_time;
		old_tsc = xen_shared_info.vcpu_info[0].time.tsc_timestamp;
	} while(version != xen_shared_info.vcpu_info[0].time.version || wc_version != xen_shared_info.wc_version);
	RDTSC(tsc);
	tsc -= old_tsc;
	system_time += NANOSECONDS(tsc);
	nanoseconds += system_time;
	seconds += nanoseconds / 1000000000;
	nanoseconds = nanoseconds % 1000000000;
	return nanoseconds * 1000;
}

int handler_calls;
int n = 10;
uint64_t system_time;

static irq_return_t clock_handler(unsigned int irq_nr, void *dev_id) {
	handler_calls++;
	uint64_t time = xen_shared_info.vcpu_info[0].time.system_time;
	if (system_time != time)
	{
		uint64_t diff = (time - system_time) / 1000000;
		uint64_t virt_diff = handler_calls * 10;
		float mult = (float)(diff) / virt_diff;
		n = round(10 * mult);
		system_time = time;
		handler_calls = 0;
	}
	for (int i = 0; i < n; i++) {
		clock_tick_handler(irq_nr, dev_id);
	}
	return IRQ_HANDLED;
}


static int xen_clock_init(void) {

	clock_source_register(&xen_cs);
	evtchn_bind_virq_t op;

	op.virq = VIRQ_TIMER;
	op.vcpu = 0;
	if (HYPERVISOR_event_channel_op(EVTCHNOP_bind_virq, &op) != 0) {
		panic("Error has happened during timer initialization.\n");
	}
	xen_event_device.irq_nr = op.port;

	handler_calls = -1;
	system_time = xen_shared_info.vcpu_info[0].time.system_time;
	while (system_time == xen_shared_info.vcpu_info[0].time.system_time)
	{
	}
	system_time = xen_shared_info.vcpu_info[0].time.system_time;

	return irq_attach(op.port, clock_handler, 0, &xen_cs, "xen clock irq");
}
