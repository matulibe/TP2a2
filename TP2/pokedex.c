#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stddef.h>
#include "lista.h"
#include "abb.h"
#include "pokedex.h"

#define LEER "r"
#define ESCRIBIR "w"
#define CAPTURADO 'S'
#define NO_CAPTURADO 'N'
#define ESTADO_1 "Capturado"
#define ESTADO_2 "No Capturado"
#define ESPECIE 'E' 
#define POKEMON 'P'

#define EVOLUCION "%i;%[^;];%i;%[^;];%[^\n]"
#define AVISTAMIENTOS "%i;%[^;];%[^;];%[^;];%i;%c\n"
#define POKE_L "%[^;];%i;%[^\n]"
#define ESPECIE_L "%i;%[^;];%[^\n]"
#define POKE_E "%c;%s;%i;%c\n" 
#define ESPECIE_E "%c;%i;%s;%s\n" 
#define LETRA " %c;"
#define ENTRENADOR "%[^\n]"
#define CANT_E 5
#define CANT_A 6
#define CANT_P 3
#define CANT_L 1

#define POKEDEX "pokedex.txt"

#define MAX 100
#define ERROR -1
#define EXITO 0
#define MENOR -1
#define MAYOR 1
#define IGUAL 0


/*
 *Pre: Recibira dos especies.
 * 
 * Post: Si la primera es mayor de vuelve 1, si es menor -1 y si son
 * iguales devuelve 0.
 */
int comparador(void* entrante, void* nodo_actual){
	if(!entrante || !nodo_actual)
		return 0;
  especie_pokemon_t* especie = (especie_pokemon_t*)nodo_actual;
  especie_pokemon_t* comparado = (especie_pokemon_t*)entrante;

	if(comparado->numero>especie->numero)
		return MAYOR;

	if(comparado->numero<especie->numero)
		return MENOR;

	return IGUAL;
}

//Libera a los pokemones de la lista.
void destruir_pokemones(void* pokemon, void* extra){
  extra=extra;
  free((particular_pokemon_t*)pokemon);
}

//Libera la especie y sus pokemones.
void liberar_especie(especie_pokemon_t* especie){
  if(especie){
    lista_con_cada_elemento(especie->pokemones, destruir_pokemones, NULL);
    lista_destruir(especie->pokemones);
    free(especie);
  }
}

//Libera la especie y sus pokemones.
void destructor(void* especie){
  if(!especie)
    return;
  liberar_especie((especie_pokemon_t*)especie);
  
}

//Reserva memoria para una especie y su lista. Devuelve la especie.
especie_pokemon_t* crear_especie(){
  especie_pokemon_t* aux = calloc(1, sizeof(especie_pokemon_t));
  if(!aux)
    return NULL;
  aux->pokemones = lista_crear();
  if(!aux->pokemones){
    free(aux);
    return NULL;
  }
  return aux;
}


//Reserva memoria para un pokemon. Devuelve el pokemon
particular_pokemon_t* crear_pokemon(){
  particular_pokemon_t* aux = calloc(1, sizeof(particular_pokemon_t));
  if(!aux)
    return NULL;
  return aux;
}



pokedex_t* pokedex_crear(char entrenador[MAX_NOMBRE]){
  if(!entrenador)
    return NULL;

  pokedex_t* aux = calloc(1, sizeof(pokedex_t));
  if(!aux)
    return NULL;
  strcpy(aux->nombre_entrenador, entrenador);
  aux->ultimos_vistos = lista_crear();
  if(!aux->ultimos_vistos){
    free(aux);
    return NULL;
  }
  aux->ultimos_capturados = lista_crear();
  if(!aux->ultimos_capturados){
    free(aux->ultimos_vistos);
    free(aux);
    return NULL;
    }
  aux->pokemones = arbol_crear(comparador, destructor);
  if(!aux->pokemones){
    free(aux->ultimos_capturados);
    free(aux->ultimos_vistos);
    free(aux);
    return NULL;
  }
  return aux;
}

//Destruira las especies ya inseratadas en el arbol las cuales hay que sacar y el pokemon recicido.
void destruir_extras(especie_pokemon_t* especie, particular_pokemon_t* poke){
  if(poke)
    free(poke);
  lista_destruir(especie->pokemones);
  free(especie);
}


/*
 *Pre: Recibira un archivo, una pokedex, una especie, un pokemon y una cantidad de leidos.
 * 
 *Post: Leerá a los pokemones y los insertara en la lista que deba. Si la especie no existe la crea
 *e insertara al pokemon en la lista. 
 */
void asignar_pokemones(FILE * archivo, pokedex_t* pokedex, especie_pokemon_t* especie, particular_pokemon_t* poke, int* leidos){
  especie_pokemon_t* buscador = NULL;
  char capturado;
  *leidos = fscanf(archivo, AVISTAMIENTOS, &especie->numero, especie->nombre, especie->descripcion, poke->nombre, &poke->nivel, &capturado);
  if(*leidos == CANT_A){
    if(capturado == CAPTURADO)
      poke->capturado = true;
    buscador = (especie_pokemon_t*)(arbol_buscar(pokedex->pokemones, especie));
    if(buscador == NULL){
        arbol_insertar(pokedex->pokemones, especie);
        buscador = especie;
      }else{
        destruir_extras(especie, NULL);
      }
      lista_insertar(buscador->pokemones, poke);
      if(capturado == CAPTURADO)
        lista_apilar(pokedex->ultimos_capturados, poke);
      lista_encolar(pokedex->ultimos_vistos, poke);
    }else{
      destruir_extras(especie, poke);
    }
}

int pokedex_avistar(pokedex_t* pokedex, char ruta_archivo[MAX_RUTA]){
  FILE * archivo = fopen(ruta_archivo, LEER);
  if(!archivo)
    return ERROR;
  particular_pokemon_t* poke = NULL;
  especie_pokemon_t* especie = NULL;
  int leidos = 0, retorno = EXITO;
  while(leidos != EOF){
    poke = crear_pokemon();
    if(!poke){
      fclose(archivo);
      return ERROR;
    }
    especie = crear_especie();
    if(!especie){
      fclose(archivo);
      free(poke);
      return ERROR;
    }
    asignar_pokemones(archivo, pokedex, especie, poke, &leidos);
  }
  fclose(archivo);
  return retorno;
}

void destruir_insertado_recientemente(pokedex_t* pokedex, especie_pokemon_t* especie, particular_pokemon_t* poke){
  free(poke);
  arbol_borrar(pokedex->pokemones, especie);
}

/*
 *Pre: Recibira la pokedex, una especie viejan una evolucionada, un pokemon y un bool.
 *
 *Post: Cambaira de lista al pokemon recibido y devolvera 0. En el caso de que no exista o no este capturado liberara la memoria que necesite
 *y devolvera -1  
 */
int cambiar_evolucionado(pokedex_t* pokedex, especie_pokemon_t* buscador, especie_pokemon_t* especie, particular_pokemon_t* poke_evol, bool reciente){
  int pos = 0, retorno = EXITO;
  bool encontrado = false;
  lista_iterador_t* iterador = lista_iterador_crear(buscador->pokemones);
  particular_pokemon_t* aux = NULL;
  while(!encontrado && lista_iterador_tiene_siguiente(iterador)){
    aux = (particular_pokemon_t*)lista_iterador_siguiente(iterador); 
    if(strcmp(aux->nombre, poke_evol->nombre)==IGUAL)
      encontrado = true;
    if(!encontrado)
      pos++;
  }
  lista_iterador_destruir(iterador);
  if(encontrado && aux->capturado == true){
    free(poke_evol);
    lista_borrar_de_posicion(buscador->pokemones, (size_t)(pos));
    lista_insertar(especie->pokemones, aux);
  }else{
    if(reciente == true)
      destruir_insertado_recientemente(pokedex, especie, poke_evol);
    else
      free(poke_evol);
    retorno = ERROR;
  }
  return retorno;
}

/*
 *Pre: Recibira un archivo, una pokedex, una especie, un pokemon, un entero de retorno y un entero de leidos
 *
 *Post: Leera la informacion de los pokemones. Creara la especie nueva si es necesario y cambiara de lista a los pokemones 
 *y liberara la memeoria que ya no sirva. Devuelve 0 o -1.
 */
int reordenar_pokedex(FILE * archivo, pokedex_t* pokedex, especie_pokemon_t* evolucion, particular_pokemon_t* poke_evol, int* retorno, int* leidos){
  int numero_poke, aux;
  bool reciente = false; //Para chequear si la especie se creo recientemente o no.
  *leidos = fscanf(archivo, EVOLUCION, &numero_poke, poke_evol->nombre, &evolucion->numero, evolucion->nombre, evolucion->descripcion);
  if(*leidos == CANT_E){
    especie_pokemon_t* especie = (especie_pokemon_t*)(arbol_buscar(pokedex->pokemones, evolucion));
    if(!especie){
      especie = evolucion;
      arbol_insertar(pokedex->pokemones, especie);
      reciente = true;
    }else
      destruir_extras(evolucion, NULL);
    especie_pokemon_t* poke = crear_especie();
    if(!poke){
      fclose(archivo);
      if(reciente)
        destruir_insertado_recientemente(pokedex, evolucion, poke_evol);
      else
        free(poke_evol);
      return ERROR;
    }
    poke->numero = numero_poke;
    especie_pokemon_t* buscador = (especie_pokemon_t*)(arbol_buscar(pokedex->pokemones, poke));
    destruir_extras(poke, NULL);
    if(!buscador){
      if(!reciente)
        destruir_extras(evolucion, poke_evol);
      else
        destruir_insertado_recientemente(pokedex, evolucion, poke_evol);
      
    }else{
      aux = cambiar_evolucionado(pokedex, buscador, especie, poke_evol, reciente);
      if(aux == ERROR && *retorno != ERROR)
        *retorno = ERROR;
    }
  }else
    destruir_extras(evolucion, poke_evol);
  reciente = false;
  return *retorno;
}


int pokedex_evolucionar(pokedex_t* pokedex, char ruta_archivo[MAX_RUTA]){
  if(!pokedex || !ruta_archivo)
    return ERROR;
  FILE * archivo = fopen(ruta_archivo, LEER);
  if(!archivo)
    return ERROR;
  int retorno = EXITO;
  int leidos = 0;
  particular_pokemon_t* poke_evol = NULL;
  especie_pokemon_t* evolucion = NULL;
  while(leidos != EOF){
    poke_evol = crear_pokemon();
    if(!poke_evol){
      fclose(archivo);
      return  ERROR;
    }
    evolucion = crear_especie();
    if(!evolucion){
      fclose(archivo);
      free(poke_evol);
      return ERROR;
    }
    retorno = reordenar_pokedex(archivo, pokedex, evolucion, poke_evol, &retorno, &leidos);
  }
  fclose(archivo);
  return retorno;
}


void pokedex_ultimos_capturados(pokedex_t* pokedex){
  if(!pokedex)
    return;
  if(lista_vacia(pokedex->ultimos_capturados)){
    printf("No se capturaron pokemones recientemente\n");
    return;
  }
  particular_pokemon_t* aux = NULL;
  while(!lista_vacia(pokedex->ultimos_capturados)){
    aux = (particular_pokemon_t*)lista_tope(pokedex->ultimos_capturados);
    printf("%s\n", aux->nombre);
    lista_desapilar(pokedex->ultimos_capturados);
  }
}

void pokedex_ultimos_vistos(pokedex_t* pokedex){
   if(!pokedex)
    return;
  if(lista_vacia(pokedex->ultimos_vistos)){
    printf("No se avistaron pokemones recientemente\n");
    return;
  }
  particular_pokemon_t* aux = NULL;
  while(!lista_vacia(pokedex->ultimos_vistos)){
    aux = (particular_pokemon_t*)lista_primero(pokedex->ultimos_vistos);
    printf("%s\n", aux->nombre);
    lista_desencolar(pokedex->ultimos_vistos);
  }
}


/*
 *Pre: Recibira un pokemon.
 * 
 * Post: Imprimira la informacion del pokemon.
 */
void listar_pokemones(void* pokemon, void* extra){
  particular_pokemon_t* poke = (particular_pokemon_t*)pokemon;
  printf("\tNombre:%s \tNivel:%i \tEstado:%s\n", poke->nombre, poke->nivel, poke->capturado==true?ESTADO_1:ESTADO_2);
}


/*
 *Pre: Recibira una especie.
 * 
 *Post: Imprimira la informacion de la especie y de sus pokemones.
 */
void listar_especie(especie_pokemon_t* especie){
  printf("Especie: %s\n Descripcion: %s\n", especie->nombre, especie->descripcion);
  lista_con_cada_elemento(especie->pokemones, listar_pokemones, NULL);
}


/*
 *Pre: Recibira un pokemon y su especie.
 * 
 * Post: Imprimira la informacion del pokemon y su especie.
 */

void imprimir_informacion(especie_pokemon_t* especie, particular_pokemon_t* pokemon){
  printf("Pokemon: %s\n", pokemon->nombre);
  printf("\tEstado: %s\n", pokemon->capturado==true?ESTADO_1:ESTADO_2);
  printf("\tNivel: %i\n", pokemon->nivel);
  printf("\tEpecie: %s\n", especie->nombre);
  printf("\tDescripcion: %s\n", especie->descripcion);
}



/*
 *Pre: Recibira una especicie, un nombre y un pokemon vacio.
 *
 * Post: Bucara al pokemon en la lista, si lo encuentra devolvera true y el pokemon 
 * auxiliar apuntara al pokemon buscaso. Si no existe devolvera fasle.
 */
bool buscar_poke(especie_pokemon_t* especie, char nombre_pokemon[], particular_pokemon_t* pokemon){
  if(!especie)
    return false;
  lista_iterador_t* iterador = lista_iterador_crear(especie->pokemones);
  bool encontro = false;
  while(lista_iterador_tiene_siguiente(iterador) && !encontro){
    pokemon = (particular_pokemon_t*)lista_iterador_siguiente(iterador);
    if(strcmp(nombre_pokemon, pokemon->nombre)==IGUAL)
      encontro = true;
  }
  if(encontro)
    imprimir_informacion(especie, pokemon);
  lista_iterador_destruir(iterador);
  return encontro;
}




void pokedex_informacion(pokedex_t* pokedex, int numero_pokemon, char nombre_pokemon[MAX_NOMBRE]){
  if(!pokedex || !numero_pokemon)
    return;
  especie_pokemon_t* aux = crear_especie();
  aux->numero = numero_pokemon;
  especie_pokemon_t* especie = (especie_pokemon_t*)arbol_buscar(pokedex->pokemones, aux);
  if(!especie){
    printf("No se podido encontrar la especie pokemon\n");
    destruir_extras(aux, NULL);
    return;
  }
  if(!nombre_pokemon){
    listar_especie(especie);
    destruir_extras(aux, NULL);
    return;
  }
  particular_pokemon_t* pokemon = NULL;
  bool encontro = buscar_poke(especie, nombre_pokemon, pokemon);
  if(!encontro){
    printf("El pokemon buscado no pudo ser localizado\n");
    destruir_extras(aux, NULL);
    return;
  }
  printf("\n");
  destruir_extras(aux, NULL);
}





void pokedex_destruir(pokedex_t* pokedex){
  arbol_destruir(pokedex->pokemones);
  lista_destruir(pokedex->ultimos_capturados);
  lista_destruir(pokedex->ultimos_vistos);
  free(pokedex);
}





/*
 *Pre: Recibira un pokemon y un archivo.
 *
 * Post: Escribira la informacion del pokemon en el archivo para guardar sus datos en la 
 * pokedex.
 */
void escribir_pokemones(void* pokemon, void* extra){
  FILE * archivo = (FILE *)extra;
  particular_pokemon_t* poke = (particular_pokemon_t*)pokemon;
  fprintf(archivo, POKE_E, POKEMON, poke->nombre, poke->nivel, poke->capturado==true?CAPTURADO:NO_CAPTURADO);
}


/*
 *Pre: Recibira una especie y un archivo.
 *
 * Post: Guardara la informacion de la especie y sus pokemones y las escribira en el 
 * archivo para guardarlas. Lo hara ordenadamente para que no se pierda el orden del arbol.
 */
bool escribir_especies(void* pokemon, void* extra){
  FILE * archivo = (FILE *)extra;
  especie_pokemon_t* especie = (especie_pokemon_t*)pokemon;
  fprintf(archivo, ESPECIE_E, ESPECIE, especie->numero, especie->nombre, especie->descripcion);
  lista_con_cada_elemento(especie->pokemones, escribir_pokemones, archivo);
  return false;
}


int pokedex_apagar(pokedex_t* pokedex){
  if(!pokedex)
    return ERROR;
  FILE * archivo = fopen(POKEDEX, ESCRIBIR);
  if(archivo == NULL){
    return  ERROR;
  }
  fprintf(archivo, "%s\n", pokedex->nombre_entrenador);
  abb_con_cada_elemento(pokedex->pokemones, ABB_RECORRER_PREORDEN, escribir_especies, archivo);
  fclose(archivo);
  return EXITO;
}


/*
 * Pre: Recibira el archivo y la pokedex ya inicializada
 * 
 * Post: Leerá el archivo e ira reservando memoria para las especies y las insertara en el arbol de la 
 * pokedex. Y tambien leerá los pokemones y los insertara en la lista de su especie.
 */
void iniciador(FILE * archivo, pokedex_t* pokedex){
  char letra, capturado; 
  int cant_leidos = 0;
  especie_pokemon_t* especie = NULL;
  particular_pokemon_t* poke = NULL;
  while(cant_leidos != EOF){
    cant_leidos = fscanf(archivo, LETRA, &letra);
    if(letra == ESPECIE){
      especie = crear_especie();
      cant_leidos = fscanf(archivo, ESPECIE_L, &especie->numero, especie->nombre, especie->descripcion);
      if(cant_leidos == CANT_P)
        arbol_insertar(pokedex->pokemones, especie);
      else
        destruir_extras(especie, NULL);
    }else if(letra == POKEMON){
      poke = crear_pokemon();
      cant_leidos = fscanf(archivo, POKE_L, poke->nombre, &poke->nivel, &capturado);
      if(capturado == CAPTURADO)
        poke->capturado = true;
      if(cant_leidos == CANT_P)
        lista_insertar(especie->pokemones, poke);
      else
        free(poke);
    }
  }
}



pokedex_t* pokedex_prender(){
  FILE * archivo = fopen(POKEDEX, LEER);
  if(!archivo){
    printf("No se pudo abrir el archivo\n");
    return NULL;
  }
  char entrenador[MAX_NOMBRE];
  int cant_leidos;
  cant_leidos = fscanf(archivo, ENTRENADOR, entrenador);
  if(cant_leidos != 1){
    printf("No hay entrenador\n");
    return NULL;
  }
  pokedex_t* pokedex = pokedex_crear(entrenador);
  if(!pokedex)
    return NULL;
  iniciador(archivo, pokedex);
  fclose(archivo);
  return pokedex; 
}