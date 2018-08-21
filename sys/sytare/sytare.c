
/******************************************************************/
/****************************************************** INCLUDES **/

#include "board.h"
#include "periph/dma.h"

/******************************************************************/
/*********************************************** EXTERNAL THINGS **/

extern char LINKER_data_start;
extern char LINKER_data_size;
extern char LINKER_data_start_rom;

extern char LINKER_bss_start;
extern char LINKER_bss_size;

extern char LINKER_checkpoint_data_start;
extern char LINKER_checkpoint_bss_start;
extern char LINKER_checkpoint_heap_start;

extern char LINKER_checkpoint0_start;
extern char LINKER_checkpoint1_start;
extern char LINKER_checkpoint_size;

extern void board_init_dco(void);


/******************************************************************/
/*********************************************** TYPES & STRUCTS **/

typedef struct
{
    size_t sp;          ///< stack pointer
    size_t base_addr;   ///< base address of checkpoint section in memory
} syt_checkpoint_t;


/******************************************************************/
/******************************************************** MACROS **/

/// Find the offset of a variable that has been allocated in the .checkpoint
/// section (argument is the address of the variable). The linker will always
/// allocate the variable in the first checkpoint, and since every symbol in C
/// can only have one address, we can only access that variable in another
/// checkpoint by it's offset (which is the same in every checkpoint).
#define GET_OFFSET_IN_CHECKPOINT(addr) ( (size_t) addr - ((size_t) &LINKER_checkpoint0_start) )

/// Reference a variable in the .checkpoint section (read as well as write).
///
/// See GET_OFFSET_IN_CHECKPOINT for more information on why this is needed.
#define GET_CHECKPOINT_VAR(name, type, checkpoint_base_addr) \
    ((type*)( (size_t) checkpoint_base_addr + GET_OFFSET_IN_CHECKPOINT(&name) ))


/******************************************************************/
/***************************************************** VARIABLES **/



// Checkpointing status variables --------------------------------//
// /!\ these must never be changed outside the primary OS routines.
static int syt_first_boot = 1;

// metadata about checkpoints that has to be stored in each checkpoint, it has
// to be allocated in the .checkpoint section
static __attribute__((section(".checkpoint")))
syt_checkpoint_t syt_checkpoint;

// assign at boot to checkpoint{0,1}
static syt_checkpoint_t* syt_last = 0;
static syt_checkpoint_t* syt_next = 0;

// statistics
static uint16_t stats_boot_count = 0;
static uint16_t stats_success_count = 0;


/******************************************************************/
/**************************************** FUNCTIONS DECLARATIONS **/

static inline bool syt_is_first_boot(void);


/******************************************************************/
/********************************************* SYSTEM PRIMITIVES **/

// function used to forgot previously saved state
static void syt_reset_to_first_boot(void)
{
    syt_first_boot = 1;
    syt_next = 0;
    syt_last = 0;
    stats_boot_count = 0;
    stats_success_count = 0;
}


/******************************************************************/
/*************************************************** SYSTEM BOOT **/

bool syt_is_first_boot(void)
{
    return (syt_first_boot == 1);
}

void riot_boot(void)
{
    // platform level init -------------------------------------------//
    board_init_dco();

    // check of broken state
    if(!syt_first_boot && syt_last == 0) {
        syt_reset_to_first_boot();
    }

    // OS init -------------------------------------------------------//
    syt_checkpoint_t* syt_checkpoint0 = GET_CHECKPOINT_VAR(syt_checkpoint, syt_checkpoint_t, ((size_t) &LINKER_checkpoint0_start));
    syt_checkpoint_t* syt_checkpoint1 = GET_CHECKPOINT_VAR(syt_checkpoint, syt_checkpoint_t, ((size_t) &LINKER_checkpoint1_start));

    if(syt_is_first_boot()) {
        // Preparing OS bss section in NVRAM at first boot
        dma_memset((void*)((unsigned int)&LINKER_bss_start), 0, ((unsigned int)&LINKER_bss_size));

        // Clear the checkpointing metadata structure and the actual checkpoints
        dma_memset(syt_checkpoint0, 0, sizeof(syt_checkpoint_t));
        dma_memset(syt_checkpoint1, 0, sizeof(syt_checkpoint_t));
        dma_memset((void*)((size_t) &LINKER_checkpoint0_start), 0, ((size_t) &LINKER_checkpoint_size));
        dma_memset((void*)((size_t) &LINKER_checkpoint1_start), 0, ((size_t) &LINKER_checkpoint_size));

        // initialize base addresses
        syt_checkpoint0->base_addr = ((size_t) &LINKER_checkpoint0_start);
        syt_checkpoint1->base_addr = ((size_t) &LINKER_checkpoint1_start);
    }

    // Initialize the checkpoint pointers
    if(syt_is_first_boot()) {
        // first run: just initialize the next pointer
        syt_next = syt_checkpoint0;
    } else {
        // normal run: initialize next pointer to the other memory zone than the
        //             one pointed by last
        syt_next = (syt_last == syt_checkpoint0) ?
                        syt_checkpoint1 :
                        syt_checkpoint0;
    }

    // statistics
    stats_boot_count++;

}

/*
__attribute__((constructor)) static void startup(void)
{
    * use putchar so the linker links it in: 
    putchar('\n');

    board_init();

    LOG_INFO("RIOT MSP430 hardware initialization complete.\n");

    kernel_init();
}

#include <msp430.h>
#include "periph/dma.h"

void riot_boot(void)
{
    dma_memset((void*)((unsigned int)&LINKER_bss_start),
        0,
        ((unsigned int)&LINKER_bss_size));

    dma_memcpy((void*)((unsigned int)&LINKER_data_start),
        (void*)((unsigned int)&LINKER_data_start_rom),
        ((unsigned int)&LINKER_data_size));
    
    startup();
}
*/
