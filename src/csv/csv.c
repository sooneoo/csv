#include "csv/csv.h"

#include <string.h>
#include <ctype.h>


void csv_push_back(Csv * self, size_t length, char * c_str_ref) {
    if(self->size >= self->capacity) {
        self->capacity = (self->capacity + 10);
        self->arr = resize(self->alloc, self->arr, sizeof(Csv_Cell) * self->capacity);
    }

    self->arr[self->size++] = (Csv_Cell) {.length = length, .c_str = c_str_ref};
}


Csv csv(Alloc * alloc, char delimiter, char * csv_str) {
    Csv instance = {
        .alloc = alloc
    };

    char * c_str_ref = csv_str;
    size_t length = 0;
    size_t column = 0;
    char c;

    //TODO: string support, what if the string contains comma symbol which is not delimiter but part of the string
    while((c = *csv_str++) != 0) {
        if(c == delimiter || c == '\r' || c == '\n' || *csv_str == 0) {
            
            csv_push_back(&instance, length, c_str_ref);
            column ++;
            
            if(c == '\r' || c == '\n' || *csv_str == 0) {
                if(instance.row > 0) {
                    if(column > instance.column) {
                        size_t new_column = column;
                        size_t old_column = instance.column;
                        size_t diff_per_row = new_column - old_column;
                        
                        // Aktuální řádek je ten, který právě skončil (instance.row - 1)
                        size_t current_row_index = instance.row;
                        
                        // Spočítáme celkový posun, o který se musíme zvětšit.
                        // Posun je součtem diff_per_row pro VŠECHNY PŘEDCHOZÍ řádky.
                        // (Aktuální řádek se již uložil s novou šířkou, takže se nepočítá)
                        size_t total_diff = current_row_index * diff_per_row;
                        
                        // 1. Zajištění kapacity pro celou matici + total_diff
                        size_t required_capacity = (instance.row * new_column);
                        if(required_capacity >= instance.capacity) {
                            // Přidat navíc kapacitu pro případ, že resize přesune data
                            instance.capacity = required_capacity + 10; 
                            instance.arr = resize(alloc, instance.arr, sizeof(Csv_Cell) * instance.capacity);
                            // POZOR: Ukazatel instance.arr se mohl změnit!
                        }

                        // 2. Přesun bloku dat pro VŠECHNY ULOŽENÉ BUŇKY
                        
                        // Adresa, odkud začínají data, která musíme posunout.
                        // Je to začátek druhého řádku (řádek 1)
                        Csv_Cell * source_start = &instance.arr[old_column]; 
                        
                        // Cíl posunu (začátek druhého řádku + total_diff)
                        Csv_Cell * dest_start = &instance.arr[old_column + total_diff]; 
                        
                        // Délka dat, která se přesouvají (všechny buňky od řádku 1 až do aktuální pozice)
                        size_t move_len = instance.size - old_column;
                        
                        // Přesunout blok (odzadu, aby nedošlo k přepsání)
                        memmove(dest_start, source_start, sizeof(Csv_Cell) * move_len);
                        
                        // 3. Vložení prázdných buněk (padding) do vytvořených mezer
                        
                        // Jdeme odzadu a vkládáme padding pro každý předchozí řádek
                        // Procházíme jen řádky, které MUSÍME padovat (row 0 až current_row_index - 1)
                        for(size_t row = 0; row < current_row_index; row++) {
                            // Vypočítáme cílové místo pro padding
                            size_t padding_index = (row * instance.column) + old_column; 
                            
                            // Nastavíme pole Csv_Cell na nuly (reprezentuje prázdné buňky)
                            // Musíme použít sizeof(Csv_Cell) * diff_per_row
                            memset(&instance.arr[padding_index], 0, sizeof(Csv_Cell) * diff_per_row);
                        }

                        instance.size += total_diff;
                        instance.column = new_column;
                    } else if(column < instance.column) {
                       for(size_t i = column; i < instance.column; i++) {
                            csv_push_back(&instance, 0, NULL);
                       }
                    }
                } else {
                    instance.column = column;
                }

                instance.row ++;
                column = 0;
            } 

            if (c == '\r' && *csv_str == '\n') {
                csv_str++;
            }
            
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
    if(self->row > (shape[0] + shape[2]) && self->column > (shape[1] + shape[3])) {
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


void csv_show(Csv * self, FILE * stream) {
    for(size_t row = 0; row < self->row; row++) {
        for(size_t column = 0; column < self->column; column++) {
            Csv_Cell *cell = csv_at(self, row, column);
            fprintf(stream, column == 0 ? "%.*s" : ",%.*s", (int) cell->length, cell->c_str);
        }

        fprintf(stream, "\n");
    }
}



void csv_finalize(Csv * self) {
    if (self != NULL && self->alloc != NULL) {
        delete(self->alloc, self->arr);
    }
}






