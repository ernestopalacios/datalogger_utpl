Registro de cambios:

Pendientes:
* Identificar si es SkyPatrol+ o anterior
* Habilitar grabar datos en USB


25-09-2014
version 3.5.4

- Detecta si el tiempo de GPS y de MSG son los mismos y le resta dos segundos a la consulta GPS para
  evitar la colision del bus serial. El compilador apaga las interrupciones mientras esta en PRINTF 
  para evitar colgarse, por eso no puede enviar y recibir al mismo tiempo!

- Consulta el RTC una vez al encenderse el PIC, luego lleva internamente el tiempo. Se estan presen-
  tando problemas al momento de leer desde el RTC, da valores de fecha y/o hora incorrectas.


25-09-2014
version 3.5.3

- Se encontro que si se hace la petición de trama GPS (AT$GPSRD=10) y se envia un comando en el mismo
  instante (AT$MSGSND) la comunicación serial se corrompe y deja de recibir tramas del SkyPatrol.

- Se encontro que a veces lee mal la hora desde el RTC. Se puso a consultar el GPS del SkyPatrol cada
  67 segundos para evitar la colision con el comando anterior

- Cuando envia mal la hora el Servidor no dibuja la grafica, esto puede provocar que el equipo aparen-
  temente deje de reportar.