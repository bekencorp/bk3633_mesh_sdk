/**
 ****************************************************************************************
 *
 * @file co_list.h
 *
 * @brief Common list structures definitions
 *
 * Copyright (C) RivieraWaves 2009-2015
 *
 *
 ****************************************************************************************
 */

#ifndef _CO_LIST_H_
#define _CO_LIST_H_

/**
 *****************************************************************************************
 * @defgroup CO_LIST List management
 * @ingroup COMMON
 *
 * @brief  List management.
 *
 * This module contains the list structures and handling functions.
 * @{
 *****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include <stdint.h>         // standard definition
#include <stdbool.h>        // boolean definition
#include <stddef.h>         // for NULL and size_t
#include "rwip_config.h"    // stack configuration
#include "compiler.h"       // for __INLINE


/*
 * DEFINES
 ****************************************************************************************
 */

/// structure of a list element header
/*@TRACE*/
struct co_list_hdr
{
    /// Pointer to next co_list_hdr
    struct co_list_hdr *next;
};

/// simplify type name of list element header
typedef struct co_list_hdr co_list_hdr_t;

/// structure of a list
struct co_list
{
    /// pointer to first element of the list
    struct co_list_hdr *first;
    /// pointer to the last element
    struct co_list_hdr *last;

    #if (KE_PROFILING)
    /// number of element in the list
    uint32_t cnt;
    /// max number of element in the list
    uint32_t maxcnt;
    /// min number of element in the list
    uint32_t mincnt;
    #endif //KE_PROFILING
};

/// simplify type name of list
typedef struct co_list co_list_t;

/*
 * MACROS
 ****************************************************************************************
 */
/// pop a specific element from the list
#define CO_LIST_POP_ELT(list, elt) co_list_extract(&(list), &(elt->hdr));

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */
/**
 ****************************************************************************************
 * @brief Initialize a list to defaults values.
 *
 * @param list           Pointer to the list structure.
 ****************************************************************************************
 */
void co_list_init(struct co_list *list);

/**
 ****************************************************************************************
 * @brief Construct a list of free elements representing a pool
 *
 * @param list           Pointer to the list structure
 * @param pool           Pointer to the pool to be initialized
 * @param elmt_size      Size of one element of the pool
 * @param elmt_cnt       Nb of elements available in the pool
 ****************************************************************************************
 */
void co_list_pool_init(struct co_list *list,
                       void *pool,
                       size_t elmt_size,
                       uint32_t elmt_cnt);

/**
 ****************************************************************************************
 * @brief Add an element as last on the list.
 *
 * @param list           Pointer to the list structure
 * @param list_hdr       Pointer to the header to add at the end of the list
 *
 ****************************************************************************************
 */
inline void co_list_push_back(struct co_list *list, struct co_list_hdr *list_hdr);

/**
 ****************************************************************************************
 * @brief Append a sequence of elements at the end of a list.
 *
 * Note: the elements to append shall be linked together
 *
 * @param list           Pointer to the list structure
 * @param first_hdr      Pointer to the first element to append
 * @param last_hdr       Pointer to the last element to append
 ****************************************************************************************
 */
inline void co_list_push_back_sublist(struct co_list *list, struct co_list_hdr *first_hdr, struct co_list_hdr *last_hdr);

/**
 ****************************************************************************************
 * @brief Add an element as first on the list.
 *
 * @param list           Pointer to the list structure
 * @param list_hdr       Pointer to the header to add at the beginning of the list
 ****************************************************************************************
 */
void co_list_push_front(struct co_list *list, struct co_list_hdr *list_hdr);

/**
 ****************************************************************************************
 * @brief Extract the first element of the list.
 * @param list           Pointer to the list structure
 * @return The pointer to the element extracted, and NULL if the list is empty.
 ****************************************************************************************
 */
struct co_list_hdr *co_list_pop_front(struct co_list *list);

/**
 ****************************************************************************************
 * @brief Search for a given element in the list, and extract it if found.
 *
 * @param list           Pointer to the list structure
 * @param list_hdr       Element to extract
 *
 * @return true if the element is found in the list, false otherwise
 ****************************************************************************************
 */
bool co_list_extract(struct co_list *list, struct co_list_hdr *list_hdr);

/**
 ****************************************************************************************
 * @brief Extract an element when the previous element is known
 *
 * Note: the element to remove shall follow immediately the reference within the list
 *
 * @param list           Pointer to the list structure
 * @param elt_ref_hdr    Pointer to the referenced element (NULL if element to extract is the first in the list)
 * @param elt_to_rem_hdr Pointer to the element to be extracted
 ****************************************************************************************
 */
void co_list_extract_after(struct co_list *list, struct co_list_hdr *elt_ref_hdr, struct co_list_hdr *elt_to_rem_hdr);

/**
 ****************************************************************************************
 * @brief Extract a sub-list when the previous element is known
 *
 * Note: the elements to remove shall be linked together and  follow immediately the reference element
 *
 * @param[in]  list           Pointer to the list structure
 * @param[in]  ref_hdr        Pointer to the referenced element (NULL if first element to extract is first in the list)
 * @param[in]  last_hdr       Pointer to the last element to extract ()
 ****************************************************************************************
 */
void co_list_extract_sublist(struct co_list *list, struct co_list_hdr *ref_hdr, struct co_list_hdr *last_hdr);

/**
 ****************************************************************************************
 * @brief Searched a given element in the list.
 *
 * @param list           Pointer to the list structure
 * @param list_hdr       Pointer to the searched element
 *
 * @return true if the element is found in the list, false otherwise
 ****************************************************************************************
 */
bool co_list_find(struct co_list *list, struct co_list_hdr *list_hdr);

/**
 ****************************************************************************************
 * @brief Merge two lists in a single one.
 *
 * This function appends the list pointed by list2 to the list pointed by list1. Once the
 * merge is done, it empties list2.
 *
 * @param list1    Pointer to the destination list
 * @param list2    Pointer to the list to append to list1
 ****************************************************************************************
 */
void co_list_merge(struct co_list *list1, struct co_list *list2);

/**
 ****************************************************************************************
 * @brief Insert a given element in the list before the referenced element.
 *
 * @param list           Pointer to the list structure
 * @param elt_ref_hdr    Pointer to the referenced element
 * @param elt_to_add_hdr Pointer to the element to be inserted
 *
 * @return true if the element is found in the list, false otherwise
 ****************************************************************************************
 */
void co_list_insert_before(struct co_list *list,
                        struct co_list_hdr *elt_ref_hdr, struct co_list_hdr *elt_to_add_hdr);

/**
 ****************************************************************************************
 * @brief Insert a given element in the list after the referenced element.
 *
 * @param list           Pointer to the list structure
 * @param elt_ref_hdr    Pointer to the referenced element
 * @param elt_to_add_hdr Pointer to the element to be inserted
 *
 * @return true if the element is found in the list, false otherwise
 ****************************************************************************************
 */
void co_list_insert_after(struct co_list *list,
                        struct co_list_hdr *elt_ref_hdr, struct co_list_hdr *elt_to_add_hdr);


/**
 ****************************************************************************************
 * @brief Count number of elements present in the list
 *
 * @param list           Pointer to the list structure
 *
 * @return Number of elements present in the list
 ****************************************************************************************
 */
uint16_t co_list_size(struct co_list *list);

/**
 ****************************************************************************************
 * @brief Test if the list is empty.
 * @param list           Pointer to the list structure.
 * @return true if the list is empty, false else otherwise.
 ****************************************************************************************
 */
__INLINE bool co_list_is_empty(const struct co_list *const list)
{
    bool listempty;
    listempty = (list->first == NULL);
    return (listempty);
}

/**
 ****************************************************************************************
 * @brief Pick the first element from the list without removing it.
 *
 * @param list           Pointer to the list structure.
 *
 * @return First element address. Returns NULL pointer if the list is empty.
 ****************************************************************************************
 */
__INLINE struct co_list_hdr *co_list_pick(const struct co_list *const list)
{
    return(list->first);
}


/**
 ****************************************************************************************
 * @brief Return following element of a list element.
 *
 * @param list_hdr     Pointer to the list element.
 *
 * @return The pointer to the next element.
 ****************************************************************************************
 */
__INLINE struct co_list_hdr *co_list_next(const struct co_list_hdr *const list_hdr)
{
    return(list_hdr->next);
}

/// @} CO_LIST
#endif // _CO_LIST_H_
