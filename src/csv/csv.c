#include "csv/csv.h"

#include <string.h>
#include <ctype.h>


Csv csv(Alloc * alloc, char delimiter, char * csv_str) {
    Csv instance = {
        .alloc = alloc
    };

    char * c_str_ref = csv_str;
    size_t length = 0;

    char c;
    //TODO: string support, what if the string contains comma symbol which is not delimiter but part of the string
    while((c = *csv_str++) != 0) {
        if(c == delimiter || c == '\n' || *csv_str == 0) {
            if(instance.size >= instance.capacity) {
                instance.capacity = (instance.capacity + 1) * 2;
                instance.arr = resize(alloc, instance.arr, sizeof(Csv_Cell) * instance.capacity);
            }
            
            if(instance.row == 0) {
                instance.column ++;
            }

            if(c == '\n' || *csv_str == 0) {
                instance.row ++;
            } 
            
            instance.arr[instance.size++] = (Csv_Cell) {.length = length, .c_str = c_str_ref};
            c_str_ref = csv_str;
            length = 0;
        } else {
            length ++;
        }
    }

    instance.stride[0] = instance.column;
    instance.stride[1] = 1;

    return instance;
}


Csv csv_slice(Csv * self, size_t shape[4]) {
    if(self->row <= (shape[0] + shape[2]) && self->column <= (shape[1] + shape[3])) {
        return (Csv) {
            .row = shape[2]
            , .column = shape[3]
            , .stride = {self->stride[0], self->stride[1]}
            , .size = shape[2] * shape[3]
            , .arr = csv_at(self, shape[0], shape[1])
        };
    } else {
        return (Csv) {0};
    }
}


Csv csv_transpose(Csv * self) {
    return (Csv) {
        .row = self->column
        , .column = self->row
        , .stride = {self->stride[1], self->stride[0]}
        , .size = self->size
        , .capacity = self->capacity
        , .arr = self->arr
    };
}


Csv_Cell * csv_at(Csv * self, size_t row, size_t column) {
    if(row < self->row && column < self->column) {
        return &self->arr[(self->stride[0] * row) + (self->stride[1] * column)];
    } else {
        return NULL;
    }
}



void csv_finalize(Csv * self) {
    if (self != NULL && self->alloc != NULL) {
        delete(self->alloc, self->arr);
    }
}






