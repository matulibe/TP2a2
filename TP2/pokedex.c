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

#define FORMATO_E "%i;%[^;];%i;[^;];[^;];%i\n"
#define FORMATO_A  "%i;%[^;];[^;];[^;];%i;%c\n"
#define FORMATO_P_P "%c;[^;];%i;%c\n"
#define FORMATO_P_E "%c;%i;[^;];[^;]\n"
#define CANT_E 6
#define CANT_A 6
#define CANT_P_P 4
#define CANT_P_E 4

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

	if(((int)entrante)>((especie->numero)))
		return MAYOR;

	if(((int)entrante)<((int)nodo_actual))
		return MENOR;

	return IGUAL;
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
  while(leidos != EOF){
    fscanf(archivo, FORMATO_A, especie->numero, especie->nombre, especie->descripcion, poke->nombre, poke->nivel, &capturado);
    especie_pokemon_t* buscador = (especie_pokemon_t*)(arbol_buscar(pokedex->pokemones, especie->numero));
    if(!buscador){
      especie_pokemon_t* especie_aux = calloc(1, sizeof(especie_pokemon_t));
      if(!especie_aux)
        return  ERROR;
      especie_aux = especie;
      arbol_insertar(pokedex->pokemones, (void*)especie_aux);
      especie_aux->pokemones = lista_crear();
      buscador = (especie_pokemon_t*)(arbol_buscar(pokedex->pokemones, especie->numero));
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
    fscanf(archivo, FORMATO_E, numero_poke, poke_evol->nombre, evolucion->numero, evolucion->nombre, evolucion->descripcion);
    especie_pokemon_t* especie = (especie_pokemon_t*)(arbol_buscar(pokedex->pokemones, (void*)evolucion->numero));
    if(!especie){
      especie = calloc(1, sizeof(especie_pokemon_t));
      strcpy(especie->nombre, evolucion->nombre);
      especie->numero = evolucion->numero;
      strcpy(especie->descripcion, evolucion->descripcion);
      arbol_insertar(pokedex, (void*)especie);
    }
    especie_pokemon_t* buscador = (especie_pokemon_t*)(arbol_buscar(pokedex->pokemones, (void*)numero_poke));
    int pos = 0;
    lista_iterador_t* iterador = lista_iterador_crear(buscador->pokemones);
    particular_pokemon_t* aux = lista_iterador_siguiente(iterador);
    while(strcmp(aux->nombre, poke_evol->nombre)!=IGUAL && lista_iterador_tiene_siguiente(iterador)){
      aux = (particular_pokemon_t*)lista_iterador_siguiente(iterador);
      pos++;
    }
    lista_iterador_destruir(iterador);
    if(aux->capturado == true){
      poke_evol->nivel == aux->nivel;
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
  while(!lista_vacia){
    aux = (particular_pokemon_t*)lista_tope(pokedex->ultimos_capturados);
    printf("%s\n", aux->nombre);
    lista_desapilar(pokedex->ultimos_capturados);
  }
}

void pokedex_ultimos_vistos(pokedex_t* pokedex){
   if(!pokedex)
    return;
  particular_pokemon_t* aux = NULL;
  while(!lista_vacia){
    aux = (particular_pokemon_t*)lista_primero(pokedex->ultimos_vistos);
    printf("%s\n", aux->nombre);
    lista_desencolar(pokedex->ultimos_vistos);
  }
}

void pokedex_informacion(pokedex_t* pokedex, int numero_pokemon, char nombre_pokemon[MAX_NOMBRE]){
  if(!pokedex || !numero_pokemon || nombre_pokemon)
    return;

  especie_pokemon_t* especie = (especie_pokemon_t*)arbol_buscar(pokedex, (void*)numero_pokemon);
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

void destruir_listas(lista_t* pokemones){

}

void pokedex_destruir(pokedex_t* pokedex){

}