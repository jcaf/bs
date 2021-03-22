22/03/2021

Firmare:
----------
- Nueva estructura de carpetas, "firmware" es la carpeta contenedora. (draft es el histórico)
- El renombra el proyecto a firmware.ino para Arduino, y firmware.cbp para CodeBlocks

BUG FIXED
1) La presentación de la temperatura en el LCD causaba un reset porque no habia suf. espacio en dtostr() para el charbuff[] de salida. Fixed
2) La temperatura estaba muy cerca entre las 2 ktes de "fuera de rango y no conectada", al ser float se compara entre un rango dentro del punto medio
3) La corriente después de serobtenida era truncada a 0, sumado con el Factor de corrección al final.. ahora se contempla antes el F.C
4) La temperatura desde el registro del LTC ahora tiene un umbral menor para que sea considerado como no conectado de ser el caso. Si el máximo
 es 30000, se usa 29700, q corresponde a 2.970V en VREF2
5) Las ktes de temperatura ahora son

#define NTC10K_NO_CONNECTED -600
#define NTC10K_OUT_RANGE -700

6) La lectura del año es corregido para mostrar el digito de mayor peso
 //Year 00..99
    Y10 = (I2Ccfx_UserBuff[6]>>4);// & 0x01; Bug fixed
    Y = I2Ccfx_UserBuff[6] & 0x0F;

- Por mejorar:
Consistencias dividendo y divisor en la funcion de obtener Temperatura. [OK, realizado]
*Pasar las ktes OUT OF RANGE y NO CONNECTED a enteros y la temperatura a float... por definir

Hardware:
1) Se reemplazó el LTC6804 del stack#2 del Master
2) Todos los mini-cktos switching deL LTC3900 estan OK
3) La impedancia del stack#5 del expasión tiene aprox. 0.9K entre 0-5vdc generados por el LTC3900, pero funciona OK


