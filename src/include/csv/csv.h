#ifndef _CSV_H_
#define _CSV_H_


#include <stdio.h>
#include <stdbool.h>
#include <alloc/alloc.h>
#include <iterator/iterator.h>


typedef struct {
    size_t length;
    char * c_str;
} Csv_Cell;


typedef struct {
    Alloc * alloc;

    size_t row;
    size_t column;

    size_t stride[2];

    size_t capacity;
    size_t size;

    Csv_Cell * arr;
} Csv;


Csv csv(Alloc * alloc, char delimiter, char * csv_str);


void csv_push_back(Csv * self, size_t length, char * c_str_ref);


Csv csv_slice(Csv * self, size_t shape[4]);


Csv csv_transpose(Csv * self);


Csv_Cell * csv_at(Csv * self, size_t row, size_t column);


void csv_show(Csv * self, FILE * stream);


void csv_finalize(Csv * self);



#endif

