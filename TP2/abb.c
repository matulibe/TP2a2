#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include "abb.h"

#define MENOR -1
#define MAYOR 1
#define IGUAL 0
#define EXITO 0
#define ERROR -1
#define VACIO 0


abb_t* arbol_crear(abb_comparador comparador, abb_liberar_elemento destructor){
  if(!comparador)
    return NULL;
  abb_t* arbol = calloc(1, sizeof(abb_t));
  if(arbol != NULL){
    arbol->comparador = comparador;
    arbol->destructor = destructor;
  }
  return arbol;
}

/*
 *Pre: Sera llamada por arbol_insertar y recicbira el arbol, el nodo nuevo y el nodo
 *con el cual lo comparara.
 *Post: Recorrera le arbol recursivamente hasta hallar su nodo padre, ya que comparara
 *al nodo nuevo con el actual para saber donde moverse. En el caso que dicho hijo no exista,
 *esa posicion se le asignara al elemento a insertar.
 */
int insercion(nodo_abb_t* actual, nodo_abb_t* nodo, abb_t* arbol){
  if(arbol->comparador(nodo->elemento, actual->elemento) == MAYOR){
    if(actual->derecha == NULL){
      actual->derecha = nodo;
      return EXITO;
    }else{
      return insercion(actual->derecha, nodo, arbol);
    }
  }else{
    if(actual->izquierda == NULL){
      actual->izquierda = nodo;
      return EXITO;
    }else{
      return insercion(actual->izquierda, nodo, arbol);
    }
  }
}


int arbol_insertar(abb_t* arbol, void* elemento){
  if(!arbol)
    return ERROR;
  nodo_abb_t* nuevo = calloc(1, sizeof(nodo_abb_t));
  nuevo->elemento = elemento;
  if(!arbol->nodo_raiz){
    arbol->nodo_raiz = nuevo;
    return EXITO;
  }
  return insercion(arbol->nodo_raiz, nuevo, arbol);
}

/*
 *Pre: Recibira un nodo y al padre de este para hallar un nodo de reemplazo.
 *Post: Recorrera el arbol de manera recursiva para buscar el nodo mayor de los menores. O sea
 *ira al hijo izquierda era a la derecha lo mas posible y devolvera dicho nodo, si es posible. Y en
 *el caso que el hijo izquierda no tenga hijos devolvera al propio hijo.
 *Modificara las referencias a los nodos hijos si es necesario.
 */
nodo_abb_t* buscar_reemplazo(nodo_abb_t* nodo, nodo_abb_t* padre){
  if(!nodo)
    return NULL;
  if(!nodo->derecha){
    if(nodo->izquierda){
      if(padre->izquierda == nodo){
        padre->izquierda = nodo->izquierda;
        return nodo;
      }
      padre->derecha = nodo->izquierda;
      nodo->izquierda = NULL;
      return nodo;
    }
    if(padre->izquierda != nodo)
      padre->derecha = NULL;
    return nodo;
  }
  return buscar_reemplazo(nodo->derecha, nodo);
}

/*
 *Pre: Recibira el nodo a borrar. Para hallar su reemplazo.
 *Post: En el caso de que no exista un hijo izquierda devolvera el derecho.
 *En el caso de que si exista llamara a buscar reemplazo y a este le asignara
 *los hijos del elemento a borrar.
 */
nodo_abb_t* reemplazar(nodo_abb_t* borrado){
  if(!borrado->izquierda)
    return borrado->derecha;
  nodo_abb_t* aux = buscar_reemplazo(borrado->izquierda, borrado);
  if(borrado->izquierda == aux)
    borrado->izquierda = NULL;
  aux->izquierda = borrado->izquierda;
  aux->derecha = borrado->derecha;
  return aux;
}

/*
 *Pre: En el caso de que el nodo a borrar sea un hijo izquierdo se llamara
 *a esta funcion y se le enviara a este hijo y a su padre.
 *Post: Se le reasignara el hijo izquierda al nodo padre dependiendo si el nodo
 *a borrar tiene hijo izquierda o no, o si no tiene hijos.
 */
void reordenar_izq(nodo_abb_t* borrado, nodo_abb_t* nodo){
  if(borrado->izquierda){
    nodo_abb_t* aux = reemplazar(borrado);
    nodo->izquierda = aux;
  }else{
    if(borrado->derecha)
      nodo->izquierda = borrado->derecha;
    else
      nodo->izquierda = NULL;
  }
}

/*
 *Pre: En el caso de que el nodo a borrar sea un hijo derecho se llamara
 *a esta funcion y se le enviara a este hijo y a su padre.
 *Post: Se le reasignara el hijo derecha al nodo padre dependiendo si el nodo
 *a borrar tiene hijo izquierda o no, o si no tiene hijos.
 */
void reordenar_der(nodo_abb_t* borrado, nodo_abb_t* nodo){
  if(borrado->izquierda){
    nodo_abb_t* aux = reemplazar(borrado);
    nodo->derecha = aux;
  }else{
    if(borrado->derecha)
      nodo->derecha = borrado->derecha;
    else
      nodo->derecha = NULL;
  }
}

/*
 *Pre: Recibira al arbol, el elemento a borrar, un nodo y el comparador(para que no queden tan largos los ifs).
 *Post: Recorrera el arbol de manera recursiva y chequeara si el elemento a borrar es menor o mayor al elemento del
 *nodo que le mandaron. Comparara si el elemento es igual al hijo al que se deberia de mover y en el caso de que lo
 *sea lo eliminara y, si es necesario, lo reemplazara.
 */
int eliminar_y_reordenar(abb_t* arbol, void* elemento, nodo_abb_t* nodo, abb_comparador comparador){
  if(!nodo)
    return ERROR;
  nodo_abb_t* borrado;
  if(comparador(elemento, nodo->elemento) == MENOR){
    borrado = nodo->izquierda;
    if(!borrado)
      return ERROR;
    if(comparador(elemento, borrado->elemento) == IGUAL){
      reordenar_izq( borrado, nodo);
      arbol->destructor(borrado->elemento);
      free(borrado);
      return EXITO;
    }
  }else if(comparador(elemento, nodo->elemento) == MAYOR){
    borrado = nodo->derecha;
    if(!borrado)
      return ERROR;
    if(comparador(elemento, borrado->elemento) == IGUAL){
      reordenar_der(borrado, nodo);
      arbol->destructor(borrado->elemento);
      free(borrado);
      return EXITO;
    }
  }
  return eliminar_y_reordenar(arbol, elemento, borrado, comparador);
}


int arbol_borrar(abb_t* arbol, void* elemento){
  if(!arbol)
    return ERROR;
  nodo_abb_t* raiz = arbol->nodo_raiz;
  if(arbol->comparador(elemento, raiz->elemento) == IGUAL){
    arbol->nodo_raiz = reemplazar(raiz);
    arbol->destructor(raiz->elemento);
    free(raiz);
    return EXITO;
  }
  return eliminar_y_reordenar(arbol, elemento, raiz, arbol->comparador);
}

/*
 *Pre: Recibira un nodo, el arbol y el elemento el cual quiere hallar
 *Post: Recorrera el arbol de manera recursiva hasta hayar el elemento si es
 *que existe. Ira comparando el nodo con el elemento y se llamara a si misma de nuevo
 *mandando al hijo correspondiente hasta hallar el elemento. En el caso de no existir el
 *elemento devolvera NULL.
 */
void* busqueda(nodo_abb_t* nodo, void* elemento, abb_t* arbol){
  if(!nodo)
    return NULL;
  int comparador = arbol->comparador(elemento, nodo->elemento);
  if(comparador == MENOR){
    return busqueda(nodo->izquierda, elemento, arbol);
  }else if(comparador == MAYOR){
    return busqueda(nodo->derecha, elemento, arbol);
  }
  return nodo->elemento;
}


void* arbol_buscar(abb_t* arbol, void* elemento){
  if(!arbol)
    return NULL;
  return busqueda(arbol->nodo_raiz, elemento, arbol);
}


void* arbol_raiz(abb_t* arbol){
  if(!arbol)
    return NULL;
  if(!arbol->nodo_raiz)
    return NULL;
  return (arbol->nodo_raiz->elemento);
}


bool arbol_vacio(abb_t* arbol){
  if(!arbol)
    return true;
  if(!arbol->nodo_raiz){
    return true;
  }
  return false;
}

/*
 *Pre: Recibira un nodo, un vector, el tamaño de este, un puntero de posicion y otro de cantidad.
 *Post: Recorrera recursivamente el arbol en la forma inorden y llenando el array a medida que
 *avanza. Primero guardara los hijos izquierda, luego el nodo y por ultimo los hijos derecha.
 *Guardara todos los elementos hasta terminar el arbol o llenar el array.
 */
void recorrido_ind(nodo_abb_t* nodo, void** array, int tamanio, int* posicion, int* cantidad){
  if(!nodo)
    return;
  recorrido_ind(nodo->izquierda, array, tamanio, posicion, cantidad);
  if(*posicion >= tamanio)
    return;
  array[*posicion] = nodo->elemento;
  *cantidad+=1;
  *posicion+=1;
  recorrido_ind(nodo->derecha, array, tamanio, posicion, cantidad);
}


int arbol_recorrido_inorden(abb_t* arbol, void** array, int tamanio_array){
  if(!array || !tamanio_array || !arbol)
    return VACIO;
  int posicion = 0, cantidad = 0;
  recorrido_ind(arbol->nodo_raiz, array, tamanio_array, &posicion, &cantidad);
  return cantidad;
}

/*
 *Pre: Recibira un nodo, un vector, el tamaño de este, un puntero de posicion y otro de cantidad.
 *Post: Recorrera recursivamente el arbol en la forma preorden y llenando el array a medida que
 *avanza. Primero guardara el nodo, luego los hijos izquierda y por ultimo los hijos derecha.
 *Guardara todos los elementos hasta terminar el arbol o llenar el array.
 */
void recorrido_nid(nodo_abb_t* nodo, void** array, int tamanio, int* posicion, int* cantidad){
  if(!nodo)
    return;
  if(*cantidad >= tamanio)
    return;
  array[*posicion] = nodo->elemento;
  *cantidad+=1;
  *posicion+=1;
  recorrido_nid(nodo->izquierda, array, tamanio, posicion, cantidad);
  recorrido_nid(nodo->derecha, array, tamanio, posicion, cantidad);
}


int arbol_recorrido_preorden(abb_t* arbol, void** array, int tamanio_array){
  if(!array || !tamanio_array || !arbol)
    return VACIO;
  int posicion = 0, cantidad = 0;
  recorrido_nid(arbol->nodo_raiz, array, tamanio_array, &posicion, &cantidad);
  return cantidad;
}

/*
 *Pre: Recibira un nodo, un vector, el tamaño de este, un puntero de posicion y otro de cantidad.
 *Post: Recorrera recursivamente el arbol en la forma postorden y llenando el array a medida que
 *avanza. Primero guardara los hijos izquierda, luego los hijos derecha y por ultimo el nodo.
 *Guardara todos los elementos hasta terminar el arbol o llenar el array.
 */
void recorrido_idn(nodo_abb_t* nodo, void** array, int tamanio, int* posicion, int* cantidad){
  if(!nodo)
    return;
  if(*cantidad >= tamanio)
    return;
  recorrido_idn(nodo->izquierda, array, tamanio, posicion, cantidad);
  recorrido_idn(nodo->derecha, array, tamanio, posicion, cantidad);
  if(*posicion >= tamanio){
    return;
  }
  array[*posicion] = nodo->elemento;
  *cantidad+=1;
  *posicion+=1;
}


int arbol_recorrido_postorden(abb_t* arbol, void** array, int tamanio_array){
  if(!array || !tamanio_array || !arbol)
    return VACIO;
  int posicion = 0, cantidad = 0;
  recorrido_idn(arbol->nodo_raiz, array, tamanio_array, &posicion, &cantidad);
  return cantidad;
}


/*
 *Pre: Recibira un nodo y el arbol
 *Post: Usara el destructor del arbol para destruir el elemento y despues liberara la memoria
 *ocupada por el nodo.
 */
static void destruir_nodos(nodo_abb_t* nodo, abb_t* arbol){
  if(!nodo)
    return;
  destruir_nodos(nodo->izquierda, arbol);
  destruir_nodos(nodo->derecha, arbol);
  arbol->destructor(nodo->elemento);
  free(nodo);

}


void arbol_destruir(abb_t* arbol){
  if(!arbol)
    return;
  destruir_nodos(arbol->nodo_raiz, arbol);
  free(arbol);
}

void cada_elemento_ind(nodo_abb_t* nodo, bool (*funcion)(void*, void*), void* extra, bool* corte){
  if(!nodo || *corte == true)
    return;
  cada_elemento_ind(nodo->izquierda, funcion, extra, corte);
  if(*corte == true)
    return;
  *corte = funcion(nodo->elemento, extra);
  cada_elemento_ind(nodo->derecha, funcion, extra, corte);
}


void cada_elemento_nid(nodo_abb_t* nodo, bool (*funcion)(void*, void*), void* extra, bool* corte){
  if(!nodo || *corte == true)
    return;
  *corte = funcion(nodo->elemento, extra);
  cada_elemento_nid(nodo->izquierda, funcion, extra, corte);
  cada_elemento_nid(nodo->derecha, funcion, extra, corte);
}


void cada_elemento_idn(nodo_abb_t* nodo, bool (*funcion)(void*, void*), void* extra, bool* corte){
  if(!nodo || *corte == true)
    return;
  cada_elemento_idn(nodo->izquierda, funcion, extra, corte);
  cada_elemento_idn(nodo->derecha, funcion, extra, corte);
  if(*corte == true)
    return;
  *corte = funcion(nodo->elemento, extra);
}

void abb_con_cada_elemento(abb_t* arbol, int recorrido, bool (*funcion)(void*, void*), void* extra){
  if(!arbol || !funcion)
    return;
  bool corte = false;
  if(recorrido == ABB_RECORRER_INORDEN){
    cada_elemento_ind(arbol->nodo_raiz, funcion, extra, &corte);
  }else if(recorrido == ABB_RECORRER_PREORDEN){
    cada_elemento_nid(arbol->nodo_raiz, funcion, extra, &corte);
  }else{
    cada_elemento_idn(arbol->nodo_raiz, funcion, extra, &corte);
  }
}
