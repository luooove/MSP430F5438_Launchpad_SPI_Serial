#include <msp430.h>
//从机，接收SPI数据返回回去，并通过串口发给电脑。

void Clk_init()
{
	BCSCTL1 = CALBC1_16MHZ;                    // Set DCO
	DCOCTL = CALDCO_16MHZ;
}
void IO_init()
{
	P1SEL = BIT1 + BIT2 + BIT5 + BIT6 + BIT7;                     // P1.1 = RXD, P1.2=TXD
	P1SEL2 = BIT1 + BIT2 + BIT5 + BIT6 + BIT7;
}
void Serial_init()
{
	UCA0CTL1 |= UCSSEL_2;                     // SMCLK
	UCA0BR0 = 0x82;                              // 16MHz 9600 UCA0BRX=1666=0x0682
	UCA0BR1 = 0x06;                              // 16MHz 9600
	UCA0MCTL = UCBRS2 + UCBRS1;               // Modulation UCBRSx = 6
	UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
	IE2 |= UCA0RXIE;                          // Enable USCI_A0 RX interrupt
}
void SPI_init()
{
	UCB0CTL1 = UCSWRST;                       // **Put state machine in reset**
	UCB0CTL0 |= UCCKPL + UCMSB + UCSYNC;      // 3-pin, 8-bit SPI master
	UCB0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
	IE2 |= UCB0RXIE;                          // Enable USCI0 RX interrupt
}

void Init_all()
{
	//Clk_init();
	IO_init();
	Serial_init();
	SPI_init();
}
int main(void)
{
	WDTCTL = WDTPW + WDTHOLD;                 // Stop watchdog timer
	//while (!(P1IN & BIT5));                   // If clock sig from mstr stays low,
											// it is not yet in SPI mode
	Init_all();
	__bis_SR_register(LPM4_bits + GIE);       // Enter LPM4, enable interrupts
}
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
{

  while (!(IFG2&UCB0TXIFG));                // USCI_A0 TX buffer ready?
  UCB0TXBUF = UCB0RXBUF;                    // TX -> RXed character
	while (!(IFG2&UCA0TXIFG));                // USCI_A0 TX buffer ready?
	UCA0TXBUF = UCB0RXBUF;                    // TX -> RXed character
}
