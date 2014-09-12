#include <18f452.h>
#DEVICE ADC=10
#use delay(clock=20M)
#fuses HS, NOWDT, NOPUT, NOLVP, NOPROTECT//, NOMCLR
#use RS232(BAUD=9600, UART1, PARITY= N, STREAM = COM_int )
#include "ds1307.c"


int sec;
int min;
int hrs;
int  day;
int  month;
int  yr;
int  dow;
unsigned long valor, valor1, valor2, valor3, valor4, valor5;
/*#int_RDA
  void RDA_isr()
  {
     if( kbhit() ){

        buffer_serial[ i_ascii ] = fgetc(COM_int);
        i_ascii=i_ascii+1;
        if(i_ascii >= 90)i_ascii=0;
   }
  }*/

void main(main){

enable_interrupts( GLOBAL );
//enable_interrupts( INT_RDA );
//   enable_interrupts( INT_RTCC );

 // setup_adc(ADC_CLOCK_INTERNAL); //configura el converso
  setup_adc(ADC_CLOCK_DIV_32);
  setup_adc_ports(ALL_ANALOG);

 ds1307_init ();      ///se inicializa el ds1307
   delay_ms(1000);
   SET_TRIS_A( 0xFF );

//ds1307_set_date_time (9, 9, 14, 21, 22, 28, 10); /// se escribe en el displositivo
fprintf(COM_int,"Hola Mundo");
while(true){
    ds1307_get_date (day, month, yr, dow);  /// se obtiene la fecha
    ds1307_get_time (hrs, min, sec);   /// se obtiene la hora
   // fprintf(COM_int,"AT$MSGSND=4,\"");
   // fprintf(COM_int, "20%d%02d%02d, %d, %d, %d\"\n\r", yr, month, day, hrs, min, sec);

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
      fprintf(COM_int, "#PT01,20%d%02d%02d,%02d%02d%02d,%Lu,%Lu,%Lu,%Lu,%Lu,%Lu,000,999$\"\n\r"yr,month, day, hrs, min, sec, valor, valor1, valor2, valor3, valor4, valor5);
      output_high(pin_c1);
      delay_ms(1000);
      output_low(pin_c1);
      delay_ms(1000);

}
}
