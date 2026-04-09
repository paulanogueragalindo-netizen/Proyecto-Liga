# Sistema de Liga de Futbol
 
**Estudiante:** Paula Andrea Noguera Galindo
 
---
 
## Descripcion del programa
 
Este programa es una aplicación de consola en C++ que permite gestionar una liga de futbol. Lee la configuracion de la liga (nombre, puntos y equipos) desde un archivo externo, permite registrar resultados de partidos y guarda esa informacion para que no se pierda al cerrar el programa. Tambien permite ver el historial de todos los partidos jugados. Al volver a ejecutar el programa, la liga retoma exactamente donde quedo.
 
---
 
## Como compilar
 
Desde la carpeta donde esta el archivo `main.cpp`, ejecuta este comando:
 
```
g++ main.cpp
```
 
---
 
## Como ejecutar
 
Despues de compilar, ejecuta:
 
```
./a.exe
```
 
> Asegurate de que los archivos `config.txt` y `partidos.txt` esten en la carpeta `../data/` relativa al ejecutable.
 
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
 
Cada linea usa el formato `clave=valor`. Se pueden agregar tantos equipos como se quiera usando una linea `equipo=` por cada uno.
 
---
 
## Decisiones de diseño
 
- **Archivos externos para la configuracion:** Los datos de la liga (nombre, puntos, equipos) se leen desde `config.txt` para que el programa sea flexible y no haya que recompilar si se cambia algo.
 
- **Persistencia con archivo de texto:** Los partidos se guardan en `partidos.txt` usando el modo de escritura `append` (`ios::app`), lo que permite que cada resultado nuevo se agregue sin borrar los anteriores. Asi el historial se mantiene aunque se cierre el programa.
 
- **Validacion de equipos:** Antes de registrar un partido, el programa verifica que los dos equipos ingresados existan en la lista cargada desde `config.txt`. Si alguno no existe, el registro se cancela.
 
- **Menu en bucle:** El programa usa un `do-while` para que el usuario pueda realizar varias acciones sin necesidad de volver a ejecutarlo.
 
- **Separacion de funciones:** La logica se divide en funciones independientes (`cargar_configuracion`, `registrar_equipo`, `ver_partidos`) para que el codigo sea mas facil de leer y mantener.