#include <system.h>
#include "types.h"
#include "errText.h"

#ifdef _ICD2
#include <icd2.h>
#else
#endif // _ICD2
#pragma message "public functions available from this module are prefixed w/'err'"

// only allowed to have 16 chars
// have func which does a lookup and fills in buffer (pntr to user provided buffer)

/*
rom unsigned char * errText = \
"err1.'.'.'.'.'.'\0" \
"err2.'.'.'.'.'.'\0" \
"err3.'.'.'.'.'.'\0" \
"err4.'.'.'.'.'.'\0" \
"err5.'.'.'.'.'.'\0" \
"err6.'.'.'.'.'.'\0" \
"err7.'.'.'.'.'.'\0" \
"err8.'.'.'.'.'.'\0" \
"err9.'.'.'.'.'.'\0" \
"erra.'.'.'.'.'.'\0" \
"errb.'.'.'.'.'.'\0" \
"errc.'.'.'.'.'.'\0" \
"errd.'.'.'.'.'.'\0" \
"erre.'.'.'.'.'.'\0" \
"errf.'.'.'.'.'.'\0" \
"err10'.'.'.'.'.'\0" \
"err11'.'.'.'.'.'\0" \
"err12'.'.'.'.'.'\0" \
"err13'.'.'.'.'.'\0" \
"err14'.'.'.'.'.'\0";

// too big - for some dumbass reason the compiler only lets us work w/256byte string in ROM and not allow me
// to make or use anything larger .. workaround is gonna suck and probably won't be as code efficient as I
// would like  - see below
//
// since I can ONLY create a 'rom char *' pointer at compile time, I can't do any ptr magic at run time, hence
// all of the strcpy()'s
//
// rom is free on this device so I can afford to ballon the code out of control, it's there, might as well use it
// then clean up later if needed :)
//
// this can easily be streamlined using same asm constructs, I want to avoid that for now and make sure it works first

*/

void errFetch(uint8_t * const buff, const uint8_t code) {
	// buff must be a <=16byte buffer
	// code is the raw value from ddc bus, see error.doc in tree to see where I got the descriptions from
	
	// TODO : do a strlen() check on buff
	
	
	switch(code) {
		case 0x01:
			strcpy(buff, (rom char *)"V_12P0");		// ptr stuff here is complicated. since we're working from ROM and based on the way pic's were never intended to use code space to store var's, this is why I do it this way
			goto clip;
		case 0x02:
			strcpy(buff, (rom char *)"V_V_CPUCORE");
			goto clip;
		case 0x03:
			strcpy(buff, (rom char *)"V_GPUCORE0");
			goto clip;
		case 0x04:
			strcpy(buff, (rom char *)"NO_ANA");
			goto clip;
		case 0x05:
			strcpy(buff, (rom char *)"THERMAL_CPU");
			goto clip;
		case 0x06:
			strcpy(buff, (rom char *)"THERMAL_GPU");
			goto clip;
		case 0x07:
			strcpy(buff, (rom char *)"THERMAL_EDRAM");
			goto clip;
		case 0x08:
			strcpy(buff, (rom char *)"GPU_RST_DONE");
			goto clip;
		case 0x09:
			strcpy(buff, (rom char *)"NO_PCIE");
			goto clip;
		case 0x0a:
			strcpy(buff, (rom char *)"NO_HANDSHAKE");
			goto clip;
		case 0x0b:
			strcpy(buff, (rom char *)"NO_CLOCKCHIP");
			goto clip;
		case 0x0c:
			strcpy(buff, (rom char *)"NO_TEMPERATURES");
			goto clip;
		case 0x10:
			strcpy(buff, (rom char *)"NBINIT_MEM_VENDOR_ID");
			goto clip;
		case 0x11:
			strcpy(buff, (rom char *)"NBINIT_MEM_READ_STROBE_DATA_WRITE");
			goto clip;
		case 0x12:
			strcpy(buff, (rom char *)"NBINIT_MEM_READ_STROBE_DELAY_TRAINING");
			goto clip;
		case 0x13:
			strcpy(buff, (rom char *)"NBINIT_MEM_WRITE_STROBE_DELAY_TRAINING");
			goto clip;
		case 0x14:
			strcpy(buff, (rom char *)"MEMORY_ADDRESSING");
			goto clip;
		case 0x15:
			strcpy(buff, (rom char *)"NBINIT__MEM_PAD_CAL_CNTL_ERROR");
			goto clip;
		case 0x30:
			strcpy(buff, (rom char *)"MFG_COMM_FAILURE");
			goto clip;
		case 0x31:
			strcpy(buff, (rom char *)"MFG_UNABLE_TO_CONNECT");
			goto clip;
		case 0x32:
			strcpy(buff, (rom char *)"MFG_CONNECTX_FAILURE");
			goto clip;
		case 0x33:
			strcpy(buff, (rom char *)"MFG_GATEWAY_ERROR");
			goto clip;
		case 0x34:
			strcpy(buff, (rom char *)"MFG_BAD_RESPONSE_DATA");
			goto clip;
		case 0x35:
			strcpy(buff, (rom char *)"MFG_CHALLENGE_REFUSED");
			goto clip;
		case 0x36:
			strcpy(buff, (rom char *)"MFG_BAD_VAULT");
			goto clip;
		case 0x40:
			strcpy(buff, (rom char *)"XSS_CDROM_TIMED_OUT");
			goto clip;
		case 0x41:
			strcpy(buff, (rom char *)"XSS_CDROM_NOT_DMA_CONFIGURED");
			goto clip;
		case 0x42:
			strcpy(buff, (rom char *)"XSS_CDROM_COULD_NOT_CREATE_DEVICE");
			goto clip;
		case 0x43:
			strcpy(buff, (rom char *)"XSS_HDD_TIMED_OUT");
			goto clip;
		case 0x44:
			strcpy(buff, (rom char *)"XSS_HDD_NOT_DMA_CONFIGURED");
			goto clip;
		case 0x45:
			strcpy(buff, (rom char *)"XSS_HDD_COULD_NOT_CREATE_DEVICE");
			goto clip;
		case 0x46:
			strcpy(buff, (rom char *)"XSS_HDD_NOT_FOUND");
			goto clip;
		case 0x47:
			strcpy(buff, (rom char *)"XSS_XAM_NO_DASH");
			goto clip;
		case 0x48:
			strcpy(buff, (rom char *)"SMC_FLASH_CONFIG_CORRUPT");
			goto clip;
		case 0x49:
			strcpy(buff, (rom char *)"HSIO_TRAINING_FAILED");
			goto clip;
		case 0x4a:
			strcpy(buff, (rom char *)"HSIO_CALIBRATION_FAILED");
			goto clip;
		case 0x4b:
			strcpy(buff, (rom char *)"UNEXPECTED_ETHERNET_PHY_VENDOR");
			goto clip;
		case 0x4c:
			strcpy(buff, (rom char *)"ETHERNET_PHY_RESET_FAIL");
			goto clip;
		case 0x4d:
			strcpy(buff, (rom char *)"ETHERNET_PHY_READ_WRITE_FAIL");
			goto clip;
		case 0x4e:
			strcpy(buff, (rom char *)"UNSUPPORTED_GPU");
			goto clip;
		case 0x4f:
			strcpy(buff, (rom char *)"XSS_FLASH_NO_XAM");
			goto clip;
		case 0x9b:
			strcpy(buff, (rom char *)"POST_2BL_E_VERIFY_SECOTP_1");
			goto clip;
		case 0x9c:
			strcpy(buff, (rom char *)"POST_2BL_E_VERIFY_SECOTP_2");
			goto clip;
		case 0x9d:
			strcpy(buff, (rom char *)"POST_2BL_E_VERIFY_SECOTP_3");
			goto clip;
		case 0x9e:
			strcpy(buff, (rom char *)"POST_2BL_E_VERIFY_SECOTP_4");
			goto clip;
		case 0x9f:
			strcpy(buff, (rom char *)"POST_2BL_E_VERIFY_SECOTP_5");
			goto clip;
		case 0xa0:
			strcpy(buff, (rom char *)"POST_2BL_E_VERIFY_SECOTP_6");
			goto clip;
		case 0xa1:
			strcpy(buff, (rom char *)"POST_2BL_E_VERIFY_SECOTP_7");
			goto clip;
		case 0xa2:
			strcpy(buff, (rom char *)"POST_2BL_E_VERIFY_SECOTP_8");
			goto clip;
		case 0xa3:
			strcpy(buff, (rom char *)"POST_2BL_E_VERIFY_SECOTP_9");
			goto clip;
		case 0xa5:
			strcpy(buff, (rom char *)"POST_2BL_E_VERIFY_OFFSET_3BL");
			goto clip;
		case 0xa6:
			strcpy(buff, (rom char *)"POST_2BL_E_LOCATE_3BL");
			goto clip;
		case 0xa7:
			strcpy(buff, (rom char *)"POST_2BL_E_VERIFY_HEADER_3BL");
			goto clip;
		case 0xa8:
			strcpy(buff, (rom char *)"POST_2BL_E_SIG_VERIFY_3BL");
			goto clip;
		case 0xa9:
			strcpy(buff, (rom char *)"POST_2BL_E_HWINIT");
			goto clip;
		case 0xaa:
			strcpy(buff, (rom char *)"POST_2BL_E_VERIFY_OFFSET_4BL");
			goto clip;
		case 0xab:
			strcpy(buff, (rom char *)"POST_2BL_E_VERIFY_HEADER_4BL");
			goto clip;
		case 0xac:
			strcpy(buff, (rom char *)"POST_2BL_E_SIG_VERIFY_4BL");
			goto clip;
		case 0xad:
			strcpy(buff, (rom char *)"POST_2BL_E_SHA_VERIFY_4BL");
			goto clip;
		case 0xae:
			strcpy(buff, (rom char *)"POST_2BL_E_UNEXPECTED_INTERRUPT");
			goto clip;
		case 0xaf:
			strcpy(buff, (rom char *)"POST_2BL_E_UNSUPPORTED_RAM_SIZE");
			goto clip;
		case 0xb1:
			strcpy(buff, (rom char *)"POST_4BL_E_VERIFY_OFFSET");
			goto clip;
		case 0xb2:
			strcpy(buff, (rom char *)"POST_4BL_E_VERIFY_HEADER");
			goto clip;
		case 0xb3:
			strcpy(buff, (rom char *)"POST_4BL_E_SHA_VERIFY");
			goto clip;
		case 0xb4:
			strcpy(buff, (rom char *)"POST_4BL_E_LZX_EXPAND");
			goto clip;
		case 0xb5:
			strcpy(buff, (rom char *)"POST_4BL_E_LOAD_6BL");
			goto clip;
			
		default:
			strcpy(buff, (rom char *)"RESERVED");
			goto clip;
		}
clip:
	buff[15] = '\0';
}

// copy rom constants into buff and then fill provided struct with parsed info
// I hope for this to be a more efficient way to store consts in code space for v2.0
void errLookup(il_t * const il, const uint8_t code) {

uint8_t buffx[48]; // 16 * 3 = 48

	switch(code) {
		default:
			strcpy(buffx, (rom char *)"[[unintialized1]" \
								     "[[unintialized2]" \
								     "[[unintialized3]");

		}
		
hang();

}
