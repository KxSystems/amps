#ifndef __CALLBACK_H__
#define __CALLBACK_H__

#include <string>

#include "HelperFunctions.h"
#include <k.h>


// Push a message callback onto the kdb event loop
void pushMessage(const char* callback_fn, K k_msg);

// Attach the callbacks to the kdb event loop
K attachCallback(void);

#endif // __CALLBACK_H__
