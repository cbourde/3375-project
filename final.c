
#define SW_BASE 0xFF200040
#define JP1_BASE 0xFF200060
#define ADC_BASE 0xFF204000
#define KEY_BASE 0xFF200050

#define LED_BASE 0xff200000
#define VGA_PIXEL_BASE 0xc8000000
#define A9_TIMER_BASE 0xfffec600

typedef struct _ADC
{
    unsigned int ch0;
    unsigned int ch1;
    unsigned int ch2;
    unsigned int ch3;
    unsigned int ch4;
    unsigned int ch5;
    unsigned int ch6;
    unsigned int ch7;
} ADC;

typedef struct _buttons
{
    int data;
    int nothing;
    int interrupt_mask;
    int edge_capture;
} buttons;

// Pointers to hardware
volatile int *led = (int *)LED_BASE;
volatile unsigned int *SW_switch_ptr = (unsigned int *)SW_BASE;
volatile unsigned int *const GPIO_ptr = (unsigned int *)JP1_BASE;
volatile ADC *const adc_ptr = (ADC *)ADC_BASE;
volatile buttons *buttons_ptr = (buttons *)KEY_BASE;

// function prototypes
// Read the switches
int getSWValue(void);

// Lookup tables for characters (I hope your IDE can collapse these lol)
int ascii_chars[96][7][5] = {
    {
        // Space
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
    },
    {
        // !
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
    },
    {
        // "
        {0, 1, 0, 1, 0},
        {0, 1, 0, 1, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
    },
    {
        // #
        {0, 1, 0, 1, 0},
        {0, 1, 0, 1, 0},
        {1, 1, 1, 1, 1},
        {0, 1, 0, 1, 0},
        {1, 1, 1, 1, 1},
        {0, 1, 0, 1, 0},
        {0, 1, 0, 1, 0},
    },
    {
        // $
        {0, 0, 1, 0, 0},
        {0, 1, 1, 1, 1},
        {1, 0, 0, 0, 0},
        {0, 1, 1, 1, 0},
        {0, 0, 0, 0, 1},
        {1, 1, 1, 1, 0},
        {0, 0, 1, 0, 0},
    },
    {
        // degree sign (replaces %)
        {0, 1, 1, 1, 0},
        {0, 1, 0, 1, 0},
        {0, 1, 1, 1, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
    },
    {
        // &
        {0, 1, 1, 0, 0},
        {1, 0, 0, 1, 0},
        {1, 0, 0, 1, 0},
        {0, 1, 1, 0, 0},
        {1, 0, 1, 0, 1},
        {1, 0, 0, 1, 0},
        {0, 1, 1, 0, 1},
    },
    {
        // '
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
    },
    {
        // (
        {0, 0, 0, 1, 0},
        {0, 0, 1, 0, 0},
        {0, 1, 0, 0, 0},
        {0, 1, 0, 0, 0},
        {0, 1, 0, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 0, 1, 0},
    },
    {
        // )
        {0, 1, 0, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 0, 1, 0},
        {0, 0, 0, 1, 0},
        {0, 0, 0, 1, 0},
        {0, 0, 1, 0, 0},
        {0, 1, 0, 0, 0},
    },
    {
        // *
        {0, 0, 1, 0, 0},
        {1, 0, 1, 0, 1},
        {0, 1, 1, 1, 0},
        {1, 0, 1, 0, 1},
        {0, 0, 1, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
    },
    {
        // +
        {0, 0, 0, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {1, 1, 1, 1, 1},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 0, 0, 0},
    },
    {
        // ,
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 1, 0, 0, 0},
    },
    {
        // -
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {1, 1, 1, 1, 1},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
    },
    {
        // .
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 1, 0, 0},
    },
    {
        // /
        {0, 0, 0, 0, 1},
        {0, 0, 0, 1, 0},
        {0, 0, 0, 1, 0},
        {0, 0, 1, 0, 0},
        {0, 1, 0, 0, 0},
        {0, 1, 0, 0, 0},
        {1, 0, 0, 0, 0},
    },
    {// 0
     {0, 1, 1, 1, 0},
     {1, 0, 0, 0, 1},
     {1, 0, 0, 1, 1},
     {1, 0, 1, 0, 1},
     {1, 1, 0, 0, 1},
     {1, 0, 0, 0, 1},
     {0, 1, 1, 1, 0}},
    {// 1
     {0, 0, 1, 0, 0},
     {0, 1, 1, 0, 0},
     {0, 0, 1, 0, 0},
     {0, 0, 1, 0, 0},
     {0, 0, 1, 0, 0},
     {0, 0, 1, 0, 0},
     {0, 1, 1, 1, 0}},
    {// 2
     {0, 1, 1, 1, 0},
     {1, 0, 0, 0, 1},
     {0, 0, 0, 1, 0},
     {0, 0, 1, 0, 0},
     {0, 1, 0, 0, 0},
     {1, 0, 0, 0, 0},
     {1, 1, 1, 1, 1}},
    {// 3
     {0, 1, 1, 1, 0},
     {1, 0, 0, 0, 1},
     {0, 0, 0, 0, 1},
     {0, 0, 1, 1, 0},
     {0, 0, 0, 0, 1},
     {1, 0, 0, 0, 1},
     {0, 1, 1, 1, 0}},
    {// 4
     {0, 0, 0, 1, 0},
     {0, 0, 1, 1, 0},
     {0, 1, 0, 1, 0},
     {1, 0, 0, 1, 0},
     {1, 1, 1, 1, 1},
     {0, 0, 0, 1, 0},
     {0, 0, 0, 1, 0}},
    {// 5
     {1, 1, 1, 1, 1},
     {1, 0, 0, 0, 0},
     {1, 1, 1, 1, 0},
     {0, 0, 0, 0, 1},
     {0, 0, 0, 0, 1},
     {1, 0, 0, 0, 1},
     {0, 1, 1, 1, 0}},
    {// 6
     {0, 1, 1, 1, 0},
     {1, 0, 0, 0, 1},
     {1, 0, 0, 0, 0},
     {1, 1, 1, 1, 0},
     {1, 0, 0, 0, 1},
     {1, 0, 0, 0, 1},
     {0, 1, 1, 1, 0}},
    {// 7
     {1, 1, 1, 1, 1},
     {0, 0, 0, 0, 1},
     {0, 0, 0, 1, 0},
     {0, 0, 1, 0, 0},
     {0, 0, 1, 0, 0},
     {0, 1, 0, 0, 0},
     {0, 1, 0, 0, 0}},
    {// 8
     {0, 1, 1, 1, 0},
     {1, 0, 0, 0, 1},
     {1, 0, 0, 0, 1},
     {0, 1, 1, 1, 0},
     {1, 0, 0, 0, 1},
     {1, 0, 0, 0, 1},
     {0, 1, 1, 1, 0}},
    {// 9
     {0, 1, 1, 1, 0},
     {1, 0, 0, 0, 1},
     {1, 0, 0, 0, 1},
     {0, 1, 1, 1, 1},
     {0, 0, 0, 0, 1},
     {1, 0, 0, 0, 1},
     {0, 1, 1, 1, 0}},
    {
        // :
        {0, 0, 0, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 0, 0, 0},
    },
    {
        // ;
        {0, 0, 0, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
    },
    {
        // <
        {0, 0, 0, 0, 0},
        {0, 0, 0, 1, 0},
        {0, 0, 1, 0, 0},
        {0, 1, 0, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 0, 1, 0},
        {0, 0, 0, 0, 0},
    },
    {
        // =
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 1, 1, 1, 0},
        {0, 0, 0, 0, 0},
        {0, 1, 1, 1, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
    },
    {
        // >
        {0, 0, 0, 0, 0},
        {0, 1, 0, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 0, 1, 0},
        {0, 0, 1, 0, 0},
        {0, 1, 0, 0, 0},
        {0, 0, 0, 0, 0},
    },
    {
        // ?
        {0, 1, 1, 1, 0},
        {1, 0, 0, 0, 1},
        {0, 0, 0, 0, 1},
        {0, 0, 0, 1, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 1, 0, 0},
    },
    {
        // @
        {0, 1, 1, 1, 0},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 1, 1},
        {1, 0, 1, 0, 1},
        {1, 0, 1, 1, 1},
        {1, 0, 0, 0, 0},
        {0, 1, 1, 1, 1},
    },
    {
        // A
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 1, 0, 1, 0},
        {0, 1, 0, 1, 0},
        {0, 1, 1, 1, 0},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
    },
    {
        // B
        {1, 1, 1, 1, 0},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 1, 1, 1, 0},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 1, 1, 1, 0},
    },
    {
        // C
        {0, 1, 1, 1, 0},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 0},
        {1, 0, 0, 0, 0},
        {1, 0, 0, 0, 0},
        {1, 0, 0, 0, 1},
        {0, 1, 1, 1, 0},
    },
    {
        // D
        {1, 1, 1, 0, 0},
        {1, 0, 0, 1, 0},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 1, 0},
        {1, 1, 1, 0, 0},
    },
    {
        // E
        {1, 1, 1, 1, 1},
        {1, 0, 0, 0, 0},
        {1, 0, 0, 0, 0},
        {1, 1, 1, 1, 0},
        {1, 0, 0, 0, 0},
        {1, 0, 0, 0, 0},
        {1, 1, 1, 1, 1},
    },
    {
        // F
        {1, 1, 1, 1, 1},
        {1, 0, 0, 0, 0},
        {1, 0, 0, 0, 0},
        {1, 1, 1, 1, 0},
        {1, 0, 0, 0, 0},
        {1, 0, 0, 0, 0},
        {1, 0, 0, 0, 0},
    },
    {
        // G
        {0, 1, 1, 1, 0},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 0},
        {1, 0, 0, 0, 0},
        {1, 0, 0, 1, 1},
        {1, 0, 0, 0, 1},
        {0, 1, 1, 1, 0},
    },
    {
        // H
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 1, 1, 1, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
    },
    {
        // I
        {1, 1, 1, 1, 1},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {1, 1, 1, 1, 1},
    },
    {
        // J
        {1, 1, 1, 1, 1},
        {0, 0, 0, 1, 0},
        {0, 0, 0, 1, 0},
        {0, 0, 0, 1, 0},
        {1, 0, 0, 1, 0},
        {1, 0, 0, 1, 0},
        {0, 1, 1, 0, 0},
    },
    {
        // K
        {1, 0, 0, 0, 1},
        {1, 0, 0, 1, 0},
        {1, 0, 1, 0, 0},
        {1, 1, 0, 0, 0},
        {1, 0, 1, 0, 0},
        {1, 0, 0, 1, 0},
        {1, 0, 0, 0, 1},
    },
    {
        // L
        {1, 0, 0, 0, 0},
        {1, 0, 0, 0, 0},
        {1, 0, 0, 0, 0},
        {1, 0, 0, 0, 0},
        {1, 0, 0, 0, 0},
        {1, 0, 0, 0, 0},
        {1, 1, 1, 1, 1},
    },
    {
        // M
        {1, 0, 0, 0, 1},
        {1, 1, 0, 1, 1},
        {1, 0, 1, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
    },
    {
        // N
        {1, 0, 0, 0, 0},
        {1, 1, 0, 0, 0},
        {1, 0, 1, 0, 0},
        {1, 0, 0, 1, 0},
        {1, 0, 0, 0, 0},
        {1, 0, 0, 0, 0},
        {1, 0, 0, 0, 0},
    },
    {
        // O
        {0, 1, 1, 1, 0},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {0, 1, 1, 1, 0},
    },
    {
        // P
        {1, 1, 1, 1, 0},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 1, 1, 1, 0},
        {1, 0, 0, 0, 0},
        {1, 0, 0, 0, 0},
        {1, 0, 0, 0, 0},
    },
    {
        // Q
        {0, 1, 1, 1, 0},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 1, 0},
        {0, 1, 1, 0, 1},
    },
    {
        // R
        {1, 1, 1, 1, 0},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 1, 1, 1, 0},
        {1, 0, 1, 0, 0},
        {1, 0, 0, 1, 0},
        {1, 0, 0, 0, 1},
    },
    {
        // S
        {0, 1, 1, 1, 0},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 0},
        {0, 1, 1, 1, 0},
        {0, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {0, 1, 1, 1, 0},
    },
    {
        // T
        {1, 1, 1, 1, 1},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
    },
    {
        // U
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {0, 1, 1, 1, 0},
    },
    {
        // V
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {0, 1, 0, 1, 0},
        {0, 1, 0, 1, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
    },
    {
        // W
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 1, 0, 1},
        {1, 1, 0, 1, 1},
        {1, 0, 0, 0, 1},
    },
    {
        // X
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {0, 1, 0, 1, 0},
        {0, 0, 1, 0, 0},
        {0, 1, 0, 1, 0},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
    },
    {
        // Y
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {0, 1, 0, 1, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
    },
    {
        // Z
        {1, 1, 1, 1, 1},
        {0, 0, 0, 0, 1},
        {0, 0, 0, 1, 0},
        {0, 0, 1, 0, 0},
        {0, 1, 0, 0, 0},
        {1, 0, 0, 0, 0},
        {1, 1, 1, 1, 1},
    },
    {
        // [
        {0, 1, 1, 1, 0},
        {0, 1, 0, 0, 0},
        {0, 1, 0, 0, 0},
        {0, 1, 0, 0, 0},
        {0, 1, 0, 0, 0},
        {0, 1, 0, 0, 0},
        {0, 1, 1, 1, 0},
    },
    {
        // \ 
        {1, 0, 0, 0, 0},
        {0, 1, 0, 0, 0},
        {0, 1, 0, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 0, 1, 0},
        {0, 0, 0, 1, 0},
        {0, 0, 0, 0, 1},
    },
    {
        // ]
        {0, 1, 1, 1, 0},
        {0, 0, 0, 1, 0},
        {0, 0, 0, 1, 0},
        {0, 0, 0, 1, 0},
        {0, 0, 0, 1, 0},
        {0, 0, 0, 1, 0},
        {0, 1, 1, 1, 0},
    },
    {
        // ^
        {0, 0, 1, 0, 0},
        {0, 1, 0, 1, 0},
        {1, 0, 0, 0, 1},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
    },
    {
        // _
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {1, 1, 1, 1, 1},
    },
    {
        // `
        {0, 1, 0, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 0, 1, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
    },
    {
        // a
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 1, 1, 1, 0},
        {0, 0, 0, 0, 1},
        {0, 1, 1, 1, 1},
        {1, 0, 0, 0, 1},
        {0, 1, 1, 1, 1},
    },
    {
        // b
        {1, 0, 0, 0, 0},
        {1, 0, 0, 0, 0},
        {1, 0, 0, 0, 0},
        {1, 1, 1, 1, 0},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 1, 1, 1, 0},
    },
    {
        // c
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 1, 1, 1, 0},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 0},
        {1, 0, 0, 0, 1},
        {0, 1, 1, 1, 0},
    },
    {
        // d
        {0, 0, 0, 0, 1},
        {0, 0, 0, 0, 1},
        {0, 0, 0, 0, 1},
        {0, 1, 1, 1, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {0, 1, 1, 1, 1},
    },
    {
        // e
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 1, 1, 1, 0},
        {1, 0, 0, 0, 1},
        {1, 1, 1, 1, 1},
        {1, 0, 0, 0, 0},
        {0, 1, 1, 1, 1},
    },
    {
        // f
        {0, 0, 1, 1, 0},
        {0, 1, 0, 0, 0},
        {1, 1, 1, 1, 0},
        {0, 1, 0, 0, 0},
        {0, 1, 0, 0, 0},
        {0, 1, 0, 0, 0},
        {0, 1, 0, 0, 0},
    },
    {
        // g 103
        {0, 0, 0, 0, 0},
        {0, 1, 1, 1, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {0, 1, 1, 1, 1},
        {0, 0, 0, 0, 1},
        {0, 1, 1, 1, 0},
    },
    {
        // h
        {1, 0, 0, 0, 0},
        {1, 0, 0, 0, 0},
        {1, 0, 0, 0, 0},
        {1, 0, 1, 1, 0},
        {1, 1, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
    },
    {
        // i
        {0, 0, 1, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 1, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 1, 1, 1, 0},
    },
    {
        // j 106
        {0, 0, 0, 1, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 1, 1, 0},
        {0, 0, 0, 1, 0},
        {0, 0, 0, 1, 0},
        {0, 1, 0, 1, 0},
        {0, 0, 1, 0, 0},
    },
    {
        // k
        {1, 0, 0, 0, 0},
        {1, 0, 0, 0, 0},
        {1, 0, 0, 1, 0},
        {1, 0, 1, 0, 0},
        {1, 1, 0, 0, 0},
        {1, 0, 1, 0, 0},
        {1, 0, 0, 1, 0},
    },
    {
        // l
        {0, 1, 0, 0, 0},
        {0, 1, 0, 0, 0},
        {0, 1, 0, 0, 0},
        {0, 1, 0, 0, 0},
        {0, 1, 0, 0, 0},
        {0, 1, 0, 0, 0},
        {0, 0, 1, 0, 0},
    },
    {
        // m
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {1, 1, 0, 1, 0},
        {1, 0, 1, 0, 1},
        {1, 0, 1, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
    },
    {
        // n
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {1, 1, 1, 1, 0},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
    },
    {
        // o
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 1, 1, 1, 0},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {0, 1, 1, 1, 0},
    },
    {
        // p 112
        {0, 0, 0, 0, 0},
        {1, 1, 1, 1, 0},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 1, 1, 1, 0},
        {1, 0, 0, 0, 0},
        {1, 0, 0, 0, 0},
    },
    {
        // q 113
        {0, 0, 0, 0, 0},
        {0, 1, 1, 1, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {0, 1, 1, 1, 1},
        {0, 0, 0, 0, 1},
        {0, 0, 0, 0, 1},
    },
    {
        // r
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {1, 0, 1, 1, 0},
        {1, 1, 0, 0, 1},
        {1, 0, 0, 0, 0},
        {1, 0, 0, 0, 0},
        {1, 0, 0, 0, 0},
    },
    {
        // s
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 1, 1, 1, 1},
        {1, 0, 0, 0, 0},
        {0, 1, 1, 1, 0},
        {0, 0, 0, 0, 1},
        {1, 1, 1, 1, 0},
    },
    {
        // t
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 1, 1, 1, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 0, 1, 0},
    },
    {
        // u
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {0, 1, 1, 1, 1},
    },
    {
        // v
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {0, 1, 0, 1, 0},
        {0, 0, 1, 0, 0},
    },
    {
        // w
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 1, 0, 1},
        {1, 1, 0, 1, 1},
        {1, 0, 0, 0, 1},
    },
    {
        // x
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {1, 0, 0, 0, 1},
        {0, 1, 0, 1, 0},
        {0, 0, 1, 0, 0},
        {0, 1, 0, 1, 0},
        {1, 0, 0, 0, 1},
    },
    {
        // y 121
        {0, 0, 0, 0, 0},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {0, 1, 1, 1, 1},
        {0, 0, 0, 0, 1},
        {0, 1, 1, 1, 0},
    },
    {
        // z
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {1, 1, 1, 1, 1},
        {0, 0, 0, 1, 0},
        {0, 0, 1, 0, 0},
        {0, 1, 0, 0, 0},
        {1, 1, 1, 1, 1},
    },
    {
        // {
        {0, 0, 0, 1, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 1, 0, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 0, 1, 0},
    },
    {
        // |
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
    },
    {
        // }
        {0, 1, 0, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 0, 1, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 1, 0, 0, 0},
    },
    {
        // ~
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 1, 0, 0, 0},
        {1, 0, 1, 0, 1},
        {0, 0, 0, 1, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
    }};

int digits[10][7][5] = {
    {// 0
     {0, 1, 1, 1, 0},
     {1, 0, 0, 0, 1},
     {1, 0, 0, 1, 1},
     {1, 0, 1, 0, 1},
     {1, 1, 0, 0, 1},
     {1, 0, 0, 0, 1},
     {0, 1, 1, 1, 0}},
    {// 1
     {0, 0, 1, 0, 0},
     {0, 1, 1, 0, 0},
     {0, 0, 1, 0, 0},
     {0, 0, 1, 0, 0},
     {0, 0, 1, 0, 0},
     {0, 0, 1, 0, 0},
     {0, 1, 1, 1, 0}},
    {// 2
     {0, 1, 1, 1, 0},
     {1, 0, 0, 0, 1},
     {0, 0, 0, 1, 0},
     {0, 0, 1, 0, 0},
     {0, 1, 0, 0, 0},
     {1, 0, 0, 0, 0},
     {1, 1, 1, 1, 1}},
    {// 3
     {0, 1, 1, 1, 0},
     {1, 0, 0, 0, 1},
     {0, 0, 0, 0, 1},
     {0, 0, 1, 1, 0},
     {0, 0, 0, 0, 1},
     {1, 0, 0, 0, 1},
     {0, 1, 1, 1, 0}},
    {// 4
     {0, 0, 0, 1, 0},
     {0, 0, 1, 1, 0},
     {0, 1, 0, 1, 0},
     {1, 0, 0, 1, 0},
     {1, 1, 1, 1, 1},
     {0, 0, 0, 1, 0},
     {0, 0, 0, 1, 0}},
    {// 5
     {1, 1, 1, 1, 1},
     {1, 0, 0, 0, 0},
     {1, 1, 1, 1, 0},
     {0, 0, 0, 0, 1},
     {0, 0, 0, 0, 1},
     {1, 0, 0, 0, 1},
     {0, 1, 1, 1, 0}},
    {// 6
     {0, 1, 1, 1, 0},
     {1, 0, 0, 0, 1},
     {1, 0, 0, 0, 0},
     {1, 1, 1, 1, 0},
     {1, 0, 0, 0, 1},
     {1, 0, 0, 0, 1},
     {0, 1, 1, 1, 0}},
    {// 7
     {1, 1, 1, 1, 1},
     {0, 0, 0, 0, 1},
     {0, 0, 0, 1, 0},
     {0, 0, 1, 0, 0},
     {0, 0, 1, 0, 0},
     {0, 1, 0, 0, 0},
     {0, 1, 0, 0, 0}},
    {// 8
     {0, 1, 1, 1, 0},
     {1, 0, 0, 0, 1},
     {1, 0, 0, 0, 1},
     {0, 1, 1, 1, 0},
     {1, 0, 0, 0, 1},
     {1, 0, 0, 0, 1},
     {0, 1, 1, 1, 0}},
    {// 9
     {0, 1, 1, 1, 0},
     {1, 0, 0, 0, 1},
     {1, 0, 0, 0, 1},
     {0, 1, 1, 1, 1},
     {0, 0, 0, 0, 1},
     {1, 0, 0, 0, 1},
     {0, 1, 1, 1, 0}}};

typedef struct _a9_timer
{
    int load;
    int count;
    int control;
    int status;
} a9_timer;

// Basic RGB color struct. The VGA display only supports the following ranges for each component:
// 0 <= r <= 31
// 0 <= g <= 63
// 0 <= b <= 31
typedef struct _color
{
    int r;
    int g;
    int b;
} color;

// Colors used in this program
color black;
color white;
color green;
color blue;
color red;  // actually orange but whatever

volatile a9_timer *timer = (a9_timer *)A9_TIMER_BASE;

void init_timer()
{
    timer->load = 1000000;     // Load period of 1000000 cycles
    timer->control = 199 << 8; // Set prescaler to 200 cycles (period = 1 second)
    timer->control |= 3;       // Set timer to continuous and start
}

int check_timeout()
{
    int timeout = timer->status;
    if (timeout)
    {
        timer->status = 1;
    }
    return timeout;
}

// Assign values to the colors declared above
void init_colors()
{
    black.r = 0;
    black.g = 0;
    black.b = 0;

    white.r = 31;
    white.g = 63;
    white.b = 31;

    green.r = 8;
    green.g = 63;
    green.b = 8;

    blue.r = 8;
    blue.g = 16;
    blue.b = 31;

    red.r = 31;
    red.g = 16;
    red.b = 8;
}

/**
 * @brief Set the state of a single LED
 *
 * @param index The LED to change
 * @param state 1 for on, 0 for off
 */
void write_led(int index, int state)
{
    if (state == 0)
    {
        *led &= ~(1 << index); // Clear only the bit at index
    }
    else
    {
        *led |= (1 << index); // Set only the bit at index
    }
}

/**
 * @brief Draw a single pixel at the specified coordinates.
 *
 * @param x X coordinate of the pixel (0 <= x <= 320)
 * @param y Y coordinate of the pixel (0 <= y <= 240)
 * @param c Pixel color
 */
void draw_pixel(int x, int y, color col)
{

    int pixel_addr = VGA_PIXEL_BASE + (y << 10) + (x << 1);

    int pixel_color = (col.r << 11) | (col.g << 6) | col.b;

    *(short *)pixel_addr = pixel_color;
}

/**
 * @brief Draw a filled rectangle at the specified coordinates.
 *
 * @param x X coordinate of top left corner of box (0 <= x <= 320)
 * @param y Y coordinate of top left corner of box (0 <= y <= 240)
 * @param w Width of box (1 <= w <= 320-x)
 * @param h Height of box (1 <= h <= 240-y)
 * @param c Color to draw box with
 */
void draw_box(int x, int y, int w, int h, color c)
{
    for (int i = 0; i < w; i++)
    {
        for (int j = 0; j < h; j++)
        {
            draw_pixel(x + i, y + j, c);
        }
    }
}

/**
 * @brief Set the background color of the display (also clears it)
 *
 * @param c The desired background color
 */
void set_display_bg(color c)
{
    draw_box(0, 0, 320, 240, c);
}

/**
 * @brief Draw a single digit on the display
 *
 * @param x X coordinate of top left corner of digit
 * @param y Y coordinate of top left corner of digit
 * @param digit The digit to draw
 * @param scale Scaling factor (width = 5 * scale, height = 7 * scale)
 * @param c Color to draw digit with
 */
void draw_digit(int x, int y, int digit, int scale, color c)
{
    for (int i = 0; i < 7; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            // For each pixel in the digit, draw a square of size (scale) at coordinates relative to x and y
            if (digits[digit][i][j])
            {
                draw_box(x + (j * scale), y + (i * scale), scale, scale, c);
            }
        }
    }
}

/**
 * @brief Draws a number on the screen (up to 3 digits)
 *
 * @param x X coordinate of top left corner
 * @param y Y coordinate of top left corner
 * @param num The number to draw
 * @param scale Scaling factor
 * @param c Color to draw number with
 */
void draw_number(int x, int y, int num, int scale, color c)
{
    // Break up num into its 3 lowest-order digits
    int num_digits[3];
    int current = num;
    int place_val = 100;
    for (int i = 0; i < 3; i++)
    {
        num_digits[i] = current / place_val;
        current %= place_val;
        place_val /= 10;
    }

    // Display the digits
    int curr_x = x;
    int leading_zero = 1;
    for (int i = 0; i < 3; i++)
    {
        if (!leading_zero || num_digits[i] != 0 || i == 2)
        {
            draw_digit(curr_x, y, num_digits[i], scale, c);
            leading_zero = 0;
            curr_x += 6 * scale;
        }
    }
}

/**
 * @brief Draws a single ASCII character to the screen
 * Note: Percent signs (%) are rendered as degree signs since the degree sign is not in the basic ASCII character set.
 * @param x X coordinate of top left corner
 * @param y Y coordinate of top left corner
 * @param a The character to draw
 * @param scale Scaling factor
 * @param c Color to draw character with
 */
void draw_ascii_char(int x, int y, char a, int scale, color c)
{

    int char_index = a - 32;

    // For g, j, p, q, and y, draw the letter two 'pixels' lower
    int below_baseline = 0;
    if (a == 103 || a == 106 || a == 112 || a == 113 || a == 121)
    {
        below_baseline = 2;
    }
    for (int i = 0; i < 7; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            // For each pixel in the digit, draw a square of size (scale) at coordinates relative to x and y
            if (ascii_chars[char_index][i][j])
            {
                draw_box(x + (j * scale), y + (i * scale) + below_baseline, scale, scale, c);
            }
        }
    }
}

/**
 * @brief Draw an ASCII string to the display
 *
 * @param x X coordinate of top left corner
 * @param y Y coordinate of top left corner
 * @param string The string to draw
 * @param len Length of the string (not including the trailing null character)
 * @param scale Scaling factor
 * @param c Color to draw string with
 */
void draw_ascii_string(int x, int y, char string[], int len, int scale, color c)
{
    // Display the digits
    int curr_x = x;
    for (int i = 0; i < len; i++)
    {
        draw_ascii_char(curr_x, y, string[i], scale, c);
        curr_x += 6 * scale;
    }
}

/**
 * @brief Update the display using the given parameters
 *
 * @param room The currently selected room number
 * @param current_temp The current temperature
 * @param set_temp The set temperature
 * @param mode 0 for off, 1 for heat, 2 for cool
 * @param sys_on Whether the system is on, 0 for off, 1 for on
 * @param fan_on Whether the fan is on, 0 for off, 1 for on
 * @param unit Temperature unit, 0 for C, 1 for F
 */
void update_display(int room, int current_temp, int set_temp, int mode, int sys_on, int fan_on, int unit)
{
    // Clear the entire display to black. Yeah it's inefficient but it's easy.
    // The real solution would be to only clear the areas with numbers/indicators, but I ran out of time lmao
    set_display_bg(black);

    color mode_color = white;
    if (mode == 1)
    { // Heat
        mode_color = red;
    }
    else if (mode == 2)
    { // Cool
        mode_color = blue;
    }

    // Room number
    draw_ascii_string(20, 20, "Room Temp:", 10, 3, white);

    // Current temp
    draw_ascii_string(20, 50, "Current", 7, 3, white);
    draw_number(20, 80, current_temp, 8, white);
    if (!unit)
    {
        // Celsius
        draw_ascii_string(115, 80, "%C", 2, 3, white);
    }
    else
    {
        // Fahrenheit
        draw_ascii_string(115, 80, "%F", 2, 3, white);
    }

    // Set temp
    draw_ascii_string(185, 50, "Set", 3, 3, white);
    draw_number(185, 80, set_temp, 8, mode_color);
    if (!unit)
    {
        // Celsius
        draw_ascii_string(280, 80, "%C", 2, 3, white);
    }
    else
    {
        // Fahrenheit
        draw_ascii_string(280, 80, "%F", 2, 3, white);
    }

    // "Heat/Cool On" indicator
    if (sys_on)
    {
        if (mode == 1)
        {
            // Heat
            draw_ascii_string(20, 150, "Heat On", 7, 3, mode_color);
        }
        else if (mode == 2)
        {
            // Cool
            draw_ascii_string(20, 150, "Cooling On", 10, 3, mode_color);
        }
    }

    // "Fan On" indicator
    if (fan_on)
    {
        draw_ascii_string(20, 180, "Fan On", 6, 3, green);
    }
}

int main()
{
    init_colors();
    init_timer();

    // Set pin directions
    GPIO_ptr[1] |= 0x3FF; // Set as output bits (pins 0-9, lowest 10 bits are outputs)
    // Write to channel 1 to set the ADC to auto-update
    adc_ptr->ch1 = 0x1;
    int bit_mask = 1 << 15;
    int setTemp = 72;

    while (1)
    {
        int switchValue = getSWValue();
        if (getSWValue() & 1) // display on
        {
            // Check bit 15 to see if the conversion is complete

            int adcCH1data = adc_ptr->ch1;
            int adcCH0data = adc_ptr->ch0;
            int averageTemp = (adcCH0data + adcCH1data) / 2;
            int buttonValues = readButtons();

            int mode = getSWValue();
            int fan = getSWValue();

            // Figure out the mode
            if (mode &= 0x4)
            {
                // Cool
                mode = 2;
            }
            else
                mode = 1;   // Heat

            // Figure out the fan
            if (fan &= 0x8)
            {
                // Fan on: set the corresponding flag and enable the fan LED
                fan = 1;
                write_led(2, 1);
            }
            else
            {
                // Fan off: clear the corresponding flag and disable the fan LED
                fan = 0;
                write_led(2, 0);
            }

            averageTemp &= 0x1FFF;

            // Update Set Temperature
            setTemp += buttonValues;
            int currentTemp = ((averageTemp * 40 / 4096) + 50);
            int on = 0;

            // Check if the heating or cooling system needs to be on
            if ((mode == 1 && currentTemp < setTemp) || (mode == 2 && currentTemp > setTemp))
            {
                on = 1;
            }

            // If the system should be on, then enable the corresponding LED
            if (on){
                switch (mode){
                    case 1:
                        // Heat: Turn on the heat LED and make sure the cooling LED is off.
                        write_led(0, 1);
                        write_led(1, 0);
                        break;
                    case 2:
                        // Cool: Turn on the cooling LED and make sure the heat LED is off.
                        write_led(0, 0);
                        write_led(1, 1);
                        break;
                    default:
                        // Off: Make sure both the heat and cool LEDs are off.
                        write_led(0, 0);
                        write_led(1, 0);
                }
            }
            else{
                // If the system should be off, disable the corresponding LEDs.
                write_led(0, 0);
                write_led(1, 0);
            }

            // Update the display once per second
            if (check_timeout())
            {
                if (switchValue &= 0x2)
                {
                    // Fahrenheit
                    update_display(0, currentTemp, setTemp, mode, on, fan, 1);
                }
                else
                {
                    // Celsius
                    update_display(0, FtoC(currentTemp), FtoC(setTemp), mode, on, fan, 0);
                }
            }
        }

        else // display off
        {

            set_display_bg(black);
        }
    }
}

/**
 * @brief Read the state of the 4 lowest switches.
 * 
 * @return int State of the switches as the 4 lowest-order bits.
 */
int getSWValue(void)
{
    volatile unsigned int switchState = *SW_switch_ptr; // get the values stored in the switch
    switchState &= 0xf;                                 // AND Mask, only lowest bit
    return switchState;
}

/**
 * @brief Convert a temperature in Fahrenheit to the superior temperature unit.
 * 
 * @param F Temperature in inferior units.
 * @return int Temperature in superior units.
 */
int FtoC(int F)
{
    return (F - 32) * 5 / 9;
}

/**
 * @brief Read the state of buttons 0 and 1 and return the appropriate temperature adjustment.
 * Uses the edge capture registers so that a press is only registered once rather than every CPU cycle.
 * @return int 1 if button 0 is pressed, -1 if button 1 is pressed, 0 if no buttons are pressed. Button 0 has priority.
 */
int readButtons(void)
{
    int buttonState = buttons_ptr->edge_capture;
    if (buttonState)
    {
        int status;
        if (buttonState == 0x01)
        {
            // Start
            status = 1;
        }
        else if (buttonState == 0x02)
        {
            // Stop
            status = -1;
        }
        else
        {
            status = 0;
        }

        buttons_ptr->edge_capture = 0b1111;
        return status;
    }

    return 0;
}
