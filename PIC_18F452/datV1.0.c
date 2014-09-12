/**********************************************\
 *       Kradac Robotics & Electronics        *
 *                                            *
 *  Proyecto:     Sensor  UTPL                *
 *  Programador:  Hugo Ramirez  & Ernesto P   *
 *  version:      1.0.1                       *
 *  Fecha:        10/06/2014                  *
 *                                            *
 **********************************************
 *
 *                                        
 *                                          
 *                                               
 *                                                     
 *
 *                                 
 *
 *                                    
 *                                                
 *                                                   
 *
 */


#include <18f452.h>
#device  ADC=10
#fuses   HS, NOWDT,NOLVP, NOPUT, NOPROTECT
#use     delay(clock=20M)

#use RS232(BAUD=9600, UART1, PARITY= N, STREAM = COM_int )

//#use rs232(baud=9600,parity=N,xmit=PIN_C6,rcv=PIN_C7,bits=8,STREAM=COM_INT)//Puerto soft skypatrol

#include "ds1307.c"


#define LED_STATUS PIN_C1

// VARIABLES GLOBALES

   unsigned int bufferSerial[100];
   int i_serial, i_timer, yr, month, day, hrs, min, sec, dow;
   long valor, valor1, valor2, valor3, valor4, valor5;



void main(void)
{
   enable_interrupts( GLOBAL );
   yr=month=day=hrs=min=sec=valor=valor1=0;
   
   setup_adc(ADC_CLOCK_DIV_32);
   setup_adc_ports(ALL_ANALOG);

   ds1307_init ();             ///se inicializa el ds1307
   delay_ms(1000);
   SET_TRIS_A( 0xFF );

   while(1)
   {
      ds1307_get_date (day, month, yr, dow);  /// se obtiene la fecha
      ds1307_get_time (hrs, min, sec);
      
      delay_ms(10);
      set_adc_channel(0);
      valor=read_adc();

      delay_ms(10);
      set_adc_channel(1);
      valor1=read_adc();
      
      delay_ms(10);
      set_adc_channel(2);
      valor2=read_adc();

      delay_ms(10);
      set_adc_channel(3);
      valor3=read_adc();

      delay_ms(10);
      set_adc_channel(4);
      valor4=read_adc();

      delay_ms(10);
      set_adc_channel(5);
      valor5=read_adc();

      delay_ms(10);
      
      fprintf(COM_int,"AT$MSGSND=4,\"");
      fprintf(COM_int, "#PT01,20%d%02d%02d,%02d%02d%02d,%Lu,%Lu,%Lu,%Lu,%Lu,%Lu,000,999$\"\n\r"
                                yr,month, day, 
                                          hrs, min, sec, 
                                                         valor,valor1,valor2,valor3,valor4,valor5);

      output_high( LED_STATUS );
      delay_ms(1000);
      output_low( LED_STATUS );
      
      delay_ms(5000);
   }
}

