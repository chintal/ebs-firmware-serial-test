

#include "serial_test.h"
#include <prbs/prbs.h>
#include "sys/bciface.h"
#include "drivers/led/led.h"

lfsr16_t prbs;
const char test_str[] = "0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ";
uint8_t tbuffer[20];

#define SERIAL_TEST_PREINIT             0x00
#define SERIAL_TEST_THROUGHPUT          'a'
#define SERIAL_TEST_BER_PRBS            'b'
#define SERIAL_TEST_THROUGHPUT_RAW      'c'
#define SERIAL_TEST_ROUNDTRIP           'd'
#define SERIAL_TEST_ROUNDTRIP_CHUNKED   'e'

#define STE_CHUNK_SIZE 5

uint8_t serial_test_state;
uint8_t serial_test_persistence;


void serial_test_init(void)
{
    bc_init();
    serial_test_state = SERIAL_TEST_PREINIT;
    
    led_init(BOARD_RED_LED_SELECTOR);
    led_init(BOARD_GREEN_LED_SELECTOR);

    led_on(BOARD_RED_LED_SELECTOR);
    led_off(BOARD_GREEN_LED_SELECTOR);
}

static void _serial_test_select(void){
    if (!(bc_unhandled_rxb())){
        return;
    }
    serial_test_state = bc_getc();
    bc_discard_rxb();
    if(bc_reqlock(1, BYTEBUF_TOKEN_SCHAR)){
        bc_putc(serial_test_state, BYTEBUF_TOKEN_SCHAR, 0);
    }   
    
    led_off(BOARD_RED_LED_SELECTOR);
    led_on(BOARD_GREEN_LED_SELECTOR);
    
    // Special Intialization
    switch(serial_test_state){
        case SERIAL_TEST_BER_PRBS:
            lfsr16_vInit(&prbs);
            break;
        case SERIAL_TEST_THROUGHPUT_RAW:
            serial_test_persistence = '0';
        default:
            break;
    }
    return;
}

static void _serial_test_throughput(void){
    // Throughput test. Only byte transmission rate is important. 
    // Error rate is in principle measurable, and easily so. This test is
    // intended to stress test the interface without other firmware 
    // bottlenecks, given the environment within which the firmware is 
    // expected to run (including the buffers, drivers, and USB stack).
    if(bc_reqlock(43, BYTEBUF_TOKEN_SCHAR)){
        bc_write((void *)(&test_str[0]), 43, BYTEBUF_TOKEN_SCHAR);
    }
}

static void _serial_test_ber_prbs(void){
    // PRBS BER test. This is not a very useful test, since generation 
    // of the PRBS seems to be the bottleneck. However, the general idea is 
    // to ensure glitch-free transmission under real-ish data loads. 
    // It may be noted that in real use cases, the need to generate the 
    // data may present similar if not narrower bottlenecks.
    if(bc_reqlock(1, BYTEBUF_TOKEN_SCHAR)){
        bc_putc(lfsr16_cGetNextByte(&prbs), BYTEBUF_TOKEN_SCHAR, 0);
    }
}

static void _serial_test_throughput_raw(void){
    // Raw Throughput test. Actual physical interface capability (along 
    // with USB stack only, if CDC). No buffering, locking, etc. Whenever 
    // it is possible to send some data, just fill the interface buffer 
    // and send it along.
    //
    // TODO Work out non-MSP430 USBCDC variants and switching
    #if BOARD_BCIFACE_TYPE == BCI_USBCDC
        while (!(CdcWriteCtrl[BOARD_BCIFACE_INTFNUM].sCurrentInBuffer->bAvailable));
        usbcdc_putc(BOARD_BCIFACE_INTFNUM, serial_test_persistence, 0x01, 0x00);
    #elif BOARD_BCIFACE_TYPE == BCI_UART
        uart_putc_bare(BOARD_BCIFACE_INTFNUM, serial_test_persistence);
    #endif
    if (serial_test_persistence != 'Z'){
        serial_test_persistence ++;
    }
    else{
        serial_test_persistence = '0';
    }
}

static void _serial_test_roundtrip(void){
    // Round Trip test. Ensure glitch-free round trip transmission under 
    // real-ish data loads using a simple per-byte echo like approach to
    // return all data recieved as is.
    //
    // NOTE This test blocks on full transmit buffer. It can be rewritten 
    //      to be a more involved state machine on serial_test_persistence.
    
    uint8_t c;
    if (bc_unhandled_rxb()){
        c = bc_getc();
    }
    else{
        return;
    }
    while (!bc_reqlock(1, BYTEBUF_TOKEN_SCHAR));
    bc_putc(c, BYTEBUF_TOKEN_SCHAR, 0);
}

static void _serial_test_roundtrip_chunked(void){
    // Chunked Round Trip test. Ensure glitch-free round trip transmission 
    // under real-ish data loads using the interface's chunked RX/TX API.
    // Return all data recieved as is. Note that for this test to work with 
    // the current naive serial-test implementation, length of each test 
    // vector should be an integer multiple of the chunk size.
    
    // NOTE This test will block on full transmit buffer. It can be rewritten 
    //      to be a more involved state machine on serial_test_persistence.

    if (bc_unhandled_rxb() >= STE_CHUNK_SIZE){
        bc_read( (void*)&tbuffer, STE_CHUNK_SIZE );
    }
    else{
        return;
    }
    
    while (!bc_reqlock( STE_CHUNK_SIZE, BYTEBUF_TOKEN_SCHAR ));
    bc_write( (void*)&tbuffer, STE_CHUNK_SIZE, BYTEBUF_TOKEN_SCHAR );
}

void serial_test_reactor(void){
    switch (serial_test_state){
        case SERIAL_TEST_PREINIT:
            _serial_test_select();
            break;
        case SERIAL_TEST_THROUGHPUT:
            _serial_test_throughput();
            break;
        case SERIAL_TEST_BER_PRBS:
            _serial_test_ber_prbs();
            break;
        case SERIAL_TEST_THROUGHPUT_RAW:
            _serial_test_throughput_raw();
            break;
        case SERIAL_TEST_ROUNDTRIP:
            _serial_test_roundtrip();
            break;
        case SERIAL_TEST_ROUNDTRIP_CHUNKED:
            _serial_test_roundtrip_chunked();
            break;
        default:
            serial_test_state = SERIAL_TEST_PREINIT;
            break;
    }
}
