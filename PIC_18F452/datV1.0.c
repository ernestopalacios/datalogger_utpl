/**********************************************\
 *       Kradac Robotics & Electronics        *
 *                                            *
 *  Proyecto:     Sensor  UTPL                *
 *  Programador:  Hugo Ramirez  & Ernesto P   *
 *  version:      3.1.0                       *
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
#include "ds1307.c"
#use RS232(BAUD=9600, UART1, PARITY= N, STREAM = COM_UART )

#define  RX_EXT   PIN_B6
#define  TX_EXT   PIN_B7

//Puerto soft skypatrol
#use rs232(baud=9600, parity=N, xmit=TX_EXT,rcv=RX_EXT, bits=8, STREAM=COM_EXT)


/////////////////////*******         DEFINICION DE CONSTANTES        *******////////////////////////////////////////
 #define LED_STATUS      PIN_C1     // Led de estado, parpadea cada Adqusición
 #define SEG_DAQ         10        // Tiempo de Adquisicion en segundos 
 #define SEG_GPS         12       // Pedir Trama de GPS para igualar el Reloj cada ciertos segundos

 #define BUFF_SER_0      200       // Tamano del buffer serial para UART0 - Hardware
 #define BUFF_SER_1      100      // Tamano del buffer serial externo - Software



//#define 

// VARIABLES GLOBALES

   char  bufferSerial[ BUFF_SER_0 ];                                // Buffer Serial para UART0

   volatile unsigned int i_serial = 0;
   volatile unsigned int i_timer  = 2;
   volatile unsigned int i = 0;           // Contador para laszos FOR()
   
   int _ovf_serial_0    = 0;            // Bandera para saber si se sobrepaso el buffer
   int _enviar_trama    = 0;           // Bandera para enviar la trama desde MAIN()

   long _segundos = 0;
   long _segundos_daq = 0;
   long _segundos_gps = 0;

   
   unsigned int yr     = 0;          // Dos ultimos digitos 20yy
   unsigned int month  = 0;
   unsigned int day    = 0;
   unsigned int hrs    = 0;
   unsigned int min    = 0;
   unsigned int sec    = 0;
   unsigned int dow    = 0;

   // Tiempo desde el GPS
   char hora1  = 0;
   char hora   = 0;

   char min1   = 0;
   char minu   = 0;

   char seg1   = 0;
   char seg    = 0;
   
   // Estructira de datos para la Hora y Fecha del PLC
   typedef struct {
      int  dia;
      int  mes;
      int  an;
      int  hora;
      int  minu;
      int  segu;
   } Tiempo;  //Fecha Interno del logo

   Tiempo rtc;
   Tiempo reloj;

   // Variables para alojar las lecturas de los pines analogicos
   long valor_AN0 = 0;
   long valor_AN1 = 0;
   long valor_AN2 = 0;
   long valor_AN3 = 0;
   long valor_AN4 = 0;
   long valor_AN5 = 0;


   /**
    * @brief Iguala la hora del RTC.
    * @details Toma una estructura de tiempo, fecha y hora y las graba el DS1307
    * 
    * @param rtc Estructura de tiempo.
    */
   void igualar_rtc( Tiempo& rtc );

   #INT_RDA
   void rda_isr()
   {
      if ( kbhit( COM_UART ) )
      {
         bufferSerial[ i_serial++ ] = fgetc( COM_UART );
         
         if( i_serial > BUFF_SER_0 )
         {
            i_serial      = 0;
            _ovf_serial_0 = 0;
         }
      }

   }

#INT_RTCC
/**
 * @brief Interrupcion del Timer 0
 * @details Realiza el monitoreo de los sensores y envia la trama
 */
void ISR_RTCC(void)
{
   /** Setup_timer_0( RTCC_DIV_256 );
    *  Fosc = 20/4 = 5 MHz
    *  Foverflow   = 5/256 = 0,01953125 Mhz = 51.2 us
    *              = 51.2us *(2^16-1 - 46004)= 9.999872
    *
    * En teoria se desiguala 1 segundo cada 21.7 horas....
    * 1 seg menos cada día.  En Teoria!!!!!!!
    * 
    * Siempre y cuando se demore muy poco 
    */
   set_timer0( 46004 );   // Reinicar el contador de segundos

   _segundos++;
   _segundos_daq++;
   _segundos_gps++;

   //fprintf(COM_EXT, "%Lu\r\n", _segundos );

   if ( _segundos_gps == SEG_GPS )
   {
      //Peticion de Trama GPS para igualar el RTC
      fprintf(COM_UART, "AT$GPSRD=10\r\n");
   }

   if ( _segundos_daq == SEG_DAQ )
   {
      _segundos_daq = 0;   // Reiniciar contador de segundos para adquisicion

      // Actualizar las variables y levantar la bandera para iniciar 
      // comunicacion
      set_adc_channel( 0 );
      delay_ms(10);
      valor_AN0 = read_adc();

      set_adc_channel( 1 );
      delay_ms(10);
      valor_AN1 = read_adc();
      
      set_adc_channel( 2 );
      delay_ms(10);
      valor_AN2 = read_adc();

      set_adc_channel( 3 );
      delay_ms(10);
      valor_AN3 = read_adc();

      set_adc_channel( 4 );
      delay_ms(10);
      valor_AN4 = read_adc();

      set_adc_channel( 5 );
      delay_ms(10);
      valor_AN5 = read_adc();

      _enviar_trama = 1;
   }

}


void main(void)
{
   enable_interrupts( GLOBAL );
   setup_adc(ADC_CLOCK_DIV_32);
   setup_adc_ports(ALL_ANALOG);
   Setup_timer_0( RTCC_DIV_256 );
   
   enable_interrupts( INT_RTCC );
   enable_interrupts( INT_RDA );
   
   SET_TRIS_A( 0xFF );            // Entradas Analogicas
   SET_TRIS_B( 0X7F );           // Todas entradas Excepto RB7 -> TX_EXT

   ds1307_init ();             // Se inicializa el ds1307
   delay_ms(1000);

   // Presentacion
   fprintf( COM_EXT, "\n\r *****************************************" );
   fprintf( COM_EXT, "\n\r              DATALOGGER   UTPL  V3     " );
   fprintf( COM_EXT, "\n\r *****************************************" );
   fprintf( COM_EXT, "\n\r    KRADAC Robotics  |  Loja - Ecuador" );
   fprintf( COM_EXT, "\n\r     www.kradac.com  |  Cel: 0991898859" );
   fprintf( COM_EXT, "\n\r             version:  3.1\n\r\n\r" );


   while(1)
   {
      
      // Capturar Trama GPS
      if ( bufferSerial[ i_serial - 2 ] == 0x0D &&
            bufferSerial[ i_serial - 1 ] == 0x0A  ) //  Detecta el ENTER / Carriage Return
      {
      
         for (i = 0; i < BUFF_SER_0; ++i)
         {
         
         // Obtener TRAMA GPS
         if ( bufferSerial[ i + 0 ] == 'G' &&
               bufferSerial[ i + 1 ] == 'P' && 
                bufferSerial[ i + 2 ] == 'R' && 
                 bufferSerial[ i + 3 ] == 'M'  )
         {
               hora1 = bufferSerial[ i + 6 ];
               hora  = bufferSerial[ i + 7 ];

               min1 = bufferSerial[ i + 8 ];
               minu = bufferSerial[ i + 9 ];

               seg1 = bufferSerial[ i + 10 ];
               seg  = bufferSerial[ i + 11 ];

               rtc.hora = hora-48 + ( hora1-48 * 10 );
               rtc.minu = min1-48 + ( minu-48  * 10 );
               rtc.segu = seg -48 + ( seg1-48  * 10 );

               reloj = rtc;
               igualar_rtc( rtc );
               

               fprintf(COM_EXT, "\r\n HORA: %c%c:%c%c:%c%c \r\n",
                                    hora1,hora,
                                      min1, minu,
                                       seg1, seg );
               // Limpiar el Buffer
               for (i = 0; i < BUFF_SER_0; ++i)
                  bufferSerial[i] = 0x00;
               
               i_serial = 0;
            }
         }

      }

      // Enviar Trama al GPS
      if ( _enviar_trama == 1 )
      {
         _enviar_trama = 0;

         ds1307_get_date (day, month, yr, dow);  /// se obtiene la fecha
         ds1307_get_time (hrs, min, sec);
         
         fprintf(COM_UART,"AT$MSGSND=4,\"");
         fprintf(COM_UART, "#PT01,20%d%02d%02d,%02d%02d%02d,%Lu,%Lu,%Lu,%Lu,%Lu,%Lu,000,999$\"\n\r"
                                   yr,month, day, 
                                             hrs, min, sec, 
                                                valor_AN0,valor_AN1,valor_AN2,valor_AN3,valor_AN4,valor_AN5);

         output_high( LED_STATUS );
         delay_ms(250);
         output_low( LED_STATUS );
      }
      
   }
}


void igualar_rtc( Tiempo& rtc )
{
   ds1307_set_date_time(rtc.dia, rtc.mes, rtc.an, rtc.an, rtc.hora, rtc.minu, rtc.segu);
}
