#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stddef.h>
#include "lista.h"
#include "abb.h"
#include "pokedex.h"

#define ABRIR "a+"
#define LEER "r"
#define ESCRIBIR "w"
#define CAPTURADO 'S'
#define NO_CAPTURADO 'N'

#define FORMATO_E "%i;%[^;];%i;%[^;];%[^;]\n"
#define FORMATO_A  "%i;%[^;];%[^;];%[^;];%i;%c\n"
#define FORMATO_P_P_L "%c;%[^;];%i;%c\n"
#define FORMATO_P_E_L "%c;%i;%[^;];%[^;]\n"
#define FORMATO_P_P_E "%c;%s;%i;%c\n" //P;nombre_pokemon;nivel;capturado(S/N
#define FORMATO_P_E_E "%c;%i;%s;%s\n" //E;numero_pokemon;nombre_especie; descripción_especie
#define CANT_E 6
#define CANT_A 6
#define CANT_P_P 4
#define CANT_P_E 4

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
    fscanf(archivo, FORMATO_A, &especie->numero, especie->nombre, especie->descripcion,poke->nombre, &poke->nivel, &capturado);
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
    fscanf(archivo, FORMATO_E, &numero_poke, poke_evol->nombre, &evolucion->numero, evolucion->nombre, evolucion->descripcion);
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
  fprintf(archivo, FORMATO_P_E_E, letra_especie, especie->numero, especie->nombre, especie->descripcion);//E;numero_pokemon;nombre_especie; descripción_especie
  lista_iterador_t* iterador = lista_iterador_crear(especie->pokemones);
  particular_pokemon_t* aux = NULL;
  while(lista_iterador_tiene_siguiente(iterador)){
    aux = (particular_pokemon_t*)lista_iterador_siguiente(iterador);
    if(aux->capturado)
      capturado = 'S';
    else
      capturado = 'N';

    fprintf(archivo, FORMATO_P_P_E, letra_pokemones, aux->nombre, aux->nivel, capturado);
  }
}


void zag(FILE * archivo, nodo_abb_t* nodo); 


void zig(FILE * archivo, nodo_abb_t* nodo){
  if(!nodo)
    return;
  especie_pokemon_t* especie = (especie_pokemon_t*)nodo->elemento;
  guardar_especie(archivo, especie);
  zag(archivo, nodo->derecha);
  zig(archivo, nodo->izquierda);
}


void zag(FILE * archivo, nodo_abb_t* nodo){
  if(!nodo)
    return;
  especie_pokemon_t* especie = (especie_pokemon_t*)nodo->elemento;
  guardar_especie(archivo, especie);
  zig(archivo, nodo->izquierda);
  zag(archivo, nodo->derecha);
}


int pokedex_apagar(pokedex_t* pokedex){
  if(!pokedex)
    return ERROR;
  FILE * archivo = fopen(POKEDEX, ESCRIBIR);
  if(!archivo)
    return  ERROR;
  fprintf(archivo, "%s\n", pokedex->nombre_entrenador);
  zig(archivo, pokedex->pokemones->nodo_raiz);
  fclose(archivo);
  return EXITO;
}

pokedex_t* pokedex_prender(){
  FILE * archivo = fopen(POKEDEX, LEER);
  
}
