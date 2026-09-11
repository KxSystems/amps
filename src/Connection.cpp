#include <amps/HAClient.hpp>
#include <amps/DefaultServerChooser.hpp>

#include <list>

#include <Connection.h>
#include <HelperFunctions.h>
#include <KdbOptions.h>
#include <PublishStore.h>
#include <Callback.h>


// Class returned as kdb foreign object when creating a connection
class ConnectionData
{
private:
  // amps-client state
  size_t startMsgID;
  std::shared_ptr<AMPS::HAClient> client;

private:
  // Constructor is private and only used by KdbConstructor static function 
  ConnectionData(std::shared_ptr<AMPS::HAClient> client_) :
    client(client_)
  {};

public:
  // InvalidConnection is thrown if an invalid foreign object is specified
  class InvalidConnection : public std::invalid_argument
  {
  public:
    InvalidConnection(std::string message) : std::invalid_argument(message.c_str())
    {};
  };

public:
  ~ConnectionData();

  // KdbDestructor is registered as foreign object destructor callback
  static K KdbDestructor(K connection);

  // Creates the ConnectionData object and wraps it in a kdb
  // foreign object
  static K KdbConstructor(K clientName, K uri, K options);

  // Returns the ConnectionData object that has been wrapped in a kdb foreign
  // object
  static ConnectionData* Get(K connection);

  // Getter functions for object members
  std::shared_ptr<AMPS::HAClient> HAClient();

  void Connect();
  bool IsConnected();
  void Disconnect();
};


void ampsDisconnectHandler(AMPS::Client& client, void* userData)
{
  const char* callback_fn = (char*)userData;

  // Create the keys
  K keys = ktn(KS, 1);
  size_t i = 0;
  kS(keys)[i++] = ss((char*)"clientName");

  // Create the values
  K k_client_name = kpn(client.getName());
  K data = knk(1, k_client_name);

  pushMessage(callback_fn, xD(keys, data));
}

ConnectionData::~ConnectionData()
{
  Disconnect();
}

K ConnectionData::KdbDestructor(K connection)
{
  KDB_EXCEPTION_TRY;

  const auto* conn_data = ConnectionData::Get(connection);

  std::cout << "Connection destructor called" << std::endl;

  // Delete the ConnectionData object.  This will decrement the refcounts of the
  // shared pointers to allow them to be deleted. 
  delete conn_data;

  return (K)0;

  KDB_EXCEPTION_CATCH;
}

K ConnectionData::KdbConstructor(K clientName, K uri, K options)
{
  if (!IsKdbString(clientName))
    return krr((S)"clientName expected -11|10h");
  if (uri->t != KS && uri->t != 0)
    return krr((S)"uri expected 11|0 of 10h");
  if (options->t != 101 && options->t != 99)
    return krr((S)"options expected 99|101h");

  // Process the options dictionary
  const auto& options_reader = KdbOptions(options, ConnectionOptions::string_options, ConnectionOptions::int_options);

  // RETRY OPTIONS
  int64_t fixed_retry_delay = 1000;
  bool have_fixed_retry_delay = options_reader.GetIntOption(ConnectionOptions::CONN_FIXED_RETRY_DELAY, fixed_retry_delay);

  int64_t fixed_retry_timeout = 10000;
  bool have_fixed_retry_timeout = options_reader.GetIntOption(ConnectionOptions::CONN_FIXED_RETRY_TIMEOUT, fixed_retry_timeout);

  if (have_fixed_retry_delay != have_fixed_retry_timeout)
    return krr((S)std::string("Only one of " + ConnectionOptions::CONN_FIXED_RETRY_DELAY + ", " + ConnectionOptions::CONN_FIXED_RETRY_TIMEOUT + " set").c_str());

  // STORE OPTIONS
  int64_t pub_store_mem_resize = 10 * 1024;
  bool have_pub_store_mem_resize = options_reader.GetIntOption(ConnectionOptions::CONN_PUBSTORE_MEM_RESIZE, pub_store_mem_resize);

  std::string pub_store_file_name;
  bool have_pub_store_file_name = options_reader.GetStringOption(ConnectionOptions::CONN_PUBSTORE_FILE_NAME, pub_store_file_name);

  if (have_pub_store_mem_resize && have_pub_store_file_name)
    return krr((S)std::string("Cannot specify both " + ConnectionOptions::CONN_PUBSTORE_MEM_RESIZE + " and " + ConnectionOptions::CONN_PUBSTORE_FILE_NAME).c_str());
  else if (!(have_pub_store_mem_resize || have_pub_store_file_name))
    std::cout << "cppcode (Kamps): No publish store specified" << std::endl;

  std::string book_store_file_name;
  bool have_book_store_file_name = options_reader.GetStringOption(ConnectionOptions::CONN_BOOKSTORE_FILE_NAME, book_store_file_name);

  // DISCONNECT OPTIONS
  std::string disconnect_handler;
  bool have_disconnect_handler = options_reader.GetStringOption(ConnectionOptions::CONN_DISCONNECT_HANDLER, disconnect_handler);
  
  // Create the server chooser
  AMPS::ServerChooserImpl* chooser = new AMPS::DefaultServerChooser();
  if (uri->t == KS) {
    for (auto i = 0; i < uri->n; ++i) {
      chooser->add(kS(uri)[i]);
    }
  } else {
    for (auto i = 0; i < uri->n; ++i) {
      if (!IsKdbString(kK(uri)[i]))
        return krr((S)"uri expected 11|0 of 10h");
      else 
        chooser->add(GetKdbString(kK(uri)[i]));
    }
  }

  // Create the reconnect strategy
  AMPS::ReconnectDelayStrategyImpl* reconnect_strategy = nullptr;
  if (have_fixed_retry_delay && have_fixed_retry_timeout) {
    // Create the fixed delay retry
    reconnect_strategy = new AMPS::FixedDelayStrategy((int)fixed_retry_delay, (int)fixed_retry_timeout);
  }

  // Create the publish store
  AMPS::BlockPublishStore* pub_store = nullptr;
  if (have_pub_store_mem_resize) {
    pub_store = new PublisherManagedSequenceStore(pub_store_mem_resize);
    std::cout << "cppcode (Kamps): memory backed publish store configured" << std::endl;
  } else if (have_pub_store_file_name) {
    pub_store = new AMPS::PublishStore(pub_store_file_name);
    std::cout << "cppcode (Kamps): file backed publish store configured" << std::endl;
  }
    
  // Create the bookmark store
  AMPS::BookmarkStoreImpl* book_store = nullptr;
  if (have_book_store_file_name) {
    std::cout << "cppcode (Kamps): mmap file backed bookmark store configured" << std::endl;
    book_store = new AMPS::MMapBookmarkStore(book_store_file_name);
  } else {
    std::cout << "cppcode (Kamps): memory backed bookmark store configured" << std::endl;
    book_store = new AMPS::MemoryBookmarkStore();
  }

  // Create the HA client
  auto client = std::make_shared<AMPS::HAClient>();
  auto client_name = GetKdbString(clientName);
  client->setName(client_name);
  client->setServerChooser(chooser);
  if (reconnect_strategy) {
    client->setReconnectDelayStrategy(reconnect_strategy);
    reconnect_strategy = nullptr;
  }
  if (pub_store) {
    client->setPublishStore(pub_store);
    pub_store = nullptr;
  }
  if (book_store) {
    client->setBookmarkStore(book_store);
    book_store = nullptr;
  }

  // Register the disconnect handler
  if (have_disconnect_handler) {
    // Make a copy of the disconnect_handler name.  Deliberately using a raw
    // malloc which isn't freed.  This is in case ampsDisconnectHandler is
    // invoked after the connection has been garbage collected by kdb+, so can't
    // put in ha_client.
    void* disconnect_fn = std::malloc(disconnect_handler.length() + 1);
    std::strcpy((char*)disconnect_fn, disconnect_handler.c_str());
    // TODO: this function is deprecated. Replacing the disconnect handler on an HAClient is not allowed.
    // Use a ConnectionStateListener to monitor connection state
    client->setDisconnectHandler(AMPS::DisconnectHandler(ampsDisconnectHandler, disconnect_fn));
  }

  // Create a ConnectionData object and wrap in a kdb foreign object. Use a raw
  // new rather than a smart point since kdb will be controlling the lifetime
  // via its refcount
  const auto* conn_data = new ConnectionData(client);
  K result = knk(2, ConnectionData::KdbDestructor, conn_data);
  result->t = 112;

  if (reconnect_strategy)
    delete reconnect_strategy;
  if (pub_store)
    delete pub_store;
  if (book_store)
    delete book_store;

  return result;
}

ConnectionData* ConnectionData::Get(K connection)
{
  if (connection->t != 112)
    throw InvalidConnection("connection expected 112h");

  if (connection->n != 2)
    throw InvalidConnection("connection length expected 2");

  return (ConnectionData*)kK(connection)[1];
}

std::shared_ptr<AMPS::HAClient> ConnectionData::HAClient()
{
  return client;
}

std::shared_ptr<AMPS::HAClient> GetHAClient(K connection)
{
  return ConnectionData::Get(connection)->HAClient();
}

K kamps_create(K clientName, K uri, K options)
{
  KDB_EXCEPTION_TRY;

  return ConnectionData::KdbConstructor(clientName, uri, options);

  KDB_EXCEPTION_CATCH;
}

void ConnectionData::Connect()
{
  if (IsConnected())
    Disconnect();

  client->getReconnectDelayStrategy().reset();
  client->connectAndLogon();
  if (client->getPublishStore().isValid())
    startMsgID = client->getPublishStore().getLastPersisted();
}

K kamps_connect(K handle)
{
  KDB_EXCEPTION_TRY;

  ConnectionData::Get(handle)->Connect();
  return (K)0;

  KDB_EXCEPTION_CATCH;
}

bool ConnectionData::IsConnected()
{
  return !client->disconnected();
}

K kamps_isConnected(K handle)
{
  KDB_EXCEPTION_TRY;

  return kb(ConnectionData::Get(handle)->IsConnected());

  KDB_EXCEPTION_CATCH;
}

void ConnectionData::Disconnect()
{
  if (!client->disconnected())
    client->disconnect();
}

K kamps_disconnect(K handle)
{
  KDB_EXCEPTION_TRY;

  ConnectionData::Get(handle)->Disconnect();
  return (K)0;

  KDB_EXCEPTION_CATCH;
}
