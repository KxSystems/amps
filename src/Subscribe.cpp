#include <Subscribe.h>
#include <Callback.h>
#include <HelperFunctions.h>
#include <KdbOptions.h>
#include <Connection.h>


K kamps_init(K unused)
{
  setm(1);
  attachCallback();

  return (K)0;
}

inline K FieldAsKC(const AMPS::Message::Field& field)
{
  K result = ktn(KC, field.len());
  std::memcpy(kG(result), field.data(), field.len());
  return result;
}

void ampsSubMessageHandler(const AMPS::Message& message, void* userData)
{
  const char* callback_fn = (char*)userData;

  // Create the keys
  K keys = ktn(KS, 5);
  size_t i = 0;
  kS(keys)[i++] = ss((char*)"bookmark");
  kS(keys)[i++] = ss((char*)"data");
  kS(keys)[i++] = ss((char*)"topic");
  kS(keys)[i++] = ss((char*)"subId");
  kS(keys)[i++] = ss((char*)"bookmarkSeqNo");

  // Create the values
  K k_bookmark = FieldAsKC(message.getBookmark());
  const auto& msg_data = message.getData();
  K k_data = ktn(KG, msg_data.len());
  std::memcpy(kG(k_data), msg_data.data(), msg_data.len());
  K k_topic = FieldAsKC(message.getTopic());
  K k_subid = FieldAsKC(message.getSubId());
  K k_bookmark_seq = kj(message.getBookmarkSeqNo());
  K data = knk(5, k_bookmark, k_data, k_topic, k_subid, k_bookmark_seq);

  pushMessage(callback_fn, xD(keys, data));
}

K kamps_subscribe(K handle, K subid, K topic, K callback, K options)
{
  if (!IsKdbString(subid))
    return krr((S)"subid expected -11|10h");
  if (!IsKdbString(topic))
    return krr((S)"topic expected -11|10h");
  if (!IsKdbString(callback))
    return krr((S)"callback expected -11|10h");

  std::string result;

  KDB_EXCEPTION_TRY;

  // Process the options dictionary
  const auto& options_reader = KdbOptions(options, SubscriberOptions::string_options, SubscriberOptions::int_options);

  int64_t timeout = 5000;
  options_reader.GetIntOption(SubscriberOptions::SUB_TIMEOUT_MS, timeout);

  std::string filter;
  options_reader.GetStringOption(SubscriberOptions::SUB_FILTER, filter);

  std::string bookmark;
  auto have_bookmark = options_reader.GetStringOption(SubscriberOptions::SUB_BOOKMARK, bookmark);

  std::string options_str;
  options_reader.GetStringOption(SubscriberOptions::SUB_OPTIONS, options_str);

  // Make a copy of the callback_fn name.  Deliberately using a raw malloc which
  // isn't freed.  This is in case ampsMessageHandler is invoked after the
  // connection has been garbage collected by kdb+, so can't put in ha_client.
  auto callback_str = GetKdbString(callback);
  void* callback_fn = std::malloc(callback_str.length() + 1);
  std::strcpy((char*)callback_fn, callback_str.c_str());

  // Get the HA client
  auto ha_client = GetHAClient(handle);

  // Create the subscription
  if (have_bookmark) {
    result = ha_client->bookmarkSubscribe(AMPS::MessageHandler(ampsSubMessageHandler, callback_fn), GetKdbString(topic), timeout, bookmark, filter, options_str, GetKdbString(subid));
  } else {
    result = ha_client->subscribe(AMPS::MessageHandler(ampsSubMessageHandler, callback_fn), GetKdbString(topic), timeout, filter, options_str, GetKdbString(subid));
  }

  KDB_EXCEPTION_CATCH;

  return (K)0;
}

K kamps_discard(K handle, K subid, K bookmark_seq_no)
{
  if (!IsKdbString(subid))
    return krr((S)"subid expected -11|10h");
  if (bookmark_seq_no->t != -KJ)
    return krr((S)"bookmark_seq_no expected -7h");

  KDB_EXCEPTION_TRY;

  // Get the HA client
  auto ha_client = GetHAClient(handle);

  // Discard the message
  ha_client->getBookmarkStore().discard(GetKdbString(subid), bookmark_seq_no->j);

  KDB_EXCEPTION_CATCH;

  return (K)0;
}

K kamps_unsubscribe(K handle, K subid)
{
  if (!IsKdbString(subid))
    return krr((S)"subid expected -11|10h");

  KDB_EXCEPTION_TRY;

  // Get the HA client
  auto ha_client = GetHAClient(handle);

  // Remove the subscription
  ha_client->unsubscribe(GetKdbString(subid));

  KDB_EXCEPTION_CATCH;

  return (K)0;
}