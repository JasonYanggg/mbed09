#include "mbed.h"
#include "mbed_rpc.h"

/**
 * This example program has been updated to use the RPC implementation in the new mbed libraries
 * This example demonstrates using RPC over serial
 */

// Use the RPC enabled wrapped class  -see RpcClasses.h for more info
Thread t1;
Thread t3;
EventQueue q1;
EventQueue q3;

RpcDigitalOut myled1(LED1, "myled1");
RpcDigitalOut myled3(LED3, "myled3");
Serial pc(USBTX, USBRX);
void LEDControl(Arguments *in, Reply *out);
RPCFunction rpcLED(&LEDControl, "LEDControl");
void blink_led1(double x);
void blink_led3(double x);

int main()
{
    char buf[256], outbuf[256];

    t1.start(callback(&q1, &EventQueue::dispatch_forever));
    t3.start(callback(&q3, &EventQueue::dispatch_forever));
    while (true) {
        memset(buf, 0, 256);      // clear buffer
        for(int i=0; i<255; i++) {
            char recv = pc.getc();
            if ( recv == '\r' || recv == '\n' ) {
                pc.printf("\r\n");
                break;
            }
            buf[i] = pc.putc(recv);
        }
        RPC::call(buf, outbuf);
        pc.printf("%s\r\n", outbuf);
    }
}

void LEDControl(Arguments *in, Reply *out)
{
    // Have code here to call another RPC function to wake up specific led or close it
    double x, y;

    x = in->getArg<double>();
    y = in->getArg<double>();

    q1.call(blink_led1, x);
    q3.call(blink_led3, y);
}

void blink_led1(double x)
{
    char buffer[200], outbuf[256];
    char strings[20];
    int n;

    while (1) {
        n = sprintf(strings, "/myled%d/write %d", 1, 1);
        strcpy(buffer, strings);
        RPC::call(buffer, outbuf);
        wait(x);

        n = sprintf(strings, "/myled%d/write %d", 1, 0);
        strcpy(buffer, strings);
        RPC::call(buffer, outbuf);
        wait(x);
    }
}

void blink_led3(double x)
{
    char buffer[200], outbuf[256];
    char strings[20];
    int n;
    
    while (1) {
        n = sprintf(strings, "/myled%d/write %d", 3, 1);
        strcpy(buffer, strings);
        RPC::call(buffer, outbuf);
        wait(x);

        n = sprintf(strings, "/myled%d/write %d", 3, 0);
        strcpy(buffer, strings);
        RPC::call(buffer, outbuf);
        wait(x);
    }
}