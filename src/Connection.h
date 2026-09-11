#ifndef __CONNECTION_H__
#define __CONNECTION_H__

#include <amps/HAClient.hpp>

#include <HelperFunctions.h>

#include "k.h"

// Getter functions for retriving AMPs objects from ConnectionData stored in a
// kdb foreign object
std::shared_ptr<AMPS::HAClient> GetHAClient(K connection);


extern "C"
{

  /**
   * @brief Creates an instance of connection class and returns pointer to
   * class.
   *
   * Supported integer options:
   *
   * .kamps.connFixedRetryDelay - Length of time in milliseconds before
   * attempting to reconnect
   *
   * .kamps.connFixedRetryTimeout - Length of time in milliseconds to continue
   * reconnect retries
   *
   * .kamps.connPubStoreMemSize - Number of blocks to grow the memory backed
   * publish store by when capacity has been exceeded
   *
   * Supported string options:
   *
   * .kamps.connPubStoreFileName - Name to use for the file backed published
   * store
   *
   * .kamps.connBookStoreFileName - Name to use for the mmap file backed
   * bookmard store
   *
   * .kamps.connDisconnectHandler - Handler to be invoked when the client is
   * unexpectedly disconnected from amps server
   *
   *
   * @param clientName  String containing the name of client to be created (has
   * to be unique for amps server)
   * @param uri         String list of uniform resource identifier for amps
   * transport to be connected
   * @param options     kdb+ dictionary of connection options or generic null
   * (::) to use the default.  Dictionary key must be a 11h list.  Values list
   * can be 7h, 11h or mixed list of -7|-11|4h.
   * @return kdb+ foreign object containing the client reference.  The
   * connection is destroyed when its refcount falls to zero.
  */
  K kamps_create(K clientName, K uri, K options);

  /**
   * @brief Initiates the connection to the amps server
   * 
   * @param handle  kdb+ foreign containing client reference
   * @return null
  */
  K kamps_connect(K handle);

  /**
   * @brief Check connection
   * 
   * @param handle  kdb+ foreign containing client reference
   * @return connection state (boolean)
  */
  K kamps_isConnected(K handle);

  /**
   * @brief Disconnect from amps server
   * 
   * @param handle  kdb+ foreign containing client reference
   * @return null
  */
  K kamps_disconnect(K handle);
}

#endif // __CONNECTION_H__
