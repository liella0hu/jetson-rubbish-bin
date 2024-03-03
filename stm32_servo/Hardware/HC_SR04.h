
#ifndef __HC_SR04_H
#define __HC_SR04_H	 
#include "sys.h"
 
 
#define SR04_Trlg PBout(12)// PB12
#define SR04_Echo PBin(13)// PB13	
 
void HC_SR04_IO_Init(void);
u16  Get_SR04_Distance(void);
 
 
 
	 				    
#endif
