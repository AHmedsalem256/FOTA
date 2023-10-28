#ifndef FPEC_INTERFACE
#define FPEC_INTERFACE



/*
 * API    : Function Used to Erase Block of Pages in flash memory
 * Input  : Void
 * Output : Function return nothing
 * */
void FPEC_voidEraseAppArea(void);

/*
 * API    : Function Used to Erase Block of Pages in flash memory
 * Input  : Variable type : u8 >> Page number in flash
 * Output : Function return nothing
 * */

void FPEC_voidFlashPageErase(u8 Copy_u8PageNumber);


/*
 * API    : Function Used to Write date in flash memory
 * Input  :
 *  1 >> Variable type 32 >> ADDRESS of FLASH PAGE
 *  2 >> Pointer type u16 >> Data to be stored
 *  3 >> Variable type u8 >> Length of the data
 * Output : Function return nothing
 * */
void FPEC_voidFlashWrite(u32 Copy_u32Address, u16* Copy_u16Data, u8 Copy_u8Length);

#endif













