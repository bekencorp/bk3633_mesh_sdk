/**
 ****************************************************************************************
 *
 * @file co_list.c
 *
 * @brief List management functions
 *
 * Copyright (C) RivieraWaves 2009-2015
 *
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @addtogroup CO_LIST
 * @{
 *****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include <string.h>      // for mem* functions
#include "co_math.h"     // maths definitions
#include "co_list.h"     // common list definitions
#include "arch.h"        // for assertions
#include "ke_mem.h"

/*
 * FUNCTION DEFINTIONS
 ****************************************************************************************
 */
inline void co_list_init(struct co_list *list)
{
    list->first = NULL;
    list->last = NULL;

    #if (KE_PROFILING)
    list->cnt = 0;
    list->maxcnt = 0;
    list->mincnt = 0xFFFFFFFF;
    #endif //KE_PROFILING
}

void co_list_pool_init(struct co_list *list,
                       void *pool,
                       size_t elmt_size,
                       uint32_t elmt_cnt)
{
    uint32_t i;
    struct co_list_hdr * elt = pool;
    struct co_list_hdr * elt_next = NULL;

    // First element
    list->first = elt;

    // Link elements
    for (i = 0; i < (elmt_cnt-1); i++)
    {
        elt_next = (struct co_list_hdr *)((uint8_t *)elt + (uint32_t)elmt_size);
        // Link element to next
        elt->next = elt_next;

        // Move to the next element
        elt = elt_next;
    }

    // Last element
    elt->next = NULL;
    list->last = elt;

    #if (KE_PROFILING)
    list->cnt = elmt_size;
    list->maxcnt = elmt_size;
    #endif //KE_PROFILING
}

__ATTR_ARM void co_list_push_back(struct co_list *list, struct co_list_hdr *list_hdr)
{
    // Sanity check
    ASSERT_ERR(list_hdr != NULL);

    // check if list is empty
    if (co_list_is_empty(list))
    {
        // list empty => pushed element is also head
        list->first = list_hdr;
    }
    else
    {
        // list not empty => update next of last
        list->last->next = list_hdr;
    }

    // add element at the end of the list
    list->last = list_hdr;
    list_hdr->next = NULL;

    #if (KE_PROFILING)
    list->cnt++;
    if(list->maxcnt < list->cnt)
    {
        list->maxcnt = list->cnt;
    }
    #endif //KE_PROFILING
}

void co_list_push_back_sublist(struct co_list *list, struct co_list_hdr *first_hdr, struct co_list_hdr *last_hdr)
{
    // Sanity check
    ASSERT_ERR(first_hdr != NULL);
    ASSERT_ERR(last_hdr != NULL);

    // check if list is empty
    if (co_list_is_empty(list))
    {
        // list empty => pushed element is also head
        list->first = first_hdr;
    }
    else
    {
        // list not empty => update next of last
        list->last->next = first_hdr;
    }

    // Update last pointer
    list->last = last_hdr;
    last_hdr->next = NULL;

    #if (KE_PROFILING)
    {
        uint8_t nb_elts = 0;
        struct co_list_hdr *cursor = first_hdr;

        // Count the number of extracted elements
        while(cursor != NULL)
        {
            nb_elts++;
            cursor = cursor->next;
        }

        list->cnt++;
        list->maxcnt = co_max(list->maxcnt, list->cnt);
    }
    #endif //KE_PROFILING
}

void co_list_push_front(struct co_list *list,
                        struct co_list_hdr *list_hdr)
{
    // Sanity check
    ASSERT_ERR(list_hdr != NULL);

    // check if list is empty
    if (co_list_is_empty(list))
    {
        // list empty => pushed element is also head
        list->last = list_hdr;
    }

    // add element at the beginning of the list
    list_hdr->next = list->first;
    list->first = list_hdr;

    #if (KE_PROFILING)
    list->cnt++;
    if(list->maxcnt < list->cnt)
    {
        list->maxcnt = list->cnt;
    }
    #endif //KE_PROFILING
}

__ATTR_ARM struct co_list_hdr *co_list_pop_front(struct co_list *list)
{
    struct co_list_hdr *element;

    // check if list is empty
    element = list->first;
    if (element != NULL)
    {

        // The list isn't empty : extract the first element
        list->first = list->first->next;

        if(list->first == NULL)
        {
            list->last = list->first;
        }

        #if (KE_PROFILING)
        list->cnt--;
        if(list->mincnt > list->cnt)
        {
            list->mincnt = list->cnt;
        }
        #endif //KE_PROFILING
    }
    return element;
}

bool co_list_extract(struct co_list *list, struct co_list_hdr *list_hdr)
{
    bool found = false;

    // sanity check
    ASSERT_ERR(list != NULL);

    struct co_list_hdr *prev = NULL;
    struct co_list_hdr *curr = list->first;

    // Search for the element
    while(curr != NULL)
    {
        // Check element
        if(curr == list_hdr)
        {
            found = true;
            break;
        }

        // Move pointers
        prev = curr;
        ASSERT_INFO(list->first != curr->next, ((uint32_t)curr)>>16, ((uint32_t)curr)&0xFFFF);
        ASSERT_INFO(curr != curr->next, ((uint32_t)curr)>>16, ((uint32_t)curr)&0xFFFF);
        curr = curr->next;
    }

    if(found)
    {
        // Check if the element is first
        if(prev == NULL)
        {
            // Extract element
            list->first = list_hdr->next;
        }
        else
        {
            // Extract element
            prev->next = list_hdr->next;
        }

        // Check if the element is last
        if(list_hdr == list->last)
        {
            // Update last pointer
            list->last = prev;
        }

        #if (KE_PROFILING)
        // Reference element extracted
        list->cnt--;
        if(list->mincnt > list->cnt)
        {
            list->mincnt = list->cnt;
        }
        #endif //KE_PROFILING
    }

    return found;
}

void co_list_extract_after(struct co_list *list, struct co_list_hdr *elt_ref_hdr, struct co_list_hdr *elt_to_rem_hdr)
{
    // sanity check
    ASSERT_ERR(list != NULL);
    ASSERT_ERR(elt_to_rem_hdr != NULL);

    // Check if the element is first
    if(elt_ref_hdr == NULL)
    {
        ASSERT_ERR(elt_to_rem_hdr == list->first);

        // The list isn't empty : extract the first element
        list->first = list->first->next;
    }
    else
    {
        ASSERT_ERR(elt_to_rem_hdr == elt_ref_hdr->next);

        // Extract element
        elt_ref_hdr->next = elt_to_rem_hdr->next;
    }

    // Check if the element is last
    if(elt_to_rem_hdr == list->last)
    {
        // Update last pointer
        list->last = elt_ref_hdr;
    }

    #if (KE_PROFILING)
    // Reference element extracted
    list->cnt--;
    if(list->mincnt > list->cnt)
    {
        list->mincnt = list->cnt;
    }
    #endif //KE_PROFILING
}

void co_list_extract_sublist(struct co_list *list, struct co_list_hdr *ref_hdr, struct co_list_hdr *last_hdr)
{
    // sanity check
    ASSERT_ERR(list != NULL);
    ASSERT_ERR(last_hdr != NULL);

    // Check if the element is first
    if(ref_hdr == NULL)
    {
        // Extract the elements
        list->first = last_hdr->next;
    }
    else
    {
        // Extract the elements
        ref_hdr->next = last_hdr->next;
    }

    // Check if the element is last
    if(last_hdr == list->last)
    {
        // Reference element becomes last
        list->last = ref_hdr;
    }
}

inline bool co_list_find(struct co_list *list,
                  struct co_list_hdr *list_hdr)
{
    struct co_list_hdr *tmp_list_hdr;

    // Go through the list to find the element
    tmp_list_hdr = list->first;

    while ((tmp_list_hdr != list_hdr) && (tmp_list_hdr != NULL))
    {
        tmp_list_hdr = tmp_list_hdr->next;
    }

    return (tmp_list_hdr == list_hdr);
}

void co_list_merge(struct co_list *list1,
                   struct co_list *list2)
{
    // Sanity check: list2 is not supposed to be empty
    ASSERT_ERR(!co_list_is_empty(list2));

    // just copy list elements
    if(co_list_is_empty(list1))
    {
        list1->first = list2->first;
        list1->last  = list2->last;
    }
    // merge lists
    else
    {
        // Append list2 to list1
        list1->last->next = list2->first;
        list1->last = list2->last;

    }

    // Empty list2
    list2->first = NULL;

    #if (KE_PROFILING)
    list1->cnt += list2->cnt;
    list2->cnt = 0;
    #endif //KE_PROFILING
}

void co_list_insert_before(struct co_list *list,
                        struct co_list_hdr *elt_ref_hdr, struct co_list_hdr *elt_to_add_hdr)
{
    // Sanity check
    ASSERT_ERR(elt_to_add_hdr != NULL);
    // If no element referenced
    if(elt_ref_hdr == NULL)
    {
        co_list_push_front(list,elt_to_add_hdr);
    }
    else
    {
        struct co_list_hdr *tmp_list_prev_hdr = NULL;
        struct co_list_hdr *tmp_list_curr_hdr;

        // Go through the list to find the element
        tmp_list_curr_hdr = list->first;

        while ((tmp_list_curr_hdr != elt_ref_hdr) && (tmp_list_curr_hdr != NULL))
        {
            // Save previous element
            tmp_list_prev_hdr = tmp_list_curr_hdr;
            // Get the next element of the list
            tmp_list_curr_hdr = tmp_list_curr_hdr->next;
        }
        // If only one element is available
        if(tmp_list_prev_hdr == NULL)
        {
            co_list_push_front(list,elt_to_add_hdr);
        }
        else
        {
            tmp_list_prev_hdr->next = elt_to_add_hdr;
            elt_to_add_hdr->next = tmp_list_curr_hdr;
            #if (KE_PROFILING)
            list->cnt++;
            if(list->maxcnt < list->cnt)
            {
                list->maxcnt = list->cnt;
            }
            #endif //KE_PROFILING
        }
    }
}

void co_list_insert_after(struct co_list *list,
                        struct co_list_hdr *elt_ref_hdr, struct co_list_hdr *elt_to_add_hdr)
{
    // Sanity check
    ASSERT_ERR(elt_to_add_hdr != NULL);
    // If no element referenced
    if(elt_ref_hdr == NULL)
    {
        co_list_push_back(list,elt_to_add_hdr);
    }
    else
    {
        struct co_list_hdr *tmp_list_curr_hdr;

        // Go through the list to find the element
        tmp_list_curr_hdr = list->first;

        while ((tmp_list_curr_hdr != elt_ref_hdr) && (tmp_list_curr_hdr != NULL))
        {
             // Get the next element of the list
            tmp_list_curr_hdr = tmp_list_curr_hdr->next;
        }
        // If only one element is available
        if(tmp_list_curr_hdr == NULL)
        {
            co_list_push_back(list,elt_to_add_hdr);
        }
        else
        {
            // Check if the found element was the last of the list
            if (!tmp_list_curr_hdr->next)
            {
                // Update last pointer
                list->last = elt_to_add_hdr;
            }

            elt_to_add_hdr->next = tmp_list_curr_hdr->next;
            tmp_list_curr_hdr->next = elt_to_add_hdr;

            #if (KE_PROFILING)
            list->cnt++;
            if(list->maxcnt < list->cnt)
            {
                list->maxcnt = list->cnt;
            }
            #endif //KE_PROFILING
        }
    }
}

uint16_t co_list_size(struct co_list *list)
{
    uint16_t count = 0;
    struct co_list_hdr *tmp_list_hdr = list->first;

    // browse list to count number of elements
    while (tmp_list_hdr != NULL)
    {
        tmp_list_hdr = tmp_list_hdr->next;
        count++;
    }

    return count;
}

/// @} CO_LIST
