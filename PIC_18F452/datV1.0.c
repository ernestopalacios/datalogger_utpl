/**********************************************\
 *       Kradac Robotics & Electronics        *
 *                                            *
 *  Proyecto:     Sensor  UTPL                *
 *  Programador:  Hugo Ramirez  & Ernesto P   *
 *  version:      3.1.1                       *
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
 #define SEG_DAQ         30        // Tiempo de Adquisicion en segundos 
 #define SEG_GPS         3600       // Pedir Trama de GPS para igualar el Reloj cada ciertos segundos

 #define BUFF_SER_0      200       // Tamano del buffer serial para UART0 - Hardware
 #define BUFF_SER_1      100      // Tamano del buffer serial externo - Software



//#define 

// VARIABLES GLOBALES

   char  bufferSerial[ BUFF_SER_0 ];                                // Buffer Serial para UART0

   volatile unsigned int i_serial = 0;
   volatile unsigned int i_timer  = 2;
   
   unsigned int i = 0;           // Contador para lazos FOR()
   unsigned int j = 0;           // Contador para lazos FOR()
   unsigned int k = 0;           // Contador para lazos FOR()
   
   int _ovf_serial_0    = 0;            // Bandera para saber si se sobrepaso el buffer
   int _enviar_trama    = 0;           // Bandera para enviar la trama desde MAIN()
   int _comas           = 0;          // Contador de comas para la trama GPRMC
   int _fecha_ok        = 0;

   unsigned long _segundos = 0;
   unsigned long _segundos_daq = 0;
   unsigned long _segundos_gps = 0;

   
   unsigned int yr     = 0;          // Dos ultimos digitos 20yy
   unsigned int month  = 0;
   unsigned int day    = 0;
   unsigned int hrs    = 0;
   unsigned int min    = 0;
   unsigned int sec    = 0;
   unsigned int dow    = 0;

   // Tiempo desde el GPS
   char c_hora1  = 0;
   char c_hora   = 0;

   char c_min1   = 0;
   char c_minu   = 0;

   char c_seg1   = 0;
   char c_seg    = 0;
   
   // Fecha del GPS
   char c_dia1  = 0;
   char c_dia   = 0;

   char c_mes1   = 0;
   char c_mes    = 0;

   char c_an1   = 0;
   char c_an    = 0;

   // Int Hora y Fecha
   int _hora = 0;
   int _min  = 0;
   int _seg  = 0;
   
   int _dia = 0;
   int _mes = 0;
   int _an  = 0;

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


   // Iguala la hora del PIC con la Hora desde el RTC
   ds1307_get_date( reloj.dia,  reloj.mes,  reloj.an, reloj.an );
   ds1307_get_time( reloj.hora, reloj.minu, reloj.segu );

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
               
               /* Recoje la Hora desde la trama GPRMC */
                  if (bufferSerial[ i + 16 ] == 'A') 
                  {
                     // CHAR
                     c_hora1 = bufferSerial[ i + 6 ];
                     c_hora  = bufferSerial[ i + 7 ];

                     c_min1 = bufferSerial[ i + 8 ];
                     c_minu = bufferSerial[ i + 9 ];

                     c_seg1 = bufferSerial[ i + 10 ];
                     c_seg  = bufferSerial[ i + 11 ];

                     // INT
                     _hora = c_hora - 48 + ( c_hora1-48 ) * 10 ;
                     _min  = c_minu - 48 + ( c_min1 -48 ) * 10 ; 
                     _seg  = c_seg  - 48 + ( c_seg1 -48 ) * 10 ; 

                  }else{

                     fprintf(COM_EXT, "\r\n TRAMA GPS INVALIDA \r\n");
                     
                  }

               /* BUSCAR LA FECHA */
                  _comas = 0;
                  for (j = i; j < BUFF_SER_0; ++j)          // Recorrer el Buffer desde la trama GPRMC
                  {
                     if ( bufferSerial[j] == ',')          // Contar las COMAS ','
                        _comas++;

                     // Recojer la Fecha en la COMA #9
                     if ( _comas == 9 && _fecha_ok == 0 )                  
                     {
                        
                        _fecha_ok = 1;

                        // CHAR
                        c_dia1  = bufferSerial[ j + 1 ];
                        c_dia   = bufferSerial[ j + 2 ];

                        c_mes1  = bufferSerial[ j + 3 ];
                        c_mes   = bufferSerial[ j + 4 ];

                        c_an1   = bufferSerial[ j + 5 ];
                        c_an    = bufferSerial[ j + 6 ];

                        //INT - Hora y Fecha en UTC
                        _dia   = c_dia-48 + ( c_dia1-48 ) * 10 ;
                        _mes   = c_mes-48 + ( c_mes1-48 ) * 10 ;
                        _an    = c_an -48 + ( c_an1 -48 ) * 10 ;


                        /***  AQUI SE RESTAN LAS 5 HORAS DE ECUADOR DE UTC  ***/

                           if ( _hora <= 5 )   // Las primeras cuatro horas del dia
                           {
                              _hora += 24;   // Agregarle un dia
                              _hora -=  5;  // Restarle las 5 horas de ECU-UTC

                              if ( _dia == 1) // Si tambien es el primer dia del mes
                              {
                                 if ( _mes == 1 )  //Ajuste para el 01 Enero de cualquier año
                                 {
                                    _an--; // Regresar al anio anterior por las siguientes 5 horas
                                    _mes = 12;   //Diciembre
                                    _dia = 31;   //31
                                 }
                                 else if ( _mes == 3 ) // Si es marzo y debe volver a febrero // Fuck Leap Year!
                                 {
                                    _dia = 28;
                                    _mes = 2;
                                 }
                                 else if ( _mes % 2 == 1 )  // Es un mes impar
                                 {  
                                    _dia = 30;   // Pasa al dia anterior
                                    _mes--;     // Regresa un mes
                                 
                                 }else{           // Es un mes par
                                    _dia = 31;
                                    _mes--;
                                 }

                              }else{ // Cualquier otro de los demas dias
                                 _dia--;
                              }
                           }else{
                              _hora -= 5;
                           }

                           // Comprueba que la hora sea la adecuada
                           if (_hora < 23 && _min < 60 && _seg < 60)
                           {
                              rtc.hora = _hora;
                              rtc.minu = _min;
                              rtc.segu = _seg ;
                              
                           }

                           // Comprueba que la fecha sea la adecuada. Asume que el anio esta bien
                           if ( _dia < 32 && _mes < 13 )   // Comprueba que la fecha sea adecuada
                           {
                              rtc.dia = _dia;
                              rtc.mes = _mes;
                              rtc.an  = _an;
                           }

                           reloj = rtc;             //Iguala la hora del PIC a la hora del GPS
                           igualar_rtc( rtc );     // Iguala el RTC a la hora del GPS

                           fprintf(COM_EXT, "\r\n HORA:  %02d:%02d:%02d\r\n",
                                                       _hora,_min,_seg );

                           fprintf(COM_EXT,     " FECHA: %02d:%02d:20%02d\r\n",
                                                     _dia,_mes,_an );
                        
                     
                     }
                     
                  }


               // Limpiar el Buffer
               for (j = 0; j < BUFF_SER_0; ++j)
                  bufferSerial[i] = 0x00;
               
               i_serial = 0;
               _fecha_ok = 0; // Listo para obtener nueva fecha

            }
         }

      }

      // Enviar Trama al GPS
      if ( _enviar_trama == 1 )
      {
         _enviar_trama = 0;

         fprintf(COM_UART,"AT$MSGSND=4,\"");
         fprintf(COM_UART, "#PT01,20%d%02d%02d,%02d%02d%02d,%Lu,%Lu,%Lu,%Lu,%Lu,%Lu,000,999$\"\n\r"
                                   reloj.an, reloj.mes, reloj.dia, 
                                             reloj.hora, reloj.minu, reloj.segu, 
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
