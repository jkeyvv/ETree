#ifndef __HEAP_H
#define __HEAP_H

#include <stddef.h>

void *pvPortMalloc( size_t xWantedSize );
void vPortFree( void *pv );
size_t xPortGetFreeHeapSize( void );
size_t xPortGetMinimumEverFreeHeapSize( void );
void vPortInitialiseBlocks( void );

#endif
