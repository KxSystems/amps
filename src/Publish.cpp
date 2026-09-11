#include <Publish.h>
#include <HelperFunctions.h>
#include <KdbOptions.h>

K amps_publish_binary_internal(K handle, K topic, K options, K data, K seq)
{
  if (!IsKdbString(topic))
    return krr((S)"topic expected -11|10h");
  if (data->t != KC && data->t != KG)
    return krr((S)"data expected 4|10h");

  KDB_EXCEPTION_TRY;

  // Process the options dictionary
  const auto& options_reader = KdbOptions(options, PublisherOptions::string_options, PublisherOptions::int_options);

  // Build the command
  AMPS::Command command("publish");
  command.setTopic(GetKdbString(topic));
  command.setData((char*)kG(data), data->n);
  if (seq) {
    if (seq->t == -KI)
      command.setSequence(seq->i);
    else if (seq->t == -KJ)
      command.setSequence(seq->j);
  }

  // Execute the command on the HA client
  auto ha_client = GetHAClient(handle);
  ha_client->executeAsync(command, AMPS::MessageHandler());

  KDB_EXCEPTION_CATCH;

  return (K)0;
}

K kamps_publishBinary(K handle, K topic, K options, K data)
{
  return amps_publish_binary_internal(handle, topic, options, data, nullptr);
}

K kamps_publishBinaryAt(K handle, K topic, K options, K data, K seq)
{
  if (seq->t != -KI && seq->t != -KJ)
    return krr((S)"seq expected -6|-7h");

  return amps_publish_binary_internal(handle, topic, options, data, seq);
}

