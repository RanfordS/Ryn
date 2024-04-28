#pragma once

#define LISTIFY_H(Type) \
typedef struct s_##Type##List\
{\
    /*! Number of elements allocated. */\
    size_t size;\
    /*! Number of elements used. */\
    size_t count;\
    /*! Elements of the list. */\
    Type* data;\
}\
Type##List;\
\
extern Type##List create##Type##List (size_t size);\
extern void append##Type##List (Type##List* list, Type element);



#define LISTIFY_C(Type) \
    \
    Type##List create##Type##List (size_t size)\
    {\
        Type##List new =\
        {\
            .size = size,\
            .count = 0,\
            .data = malloc (size*sizeof(Type)),\
        };\
        return new;\
    }\
    \
    void append##Type##List (Type##List* list, Type element)\
    {\
        size_t i = list->count++;\
        if (list->size <= list->count)\
        {\
            size_t newSize = list->size *= 2;\
            list->data = realloc (list->data, newSize*sizeof(Type));\
        }\
        list->data[i] = element;\
    }

// EOF //
