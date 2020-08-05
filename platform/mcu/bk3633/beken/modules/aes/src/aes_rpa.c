/**
 ****************************************************************************************
 * *
 * @file aes_rpa.c
 *
 * @brief Definition file for Confirm value generation function c1 for LE Legacy Pairing
 *
 * Copyright (C) RivieraWaves 2017-2018
 *
 * Algorithm:
 *
 *    irk[i] is 128 bits
 *    addr is 48 bits, composed of prand (24 bits) and hash (24 bits)
 *       addr = prand || hash
 *
 *    prand' is 128 bits
 *       prand' = padding || prand
 *
 *    The algorithm finds the index i, where hash = AES-128(irk[i], prand')
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @addtogroup AES
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"
#if (BT_DUAL_MODE || BLE_STD_MODE)
#if (BLE_EMB_PRESENT || BLE_HOST_PRESENT)

#include "arch.h"     // For architecture platform defines
#include "aes_int.h"  // AES internals
#include <string.h>   // for memcpy function
#include "co_utils.h" // for Bit field access
#include "co_math.h"  // for Maths operation
#include "co_endian.h"// For endianess requiements

/*
 * DEFINES
 ****************************************************************************************
 */

/// Generation algorithm step (RAND/HASH)
enum aes_rpa_gen_step
{
    AES_RPA_GEN_RAND,
    AES_RPA_GEN_HASH,
};


/*
 * TYPES DEFINITION
 *****************************************************************************************
 */

/// Structure definition of the AES RPA generation algorithm
struct aes_rpa_gen_env
{
    /// AES Environment structure
    struct aes_func_env  aes_env;
    /// Pointer to IRK
    struct irk* irk;
    /// Padded random number
    uint8_t prand_bis[ENC_DATA_LEN];
    /// Generation algorithm step (RAND/HASH)
    uint8_t step;
};

/// Structure definition of the AES RPA resolution algorithm
struct aes_rpa_resolve_env
{
    /// AES Environment structure
    struct aes_func_env  aes_env;
    /// Callback indicating the result of address resolution
    aes_rpa_func_result_cb res_cb;
    /// Pointer to RAL
    struct irk irk[BLE_RAL_MAX];
    /// Pointer to BD address
    struct bd_addr* addr;
    /// Padded random number
    uint8_t prand_bis[ENC_DATA_LEN];
    /// Current index
    uint8_t index;
};

/*
 * GLOBAL VARIABLE DEFINITION
 *****************************************************************************************
 */


/*
 * MODULE INTERNAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */


/*
 * LOCAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */

__STATIC bool aes_rpa_gen_continue(struct aes_rpa_gen_env* env, uint8_t* aes_res)
{
    bool finished = false;

    switch(env->step)
    {
        case AES_RPA_GEN_RAND:
        {
            memset(&env->prand_bis[0], 0, ENC_DATA_LEN);
            memcpy(&env->prand_bis[0], aes_res, BD_ADDR_PRAND_LEN);

            // Set the address type as resolvable private address (2 msbs of prand)
            env->prand_bis[BD_ADDR_PRAND_LEN - 1] &= 0x3F;
            env->prand_bis[BD_ADDR_PRAND_LEN - 1] |= BD_ADDR_RSLV;

            /*
             * Encrypt PRAND with local IRK, to obtain HASH
             */
            aes_start(&(env->aes_env), &env->irk->key[0], &env->prand_bis[0]);
            env->step = AES_RPA_GEN_HASH;
        }
        break;

        case AES_RPA_GEN_HASH:
        {
            /*
             * Construct RPA, from PRAND and HASH, address provided in LSB->MSB
             *      -----------------------------------------------------------------------------------------
             *      | hash[0:(HASH_LEN-1)] | prand[(HASH_LEN:(BD_ADDR_LEN-1)] |
             *      -----------------------------------------------------------------------------------------
             */
            memcpy(aes_res+BD_ADDR_HASH_LEN, &env->prand_bis[0], BD_ADDR_PRAND_LEN);

            finished = true;
        }
        break;

        default:
        {
            ASSERT_INFO(0, env->step, env->aes_env.src_info);
        }
        break;
    }

    return finished;
}

__STATIC bool aes_rpa_resolve_continue(struct aes_rpa_resolve_env* env, uint8_t* aes_res)
{
    bool finished = false;

    do
    {
        // Compare hash values
        if(memcmp(&env->addr->addr[0], aes_res, BD_ADDR_HASH_LEN))
        {
            // The IRK did not match, jump to next valid IRK
            uint8_t position = env->index + 1;
            for(; position < BLE_RAL_MAX; position++)
            {
                if (memcmp(env->irk[position].key, co_null_key, KEY_LEN))
                    break;
            }
            env->index = position;

            // Check that IRKs have not been all used
            if(env->index < BLE_RAL_MAX)
            {
                // Start AES-128 on next IRK
                aes_start(&(env->aes_env), &env->irk[env->index].key[0], &env->prand_bis[0]);
                break;
            }
        }

        // Indicate process completion
        if(env->res_cb != NULL)
        {
            env->res_cb(env->index, env->aes_env.src_info);
        }

        finished = true;

    } while(0);

    return finished;
}

/*
 * GLOBAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */

void aes_rpa_gen(struct irk* irk, aes_func_result_cb res_cb, uint32_t src_info)
{
    struct aes_rpa_gen_env* env;

    // Allocate AES RPA environment memory
    env = (struct aes_rpa_gen_env*) aes_alloc(sizeof(struct aes_rpa_gen_env), (aes_func_continue_cb) aes_rpa_gen_continue, res_cb, src_info);

    // prepare parameters
    env->irk = irk;

    /*
     * Start by generating a 3-bytes random number PRAND
     */

    // Generate random number
    for (uint8_t i = 0 ; i < KEY_LEN; i++)
    {
        env->prand_bis[i] = co_rand_byte();
    }

    aes_start(&(env->aes_env), &env->irk->key[0], &env->prand_bis[0]);
    env->step = AES_RPA_GEN_RAND;
}

void aes_rpa_resolve(uint8_t nb_irk, struct irk* irk, struct bd_addr* addr, aes_rpa_func_result_cb res_cb, uint32_t src_info)
{
    uint8_t position = 0;
    struct aes_rpa_resolve_env* env;

    // Allocate AES RPA environment memory
    env = (struct aes_rpa_resolve_env*) aes_alloc(sizeof(struct aes_rpa_resolve_env), (aes_func_continue_cb) aes_rpa_resolve_continue, NULL, src_info);

    // prepare parameters
    env->res_cb = res_cb;
    memcpy(env->irk, irk, nb_irk * sizeof(struct irk));
    env->addr = addr;

    /*
     * Address provided in LSB->MSB
     *      -----------------------------------------------------------------------------------------
     *      | hash[0:(HASH_LEN-1)] | prand[(HASH_LEN:(BD_ADDR_LEN-1)] |
     *      -----------------------------------------------------------------------------------------
     *
     * prand_bis is LSB->MSB
     *      --------------------------------------------------------------------------------------
     *      | prand[0:(PRAND_LEN-1)] | 0[(PRAND_LEN):(ENC_LEN-1)]  |
     *      --------------------------------------------------------------------------------------
     */
    // Clear prand_bis
    memset(&env->prand_bis[0], 0x00, ENC_DATA_LEN);

    // Copy prand value in prand_bis
    memcpy(&env->prand_bis[0], &addr->addr[BD_ADDR_HASH_LEN], BD_ADDR_PRAND_LEN);

    // Find the first valid peer IRK
    for(position = 0; position < (BLE_RAL_MAX - 1); position++)
    {
        if (memcmp(env->irk[position].key, co_null_key, KEY_LEN))
            break;
    }

    // Start AES-128 on first valid IRK
    env->index = position;
    aes_start(&(env->aes_env), &env->irk[env->index].key[0], &env->prand_bis[0]);
}

#endif // (BLE_EMB_PRESENT || BLE_HOST_PRESENT)
#endif //(BT_DUAL_MODE || BLE_STD_MODE) 
/// @} AES_RPA
