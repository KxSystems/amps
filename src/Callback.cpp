#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <cstring>

#include <string>
#include <map>
#include <future>
#include <iostream>

#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
static SOCKET spair[2];
#else
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#define SOCKET_ERROR -1
static int spair[2];
#endif

#include "socketpair.c"

#include "Callback.h"
#include "HelperFunctions.h"
#include <k.h>


// Flag indicating whether socketpair has been initialised
static bool validinit = false;

// Release K object, printing any errors
void pr0(K x)
{
  if (!x)
    return;
  if (-128 == x->t)
    std::cerr << "pr0: " << x->s << std::endl;
  r0(x);
}

// Callback function registered with atexit() to clean up
static void DetachCallback(void)
{
  // Close both ends of the socketpair
  int sp;
  if ((sp = spair[0])) {
    sd0x(sp, 0);
    close(sp);
  }
  if ((sp = spair[1]))
    close(sp);

  // Mark as no longer initialised
  spair[0] = 0;
  spair[1] = 0;
  validinit = false;
}

// Push message onto the socketpair
void pushMessage(const char* callback_fn, K k_msg)
{
  // Mixed list with which to invoke the q callback using value[mixed]
  K msg_data = knk(2, ks((S)callback_fn), k_msg);

  // Push onto the socketpair
  send(spair[1], (char*)&msg_data, sizeof(&msg_data), 0);
}

// Pop message from socketpair and generate callback
void popMessage(K msg_data)
{
  // Invoke q callback
  pr0(k(0, (char*)"value", msg_data, (K)0));
}

// Callback function which is put on the q event loop
K ampsKdbCallback(int fd)
{
  // All the events which we get off the socketpair are fixed length,
  // sizeof(K).
  K msg_data;
  int64_t rc = recv(fd, (char*)&msg_data, sizeof(&msg_data), 0);
  if (rc != sizeof(msg_data)) {
    std::cerr << "recv error: " << rc << ", expected: " << sizeof(&msg_data) << std::endl;
    return (K)0;
  }

  // Process the message
  popMessage(msg_data);

  return (K)0;
}

// Called during initialisation to put ampsKdbCallback onto the q event loop
K attachCallback(void)
{
  if (validinit)
    return krr((S)"Already initialized");

  // Initialise the socketpair
  if (dumb_socketpair(spair, 1) == SOCKET_ERROR) {
    std::cerr << "Socketpair initialization failed: " << strerror(errno) << std::endl;
    return (K)0;
  }

  // Have to use (0 - fd) rather than simple negate, since SOCKET on Windows is unsigned ptr.
  pr0(sd1(0 - spair[0], &ampsKdbCallback));
  validinit = true;

  // Attach cleanup function
  atexit(DetachCallback);

  return (K)0;
}
