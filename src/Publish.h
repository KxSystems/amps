#ifndef __PUBLISH_H__
#define __PUBLISH_H__

#include <Connection.h>

extern "C" 
{

  /**
   * @brief Publish a message to a topic
   * 
   * Supported options:
   * 
   * None currently (specify ::)
   * 
   * @param handle  kdb+ foreign containing client reference
   * @param topic   String containing name of topic to publish on
   * @param options kdb+ dictionary of publisher options or generic null (::)
    to use the default.  Dictionary key must be a 11h list.  Values list can be
    7h, 11h or mixed list of -7|-11|4h.
   * @param data    Byte or char array of data to publish
   * @return null
  */
  K kamps_publishBinary(K handle, K topic, K options, K data);

  /**
   * @brief Publish a message to a topic at a given sequence num
   * 
   * @param handle  kdb+ foreign containing client reference
   * @param topic   String containing name of topic to publish on
   * @param options kdb+ dictionary of publisher options or generic null (::)
    to use the default.  Dictionary key must be a 11h list.  Values list can be
    7h, 11h or mixed list of -7|-11|4h.
   * @param data    Byte of char array of data to publish
   * @param seq     Sequencer number (long) to publish at
   * @return null
  */
  EXP K kamps_publishBinaryAt(K handle, K topic, K options, K data, K seq);

}


#endif // __PUBLISH_H__
