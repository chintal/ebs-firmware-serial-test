
#include<ds/bytebuf.h>
#include<platform/cpu.h>

#include "application.h"
#include "hal/uc.h"
#include "sys/sys.h"

#include "application.h"
#include "application_descriptors.h"
#include "serial_test.h"


volatile uint8_t rval=0;

static void deferred_exec(void);

static void deferred_exec(void){
    serial_test_reactor();
}


static void _initialize_interrupts(void);

static void _initialize_interrupts(void){
    // Needed to ensure linker includes interrupt handlers 
    // in the output.
    #ifdef uC_INCLUDE_UART_IFACE
    __uart_handler_inclusion = 1;
    #endif
    
    #ifdef uC_INCLUDE_USB_IFACE
    __usb_handler_inclusion = 1;
    #endif
}

int main(void)
{   
    // Pre-init, and assoicated Housekeeping
    _initialize_interrupts();
    
    // uC Core Initialization
    watchdog_hold();
    power_set_full();
    clock_set_default();
    global_interrupt_enable();
    
    #ifdef uC_INCLUDE_USB_IFACE
    usb_init();
    #endif
    id_init();
    application_descriptors_init();
    
    serial_test_init();

    __delay_cycles(10000000);
    
    while(1){
        deferred_exec();
    }
    return(0);
}
