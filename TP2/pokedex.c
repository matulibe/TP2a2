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
#define ESPECIE 'E' 
#define POKEMON 'P'

#define EVOLUCION "%i;%[^;];%i;%[^;];%[^;]\n"
#define AVISTAMIENTOS "%i;%[^;];%[^;];%[^;];%i;%c\n"
#define POKE_L "%c;%[^;];%i;%c\n"
#define ESPECIE_L "%c;%i;%[^;];%[^;]\n"
#define POKE_E "%c;%s;%i;%c\n" //P;nombre_pokemon;nivel;capturado(S/N
#define ESPECIE_L "%c;%i;%s;%s\n" //E;numero_pokemon;nombre_especie; descripción_especie
#define CANT_E 6
#define CANT_A 6
#define CANT_P 4

#define POKEDEX "pokedex.txt"

#define MAX 100
#define ERROR -1
#define EXITO 0
#define MENOR -1
#define MAYOR 1
#define IGUAL 0


int comparador(void* entrante, void* nodo_actual){
	if(!entrante || !nodo_actual)
		return 0;
  especie_pokemon_t* especie = (especie_pokemon_t*)(nodo_actual);

	if(*((int*)entrante)>(especie->numero))
		return MAYOR;

	if(*((int*)entrante)<(especie->numero))
		return MENOR;

	return IGUAL;
}


void liberar_especie(especie_pokemon_t* especie){
  if(especie){
    free(especie);
  }
}

void destructor(void* especie){
  if(!especie)
    return;
  liberar_especie((especie_pokemon_t*)especie);
  
}


pokedex_t* pokedex_crear(char entrenador[MAX_NOMBRE]){
  if(!entrenador)
    return NULL;

  pokedex_t* aux = calloc(1, sizeof(pokedex_t));
  if(!aux)
    return NULL;
  strcpy(aux->nombre_entrenador, entrenador);
  aux->ultimos_vistos = lista_crear();
  if(aux->ultimos_vistos){
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


int pokedex_avistar(pokedex_t* pokedex, char ruta_archivo[MAX_RUTA]){
  FILE * archivo = fopen(ruta_archivo, LEER);
  particular_pokemon_t* poke = calloc(1,sizeof(particular_pokemon_t));
  if(!poke)
    return ERROR;
  especie_pokemon_t* especie = NULL;
  char capturado;
  int leidos = 0;
  while(leidos != EOF){//numero_pokemon;nombre_especie; descripción_especie;nombre_pokemon;nivel;capturado(S/N)
    fscanf(archivo, AVISTAMIENTOS,&especie->numero, especie->nombre, especie->descripcion,poke->nombre, &poke->nivel, &capturado);
    especie_pokemon_t* buscador = (especie_pokemon_t*)(arbol_buscar(pokedex->pokemones, &especie->numero));
    if(!buscador){
      especie_pokemon_t* especie_aux = calloc(1, sizeof(especie_pokemon_t));
      if(!especie_aux)
        return  ERROR;
      especie_aux = especie;
      arbol_insertar(pokedex->pokemones, (void*)especie_aux);
      especie_aux->pokemones = lista_crear();
      buscador = (especie_pokemon_t*)(arbol_buscar(pokedex->pokemones, &especie->numero));
    }
    lista_insertar(buscador->pokemones, (void*)poke);
    if(capturado == CAPTURADO)
      lista_apilar(pokedex->ultimos_capturados, (void*)poke);
    lista_encolar(pokedex->ultimos_vistos, (void*)poke);
  }
  fclose(archivo);
  return EXITO;
}


int pokedex_evolucionar(pokedex_t* pokedex, char ruta_archivo[MAX_RUTA]){
  if(!pokedex || ruta_archivo)
    return ERROR;
  FILE * archivo = fopen(ruta_archivo, LEER);
  int numero_poke;
  particular_pokemon_t* poke_evol = calloc(1, sizeof(particular_pokemon_t));
  if(!poke_evol)
    return  ERROR;
  especie_pokemon_t* evolucion = NULL;
  int leidos = 0;
  while(leidos != EOF){
    fscanf(archivo, EVOLUCION, &numero_poke, poke_evol->nombre, &evolucion->numero, evolucion->nombre, evolucion->descripcion);
    especie_pokemon_t* especie = (especie_pokemon_t*)(arbol_buscar(pokedex->pokemones, &evolucion->numero));
    if(!especie){
      especie = calloc(1, sizeof(especie_pokemon_t));
      strcpy(especie->nombre, evolucion->nombre);
      especie->numero = evolucion->numero;
      strcpy(especie->descripcion, evolucion->descripcion);
      arbol_insertar(pokedex->pokemones, (void*)especie);
    }
    especie_pokemon_t* buscador = (especie_pokemon_t*)(arbol_buscar(pokedex->pokemones, &numero_poke));
    int pos = 0;
    lista_iterador_t* iterador = lista_iterador_crear(buscador->pokemones);
    particular_pokemon_t* aux = lista_iterador_siguiente(iterador);
    while(strcmp(aux->nombre, poke_evol->nombre)!=IGUAL && lista_iterador_tiene_siguiente(iterador)){
      aux = (particular_pokemon_t*)lista_iterador_siguiente(iterador);
      pos++;
    }
    lista_iterador_destruir(iterador);
    if(aux->capturado == true){
      poke_evol->nivel = aux->nivel;
      poke_evol->capturado = true;
      lista_borrar_de_posicion(buscador->pokemones, (size_t)pos);
      lista_insertar(especie->pokemones, (void*)poke_evol);
    }
    
  }
  fclose(archivo);
  return EXITO;
}


void pokedex_ultimos_capturados(pokedex_t* pokedex){
  if(!pokedex)
    return;
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
  particular_pokemon_t* aux = NULL;
  while(!lista_vacia(pokedex->ultimos_vistos)){
    aux = (particular_pokemon_t*)lista_primero(pokedex->ultimos_vistos);
    printf("%s\n", aux->nombre);
    lista_desencolar(pokedex->ultimos_vistos);
  }
}

void pokedex_informacion(pokedex_t* pokedex, int numero_pokemon, char nombre_pokemon[MAX_NOMBRE]){
  if(!pokedex || !numero_pokemon || nombre_pokemon)
    return;

  especie_pokemon_t* especie = (especie_pokemon_t*)arbol_buscar(pokedex->pokemones, &numero_pokemon);
  if(!especie){
    printf("No se podido encontrar la especie pokemon\n");
    return;
  }
  lista_iterador_t* iterador = lista_iterador_crear(especie->pokemones);
  particular_pokemon_t* pokemon = NULL;
  while(strcmp(nombre_pokemon, pokemon->nombre)!=IGUAL && lista_iterador_tiene_siguiente(iterador)){
    pokemon = (particular_pokemon_t*)lista_iterador_siguiente(iterador);
  }
  lista_iterador_destruir(iterador);
  if(strcmp(nombre_pokemon, pokemon->nombre)!=IGUAL){
    printf("El pokemon buscado no pudo ser localizado\n");
    return;
  }
  printf("Pokemon: %s\n", pokemon->nombre);
  printf("\tEstado: %s\n", pokemon->capturado==true?"Capturado":"No Capturado");
  printf("\tNivel: %i\n", pokemon->nivel);
  printf("\tEpecie: %s\n", especie->nombre);
  printf("\tDescripcion: %s\n", especie->nombre);
}


bool destruir_pokemones(void* elemento, void* extra){
  if(!elemento)
    return false;
  extra = extra;
  especie_pokemon_t* especie = (especie_pokemon_t*)elemento;
  lista_destruir(especie->pokemones);
  return false;
}

void pokedex_destruir(pokedex_t* pokedex){
  abb_con_cada_elemento(pokedex->pokemones, ABB_RECORRER_PREORDEN, destruir_pokemones, NULL);
  arbol_destruir(pokedex->pokemones);
  lista_destruir(pokedex->ultimos_capturados);
  lista_destruir(pokedex->ultimos_vistos);
  free(pokedex);
}



void guardar_especie(FILE * archivo, especie_pokemon_t* especie){//P;nombre_pokemon;nivel;capturado(S/N
  char letra_especie = 'E';
  char letra_pokemones = 'P';
  char capturado = 'N';
  fprintf(archivo, ESPECIE_L, letra_especie, especie->numero, especie->nombre, especie->descripcion);//E;numero_pokemon;nombre_especie; descripción_especie
  lista_iterador_t* iterador = lista_iterador_crear(especie->pokemones);
  particular_pokemon_t* aux = NULL;
  while(lista_iterador_tiene_siguiente(iterador)){
    aux = (particular_pokemon_t*)lista_iterador_siguiente(iterador);
    if(aux->capturado)
      capturado = 'S';
    else
      capturado = 'N';

    fprintf(archivo, POKE_E, letra_pokemones, aux->nombre, aux->nivel, capturado);
  }
}


bool contar_elementos(void* elemento, void* cantidad){
  if(elemento)
    (*(int*)cantidad)+=1;  
  return false;
}

void guardar_vector(nodo_abb_t* nodo, int posicion, especie_pokemon_t* especies){
  if(!nodo)
    return;
  especies[posicion] = (*(especie_pokemon_t*)nodo->elemento);
  guardar_vector(nodo->izquierda, posicion+1, especies);
  guardar_vector(nodo->derecha, posicion+2, especies);
}

void escribir_archivo(FILE * archivo, especie_pokemon_t* especies, int cantidad){
    if(!especies)
      return;
    int i = 0;
    char especie = 'E';
    char pokemon = 'P';
    while(i < cantidad){
      especie_pokemon_t* aux = &especies[i];
      lista_iterador_t* pokemones = lista_iterador_crear(aux->pokemones);
      if(!pokemones)
        return;
      fprintf(archivo, ESPECIE_L, especie, aux->numero, aux->nombre, aux->descripcion);
      while(lista_iterador_tiene_siguiente(pokemones)){
        particular_pokemon_t* poke = (particular_pokemon_t*)lista_iterador_siguiente(pokemones);
        fprintf(archivo, POKE_E, pokemon, poke->nombre, poke->nivel, poke->capturado==true?CAPTURADO:NO_CAPTURADO);
      }
      i++;
    }
}

int pokedex_apagar(pokedex_t* pokedex){
  if(!pokedex)
    return ERROR;
  int cantidad = 0;
  int posicion = 0;
  abb_con_cada_elemento(pokedex->pokemones, ABB_RECORRER_PREORDEN, contar_elementos, &cantidad);
  especie_pokemon_t* especies = malloc(sizeof(especie_pokemon_t)*(unsigned int)cantidad);
  if(!especies)
    return ERROR;
  guardar_vector(pokedex->pokemones->nodo_raiz, posicion, especies);
  FILE * archivo = fopen(POKEDEX, ESCRIBIR);
  if(!archivo){
    free(especies);
    return  ERROR;
  }
  fprintf(archivo, "%s\n", pokedex->nombre_entrenador);
  escribir_archivo(archivo, especies, cantidad);
  free(especies);
  fclose(archivo);
  return EXITO;
}


void iniciador(FILE * archivo, pokedex_t* pokedex){
  char letra_especie = 'E';
  char letra_poke = 'P';
  int cant_leidos = 0;
  while(cant_leidos != EOF){
    especie_pokemon_t* especie = calloc(1,sizeof(especie_pokemon_t));
    especie->pokemones = lista_crear();
    cant_leidos = fscanf(archivo, ESPECIE_L, &letra_especie, &especie->numero, especie->nombre, especie->descripcion);
    if(letra_especie != ESPECIE || cant_leidos != CANT_P)
      return NULL;
    especie->pokemones = lista_crear();
    while(letra_poke =)
  }

}


pokedex_t* pokedex_prender(){
  FILE * archivo = fopen(POKEDEX, LEER);
  if(!archivo)
    return NULL;
  char entrenador[MAX];
  fscanf(archivo, "%s\n", entrenador);
  pokedex_t* pokedex = pokedex_crear(entrenador);
  if(!pokedex)
    return NULL;
  iniciador(archivo, pokedex);
  return pokedex; 
}




