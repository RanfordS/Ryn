#pragma once

#define LISTIFY_H(Type) \
typedef struct s_##Type##List\
{\
    /*! Number of elements allocated. */\
    Size size;\
    /*! Number of elements used. */\
    Size count;\
    /*! Elements of the list. */\
    Type* data;\
}\
Type##List;\
\
extern Type##List create##Type##List (Size size);\
extern void append##Type##List (Type##List* list, Type element);\
extern void delete##Type##List (Type##List* list);\



#define LISTIFY_C(Type) \
    \
    Type##List create##Type##List (Size size)\
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
        Size i = list->count++;\
        if (list->size <= list->count)\
        {\
            Size newSize = list->size *= 2;\
            list->data = realloc (list->data, newSize*sizeof(Type));\
        }\
        list->data[i] = element;\
    }\
    \
    void delete##Type##List (Type##List* list)\
    {\
        free (list->data);\
    }\

// EOF //
