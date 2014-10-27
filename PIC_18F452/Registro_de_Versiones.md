Registro de cambios:

Pendientes:
* Identificar si es SkyPatrol+ o anterior
* Habilitar grabar datos en USB

Observaciones:
* El PIC no puede leer y escribir por el puerto serial al mismo tiempo.
* Pruebas iniciales, parece que se debe hacer más seguido el rtc_init(); para evitar lecturas
  incorrectas



--------------------------            25-09-2014         -------------------------------------------

version 3.5.5

- Mejorada la deteccion de colision entre GPS y MSG.

- Si no recibe una trama serial desde el SkyPatrol por mas de 60 segundos resetea el micro por WDT

- Añadida la funcion de envio por SKYPATROL TT8750+. para que funcione se debe compilar con la ban-
  dera  #define TT8750_PLUS


--------------------------            25-09-2014         -------------------------------------------

version 3.5.4

- WDT Habilitado:
  
  Si se cuelga el micro lo reinica 512ms. Resetea a propósito el PIC a una hora determinada del dia.
  (se configura al momento de compilar)   cuando resetea desde WDT, guarda la hora en EEPROM para no
  perder el reloj del pic.

- Detecta si el tiempo de GPS y de MSG son los mismos y le resta dos segundos a la consulta GPS para
  evitar la colision del bus serial. El compilador apaga las interrupciones mientras esta en PRINTF 
  para evitar colgarse, por eso no puede enviar y recibir al mismo tiempo!

- Consulta el RTC una vez al encenderse el PIC, luego lleva internamente el tiempo. Se estan presen-
  tando problemas al momento de leer desde el RTC, da valores de fecha y/o hora incorrectas.

- Reloj Paralelo al RTC corriendo en el PIC, se ajusta cada minuto con trama GPS

- Se pide trama GPS en el segundo 5 de cada segundo.

- Peor caso de escenario: Se reinicia el equipo y no lee bien desde el RTC y no hay señal GPS.
  En ese caso inicia en la hora 01:02:03 sigue intentando obtener la hora correcta desde el RTC.
  Si llega la señal GPS se soluciona el problema, iguala Reloj del PIC y RTC.



---------------------------------      25-09-2014          ------------------------------------------

version 3.5.3

- Se encontro que si se hace la petición de trama GPS (AT$GPSRD=10) y se envia un comando en el mismo
  instante (AT$MSGSND) la comunicación serial se corrompe y deja de recibir tramas del SkyPatrol.

- Se encontro que a veces lee mal la hora desde el RTC. Se puso a consultar el GPS del SkyPatrol cada
  67 segundos para evitar la colision con el comando anterior

- Cuando envia mal la hora el Servidor no dibuja la grafica, esto puede provocar que el equipo aparen-
  temente deje de reportar.
