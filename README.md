# sistope-lab1
Laboratorio 1 de Sistemas Operativos

## Enunciado

En este laboratorio crearemos un "reconocedor", que en base a una expresión regular y a un archivo de texto, diga si cada expresión es una expresión regular o no .

La expresión regular a reconocer es:
```
(A|C|G)∗G+T+C(A|C|G|T)∗
```

El siguiente es una ejemplo de salida:

```txt
GATCTTATATAACTGTGAGATTAATCTCAGATAATGACACAAAATATAGTGAAGTTGGTA no
AGTTATTTAGTAAAGCTCATGAAAATTGTGCCCTCCATTCCCATATAATTTATTAATTGT no
CTAGGAACTTCCACATACATTGCCTCAATTTATCTTTCAACAACTTGTGTGTTATATTTT no
GGAATACAGATACAAAGTTATTATGCTTTCAAAATATTCTTTTGCTAATTCTTAGAACAA no
AGAAAGGCATAAATATATTAGTATTTGTGTACATCTGTTCCTTCCTGTGTGACCCTAAGT no
TTAGTAGAAGAAAGGAGAGAAAATATAGCCTAGCTTATAAATTTAAAAAAAAATTTATTT no
GGTCCATTTTGTGAAAAACATAAAAAAAGAACTGTCACATCTTAATTTAAAAAATATATG si
CTTAGTGGTAAGGAGATATATGTCAACTTTTAAGAGGTTGAAAAACAAACGCCTCCCATT no
ATAAGTTTATACTTCACCTCCCACCACTATAACAACCCAGAATCCATGAGGGCATTATCA no

Total de expresiones que Si son regulares:1
Total de expresiones que No son regulares:8
Total de lineas leídas:9
Total de lineas procesadas:9
[Thread #01] Total de expresiones leídas:0
[Thread #01] Total de expresiones procesadas:0
[Thread #02] Total de expresiones leídas:9
[Thread #02] Total de expresiones procesadas:9
[Thread #03] Total de expresiones leídas:0
[Thread #03] Total de expresiones procesadas:0
```

## Lógica de la solución

En este laboratorio crearemos un "reconocedor", que en base a una expresión regular y a un archivo de texto, diga si cada expresión es una expresión regular o no.

1. El proceso principal (hebra madre) recibirá por línea de comandos el nombre del archivo de entrada, el nombre del archivo de salida, el tamaño del chunk que leerá cada hebra y la cantidad de hebras que deberán ser generadas por la hebra madre para realizar los cálculos necesarios.
2. La hebra madre solo debe abrir el archivo de entrada y las hebras hijas deberán leer el archivo.
3. Los puntos de la sección cálculos necesarios deben ser obtenidos por las hebras hijas. Por otro lado, la hebra madre debe esperar a que todas ellas terminen su trabajo.
4. Cuando una hebra esta libre, trata de leer chunk líneas del archivo de entrada. Este acceso debe ser exclusivo, es decir, solo una hebra hija puede estar leyendo el archivo a la vez.
5. Una vez que la hebra lee el chunk, esta se encarga de actualizar los datos para cada expresión correspondiente, para lo cual debe existir una o varias estructuras compartidas las cuales registren los datos de las expresiones.
6. El acceso a estas estructuras compartidas también debe ser exclusivo. Note que es muy probable que varias hebras estén actualizando estas estructuras a la vez.
7. Una vez que una hebra termina de actualizar los datos para una línea especifica, esta vuelve a intentar leer chunk líneas del archivo de entrada. Esto se realiza hasta que no se puedan leer más líneas.
8. Como el número de hebras no necesariamente divide al número de líneas del archivo de entrada, ocurrirá el caso de que una hebra leerá menos líneas que chunk cuando llegue al final del archivo. También existe la posibilidad de que una hebra intente leer y se encuentre con el final del archivo. La solución para esta problemática debe ser implementada por usted.
9. En caso de que se emplee el flag -b, se debe mostrar por la terminal los resultados del laboratorio.
10. Finalmente, la hebra madre escribe los resultados con el mismo formato solicitado en el laboratorio 1 y el laboratorio 2.

## Ejecución

El programa se ejecutará usando los siguientes argumentos (ejemplo):
```
$ ./lab1 -i datos_juegos.csv -o salida.txt -b
```
- `-i`: nombre del archivo de entrada.
- `-o`: nombre de archivo de salida.
- `-n`: numero de hebras a generar.
- `-c`: tamaño del chunk.
- `-b`: bandera o flag que permite indicar si se quiere ver por consola la cantidad de expresiones regulares encontradas.

Como requerimientos no funcionales, se exige lo siguiente:
- Debe funcionar en sistemas operativos con kernel Linux.
- Debe ser implementado en lenguaje de programación C.
- Se debe utilizar un archivo Makefile para compilar los distintos targets.
- Realizar el programa utilizando buenas prácticas, dado que este laboratorio no contiene manual de usuario ni informe, es necesario que todo esté debidamente comentado.
- Los programas se encuentren desacoplados, es decir, que se desarrollen las funciones correspondientes en otro archivo .c para mayor entendimiento de la ejecución.
- La solución debe implementar:
    1. `pthread_create()`
    2. `pthread_join()`
    3. `pthread_mutex_init()`
    4. `pthread_mutex_lock()`
    5. `pthread_mutex_unlock()`
