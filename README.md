Para la compilacion del programa unicamente se debe ejecutar el comanto make en consola en la direccion donde se hayen los archivos main.cpp cache.cpp cache.h y aligned.trace


Abstracto

Este es un programa para la simulación del comportamiento de los multicaches de 2 procesadores, con protocolo de coherencia MESI.
El principio estructural de los multiprocesadores es el siguiente:

	1. Se tienen 2 procesadores, cada uno con un cache L1 de 8 KB y bloques de 16 B.
	2. Se cuenta ademas con un procesador L2 comun de 64 KB y bloques de 16 B.
	3. Debido a que cada bloque es de 16 B se utilizan 4 bits de byte offset.
	4. El tamano del indice depende del numero de bloques en el cache, asi los bits del indice indican la posicion del bloque en el cache.
	5. El resto de la instruccion que no es byte offset, ni es index, es tag.
	4. La estructura de los caches es mapeo directo.	
	5. El protocolo de coherencia entre los 3 caches es protocolo MESI.

Luego de la simulación se imprime en pantalla el numero de hits y misses encontrados por cada cache. Ademas se determina el estado de una misma instruccion en los diversos cache, obtenida a partir de su respectivo index para cada cache, y se comparan en consola los respectivos estados. Esto se realiza para las primeras 6 intrucciones en ser leidas cuyos indices fueron distintos.


