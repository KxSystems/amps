#ifndef __PUBLISH_STORE_H__
#define __PUBLISH_STORE_H__

#include <amps/MemoryPublishStore.hpp>


class PublisherManagedSequenceStore : public AMPS::MemoryPublishStore
{
public:

  PublisherManagedSequenceStore(size_t blockPerRealloc_)
    : MemoryPublishStore(blockPerRealloc_)
  {
    this->discardUpTo(1);
  }

  virtual amps_uint64_t store(const AMPS::Message& message_)
  {
    return AMPS::BlockPublishStore::store(message_, false);
  }
};

#endif // __PUBLISH_STORE_H__
