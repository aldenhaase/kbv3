#define DDRB *((volatile unsigned char*)0x37)

#define PORTB *((volatile unsigned char*)0x38)

int main(){

    DDRB |= (1 << 3);

    while (1) {
        PORTB |= (1 << 3);
        for (unsigned int i = 0; i < 50000; i++) {
            asm("nop");
        }
        PORTB = 0x0;
        for (unsigned int i = 0; i < 50000; i++) {
            asm("nop");
        }
    }
}
