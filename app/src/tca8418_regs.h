#define TCA8418_CFG_REG 0x01
/*Key events interrupt enable to host processor */
#define TCA8418_CFG_SET_KE_IEN_BIT  1<<0
/*Processor interrupt is deasserted for 50us and reasserted with pending
 * intrrupts if host tries to clear interrupt while there are pending key press*/
#define TCA8418_CFG_SET_INT_CFG_BIT 1<<4

#define TCA8418_INT_STAT_REG 0x02
#define TCA8418_INT_STAT_K_INT_MASK  1 << 0
#define TCA8418_INT_STAT_K_INT_CLEAR 1 << 0

/* More registers exist but act as a FIFO */
#define TCA8418_KEY_EVENT_REG 0x04

/* Keypad or GPIO selection */
#define TCA8418_KP_GPIO1_REG 0x1D
#define TCA8418_KP_GPIO2_REG 0x1E
#define TCA8418_KP_GPIO3_REG 0x1F
