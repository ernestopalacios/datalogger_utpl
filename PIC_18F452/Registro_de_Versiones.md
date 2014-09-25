Registro de cambios:


25-09-2014
version 3.5.3

- Se encontro que si se hace la petición de trama GPS (AT$GPSRD=10) y se envia un comando en el mismo
  instante (AT$MSGSND) la comunicación serial se corrompe y deja de recibir tramas del SkyPatrol.

- Se encontro que a veces lee mal la hora desde el RTC. Se puso a consultar el GPS del SkyPatrol cada
  67 segundos para evitar la colision con el comando anterior

- Cuando envia mal la hora el Servidor no dibuja la grafica, esto puede provocar que el equipo aparen-
  temente deje de reportar.