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
#define CANT_E 6
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

void destruir_pokemones(void* pokemon, void* extra){
  extra=extra;
  free((particular_pokemon_t*)pokemon);
}

void liberar_especie(especie_pokemon_t* especie){
  if(especie){
    lista_con_cada_elemento(especie->pokemones, destruir_pokemones, NULL);
    lista_destruir(especie->pokemones);
    free(especie);
  }
}

void destructor(void* especie){
  if(!especie)
    return;
  liberar_especie((especie_pokemon_t*)especie);
  
}


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


int pokedex_avistar(pokedex_t* pokedex, char ruta_archivo[MAX_RUTA]){
  FILE * archivo = fopen(ruta_archivo, LEER);
  if(!archivo)
    return ERROR;
  particular_pokemon_t* poke = NULL;
  especie_pokemon_t* especie = NULL;
  especie_pokemon_t* buscador = NULL;
  char capturado;
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
    leidos = fscanf(archivo, AVISTAMIENTOS, &especie->numero, especie->nombre, especie->descripcion, poke->nombre, &poke->nivel, &capturado);
    if(capturado == CAPTURADO)
      poke->capturado = true;
    buscador = (especie_pokemon_t*)(arbol_buscar(pokedex->pokemones, &especie));
    if(buscador == NULL){
      arbol_insertar(pokedex->pokemones, especie);
      especie->pokemones = lista_crear();
      buscador = especie;
    }else
      free(especie);
    lista_insertar(buscador->pokemones, poke);
    if(capturado == CAPTURADO)
      lista_apilar(pokedex->ultimos_capturados, (void*)poke);
    lista_encolar(pokedex->ultimos_vistos, (void*)poke);
  }
  fclose(archivo);
  return retorno;
}


int cambiar_evolucionado(especie_pokemon_t* buscador, especie_pokemon_t* especie, particular_pokemon_t* poke_evol){
  int pos = 0, retorno = EXITO;
  bool encontrado = false;
  lista_iterador_t* iterador = lista_iterador_crear(buscador->pokemones);
  particular_pokemon_t* aux = NULL;
  while(lista_iterador_tiene_siguiente(iterador) && !encontrado){
    aux = (particular_pokemon_t*)lista_iterador_siguiente(iterador); 
    if(strcmp(aux->nombre, poke_evol->nombre)!=IGUAL)
      encontrado = true;
    pos++;
    }
  lista_iterador_destruir(iterador);
  if(aux->capturado == true){
    poke_evol->nivel = aux->nivel;
    poke_evol->capturado = true;
    lista_borrar_de_posicion(buscador->pokemones, (size_t)pos);
    lista_insertar(especie->pokemones, poke_evol);
  }else
    retorno = ERROR;
  return retorno;
}


int pokedex_evolucionar(pokedex_t* pokedex, char ruta_archivo[MAX_RUTA]){
  if(!pokedex || !ruta_archivo)
    return ERROR;
  FILE * archivo = fopen(ruta_archivo, LEER);
  if(!archivo)
    return ERROR;
  int retorno = EXITO, numero_poke = 0, leidos = 0, aux;
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
    leidos = fscanf(archivo, EVOLUCION, &numero_poke, poke_evol->nombre, &evolucion->numero, evolucion->nombre, evolucion->descripcion);
    if(leidos != EOF){
    especie_pokemon_t* especie = (especie_pokemon_t*)(arbol_buscar(pokedex->pokemones, &evolucion->numero));
    if(especie == NULL){
      especie = evolucion;
      arbol_insertar(pokedex->pokemones, especie);
    }
    especie_pokemon_t* buscador = (especie_pokemon_t*)(arbol_buscar(pokedex->pokemones, &numero_poke));
    if(!buscador){
      free(poke_evol);
      arbol_borrar(pokedex->pokemones, especie);
    }else 
      aux = cambiar_evolucionado(buscador, especie, poke_evol);
    if(aux == ERROR && retorno != ERROR)
      retorno = ERROR;
    }else{
      free(poke_evol);
      arbol_borrar(pokedex->pokemones, evolucion);
    }
  }
  fclose(archivo);
  return retorno;
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

void listar_pokemones(void* pokemon, void* extra){
  particular_pokemon_t* poke = (particular_pokemon_t*)pokemon;
  printf("\tNombre:%s \tNivel:%i \tEstado:%s\n", poke->nombre, poke->nivel, poke->capturado==true?ESTADO_1:ESTADO_2);
}

void listar_especie(especie_pokemon_t* especie){
  printf("Especie: %s\n Descripcion: %s\n", especie->nombre, especie->descripcion);
  lista_con_cada_elemento(especie->pokemones, listar_pokemones, NULL);
}


void imprimir_informacion(especie_pokemon_t* especie, particular_pokemon_t* pokemon){
  printf("Pokemon: %s\n", pokemon->nombre);
  printf("\tEstado: %s\n", pokemon->capturado==true?ESTADO_1:ESTADO_2);
  printf("\tNivel: %i\n", pokemon->nivel);
  printf("\tEpecie: %s\n", especie->nombre);
  printf("\tDescripcion: %s\n", especie->descripcion);
}

void pokedex_informacion(pokedex_t* pokedex, int numero_pokemon, char nombre_pokemon[MAX_NOMBRE]){
  if(!pokedex || !numero_pokemon)
    return;
  especie_pokemon_t* aux = crear_especie();
  aux->numero = numero_pokemon;
  especie_pokemon_t* especie = (especie_pokemon_t*)arbol_buscar(pokedex->pokemones, aux);
  if(!especie){
    printf("No se podido encontrar la especie pokemon\n");
    return;
  }
  if(!nombre_pokemon ){
    listar_especie(especie);
    free(aux);
    return;
  }
  lista_iterador_t* iterador = lista_iterador_crear(especie->pokemones);
  particular_pokemon_t* pokemon = NULL;
  bool encontro = false;
  while(lista_iterador_tiene_siguiente(iterador) && !encontro){
    pokemon = (particular_pokemon_t*)lista_iterador_siguiente(iterador);
    if(strcmp(nombre_pokemon, pokemon->nombre)==IGUAL)
      encontro = true;
  }
  lista_iterador_destruir(iterador);
  if(!encontro){
    printf("El pokemon buscado no pudo ser localizado\n");
    return;
  }
  imprimir_informacion(especie, pokemon);
  free(aux);
}





void pokedex_destruir(pokedex_t* pokedex){
  arbol_destruir(pokedex->pokemones);
  lista_destruir(pokedex->ultimos_capturados);
  lista_destruir(pokedex->ultimos_vistos);
  free(pokedex);
}






void escribir_pokemones(void* pokemon, void* extra){
  FILE * archivo = (FILE *)extra;
  particular_pokemon_t* poke = (particular_pokemon_t*)pokemon;
  fprintf(archivo, POKE_E, POKEMON, poke->nombre, poke->nivel, poke->capturado==true?CAPTURADO:NO_CAPTURADO);
}

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
      arbol_insertar(pokedex->pokemones, especie);
    }else if(letra == POKEMON){
      poke = crear_pokemon();
      cant_leidos = fscanf(archivo, POKE_L, poke->nombre, &poke->nivel, &capturado);
      if(capturado == CAPTURADO)
        poke->capturado = true;
      else
        poke->capturado = false;
      if(cant_leidos != EOF)
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