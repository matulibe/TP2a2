Introduccion:

El codigo presentado es un TDA de una pokedex. Este simula el funcionamineto de una, mediante la lectura de archivos los cuales se los pasa el usuario para avistar o evolucionar pokemones.
Esta tiene que cumplir con las acciones basicas de una pokedex, dar informacion de un pokemon o especie, agregar la informacion de pokemones avistados y agregar la informacion de pokemones
evolucionados, y guardar la informacion de los mismos para que se mantengan en la pokedex(En este caso que se guarden en el archivo correspondiente).


Linea de compilacion:

Nuestra linea de compilacion es: gcc *.c -Wall -Werror -Wconversion -std=c99 -o pokedex

En la cual gcc es nuestro compilador(GNU compiler collection). 

*.c Es el programa que queremos compilar. En este caso el '*' le dice al compilador que compile todos los archivos dentro de la carpeta.

-Wall habilita la advertencias de errores que no evitan que el código corra pero que pueden ser evitados fácilmente. Por ejemplo hacer retornos indebidos en funicones(Devolver un char en una 
funcion que devuelve int por ejemplo) o no hacerlos cuando deberia.

-Werror le avisa al programa que tiene que considerar todas las advertencias como errores.

-Wconversion avisa las conversiones las cuales pueden alterar un valor.

-std=99 le avisa al compilador que versión de C estoy usando. En este caso es la versión de 1999.

-o le indica al compilador el nombre por el cual se va a llamar al programa.

-pokedex es el nombre con el cual se llamara al programa en el momento de ejecutarlo.


Linea de ejecucion:

valgrind --leak-check=full --track-origins=yes --show-reachable=yes ./pokedex

-valgrind valgrind es el programa que usamos para correr el programa ya que crea un entorno seguro para poder ver las perdidas de memoria.

--leak-check=full ya de por si valgrind te avisa si hay pérdida de memoria. Pero si usas esta opción, te dará más información, te avisara en que función se asignó esa memoria perdida
y te dirá si la memoria está perdida definitivamente o si se puede acceder, si se perdió de manera indirecta, si hay una posible pérdida de memoria, o si se pudo suprimir el error.

-track-origins=yes te avisa si hay variables no inicializadas. Sea en el heap o en el stack.

--show-reachable=yes te mostrara los bloques de memoria que todavía son accesibles, los que se perdieron de manera indirecta, los que definitivamente se perdieron y los que posiblemente 
se perdieron.




Modificaciones:


-Se modifico la funcion 'abb_con_cada_elemento' ya que esta acotaba la cantidad de elementos que podia recibir el arbol. En vez de usar un vector en el stack se implementaron 3 funciones 
las cuales recorren el arbol de manera recursiva y ejecutan la funcion recibida.

-A las funiciones 'destruir_nodos' del tda listas y abb se las declaro como funciones estaticas para restringir el ambito de donde se definen y asi evitar que haya problemas al compilar.