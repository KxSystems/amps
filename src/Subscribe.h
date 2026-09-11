#ifndef __SUBSCRIBE_H__
#define __SUBSCRIBE_H__

#include <HelperFunctions.h>


extern "C"
{
  /**
   * @brief Initialise the kamps client by attaching to q's event loop
   * 
   * @param unused 
   * @return null
  */
  K kamps_init(K unused);

  /**
   * @brief Subscribe to a topic and register the specified callback function
   *
   * Supported integer options:
   *
   * .kamps.subTimeoutMs - Length of time in milliseconds to attempt to create
   * the subscriptions.  Default 5000ms.
   *
   * Supported string options:
   *
   * .kamps.subBookmark - If specified subscribes using the specified bookmark
   * such as "recent" (non-discarded), "0" (epoch) or "0|1|" (now)
   *
   * .kamps.subFilter - Specifies a filter to apply to the subscription, e.g.
   * "/streamId = 'xxxx'"
   *
   * .kamps.subOptions - Generic options to be passed verbatim to
   * AMPS::Client::subscribe()
   *
   * @param handle    kdb+ foreign containing client reference
   * @param subid     String containing the name of the subscription to create
   * @param topic     String containing the name of the topic to subscribe to
   * @param callback  String containing the name of the callback function to be
   * invoked
   * @param options   kdb+ dictionary of subscriber options or generic null (::)
   * to use the defaults.  Dictionary key must be a 11h list.  Values list can
   * be 7h, 11h or mixed list of -7|-11|4h.
   * @return          null
  */
  EXP K kamps_subscribe(K handle, K subid, K topic, K callback, K options);

  /**
   * @brief Discard a message from the client's bookmark store
   *
   * @param handle        kdb+ foreign containing client reference
   * @param subid         String containing the subscription name
   * @param bookmark_seq  The bookmark sequence number passed to the
   * subscription callback number
   * @return              null
  */
  EXP K kamps_discard(K handle, K subid, K bookmark_seq);

  /**
   * @brief Remove a subscription to a topic
   *
   * @param handle  kdb+ foreign containing client reference
   * @param subid   String containing the name of the subscription which was
   * specified when it was created
   * @return        null
  */
  EXP K kamps_unsubscribe(K handle, K subid);
}

#endif // __SUBSCRIBE_H__
