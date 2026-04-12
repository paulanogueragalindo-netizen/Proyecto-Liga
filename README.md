# Sistema de Liga de Futbol
 
**Estudiante:** Paula Andrea Noguera Galindo
 
---
 
## Descripción del programa
 
Este programa es una aplicación de consola en C++ que permite gestionar una liga de fútbol. Lee la configuración de la liga desde un archivo externo, registra resultados de partidos y guarda toda la información de forma persistente para que no se pierda al cerrar el programa. Calcula automáticamente la tabla de posiciones ordenada por puntos, diferencia de goles y goles a favor, y la actualiza cada vez que se registra o edita un resultado. También permite consultar el historial de jornadas, ver enfrentamientos entre dos equipos específicos, exportar la tabla a un archivo de texto y editar resultados ya registrados. Al volver a ejecutar el programa, la liga retoma exactamente donde quedó.
 
---
 
## Cómo compilar
 
Desde la carpeta donde esta el archivo `main.cpp`, ejecuta este comando:
 
```
g++ main.cpp
```
 
---
 
## Cómo ejecutar
 
Después de compilar, ejecuta:
 
```
./a.exe
```
 
> Asegúrate de que los archivos `config.txt` y `partidos.txt` estén en la carpeta `../data/` relativa al ejecutable.
 
---
 
## Formato de config.txt
 
El archivo `config.txt` debe estar en la carpeta `data/` y seguir este formato:
 
```
liga=<nombre de la liga>
victoria=<puntos por ganar>
empate=<puntos por empatar>
derrota=<puntos por perder>
equipo=<nombre del equipo 1>
equipo=<nombre del equipo 2>

```
 
**Ejemplo:**
 
```
liga=Liga Colombiana
victoria=3
empate=1
derrota=0
equipo=Atletico Nacional
equipo=Junior FC
equipo=Millonarios
equipo=Santa Fe
```
 
Cada línea usa el formato `clave=valor`. Se pueden agregar tantos equipos como se quiera usando una línea `equipo=` por cada uno.
 
---
 
## Decisiones de diseño
 
- **Recálculo dinámico de la tabla:** La tabla se construye desde cero cada vez que se solicita, leyendo `partidos.txt`. Esto garantiza que siempre esté sincronizada con los datos reales, incluso si se edita un resultado anterior.

- **Separación entre partidos y jornadas:** `partidos.txt` guarda los resultados en formato plano para facilitar el cálculo de la tabla, mientras que `fechas.txt` organiza esos mismos partidos en jornadas con marcadores `JORNADA=N` y `FIN_JORNADA` para el historial.

- **Puntos configurables:** Los puntos por victoria, empate y derrota se leen desde `config.txt`, lo que permite adaptar el programa a distintas ligas sin modificar ni recompilar el código.

- **Criterios de desempate:** La tabla se ordena por puntos, luego por diferencia de goles y finalmente por goles a favor, reflejando los criterios reales usados en la mayoría de ligas de fútbol.

- **Validación de partidos duplicados:** Antes de registrar un partido, el programa verifica que ese enfrentamiento no exista ya en la jornada actual, evitando registros repetidos sin bloquear que los mismos equipos se enfrenten en jornadas distintas.

