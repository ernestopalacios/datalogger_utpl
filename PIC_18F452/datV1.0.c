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
#fuses HS, NOWDT,NOLVP, NOPUT, NOPROTECT
#use delay(clock=20M)

#use RS232(BAUD=9600, UART1, PARITY= N, STREAM = COM_int )

//#use rs232(baud=9600,parity=N,xmit=PIN_C6,rcv=PIN_C7,bits=8,STREAM=COM_INT)//Puerto soft skypatrol

#include "ds1307.c"


// VARIABLES GLOBALES

   unsigned int bufferSerial[100];
   int i_serial,i_timer, yr, month, day, hrs, min, sec, dow;
   long valor, valor1;




void main(void)
{
   enable_interrupts( GLOBAL );
   setup_adc(ADC_CLOCK_INTERNAL);
   yr=month=day=hrs=min=sec=valor=valor1=0;
   
   setup_adc(ADC_CLOCK_INTERNAL);
   ds1307_init();

   while(1)
   {
      ds1307_get_date (day, month, yr, dow);  /// se obtiene la fecha
      ds1307_get_time (hrs, min, sec);
      
      output_high(PIN_C1);
      delay_ms(500);
      output_low(PIN_C1);
      delay_ms(50000);     // Retardo de 50seg
      
      setup_adc_ports(AN0);   //Solo el canal 0
      set_adc_channel(0);
      set_adc_channel(1);
      
      valor=read_adc();
      valor1=read_adc();
      
      fprintf(COM_int,"AT$MSGSND=4,\"");
      fprintf(COM_int, "#PT02,%d%d%d,%d%d%d,%Lu,%Lu,000,000,000,000,000,000,999$\"\n\r", yr,month, day, hrs, min, sec, valor, valor1);
   }
}

