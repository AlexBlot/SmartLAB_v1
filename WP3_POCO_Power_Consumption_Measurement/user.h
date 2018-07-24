/******************************************************************************/
/* User Level #define Macros                                                  */
/******************************************************************************/

#define SLEEP()     asm("sleep")
#define GREENLED    LATCbits.LATC4
#define REDLED      LATCbits.LATC5
#define CSN         LATCbits.LATC6
#define CE          LATAbits.LATA4
#define IRQ         LATAbits.LATA5
#define SDI         LATBbits.LATB4
#define SCK         LATBbits.LATB6

#define UNUSED_TRIS 0
#define MUSHADD     0x0D
/******************************************************************************/
/* User Function Prototypes                                                   */
/******************************************************************************/

/* TODO User level functions prototypes (i.e. InitApp) go here */
void InitApp(void);         /* I/O and Peripheral Initialization */
void timer_setup_and_start_IE(void);


/******************************************************************************/
/* Global Variables                                            */
/******************************************************************************/

uint8_t SW;             // Switch value
uint8_t MUSHROOM_ON;    // Mushroom state
uint8_t TMR_CNT;        // Timer Counter
