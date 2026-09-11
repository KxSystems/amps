#ifndef __KDB_OPTIONS__
#define __KDB_OPTIONS__

#include <string>
#include <map>
#include <stdexcept>
#include <cctype>
#include <set>

#include "k.h"


// Supported publisher options
namespace PublisherOptions
{
// Int options

// String options


const static std::set<std::string> int_options = {
};
const static std::set<std::string> string_options = {
};
}


// Supported subscriber options
namespace SubscriberOptions
{
// Int options
const std::string SUB_TIMEOUT_MS = "timeout";

// String options
const std::string SUB_BOOKMARK = "bookmark";
const std::string SUB_FILTER   = "filter";
const std::string SUB_OPTIONS  = "options";

const static std::set<std::string> int_options = {
  SUB_TIMEOUT_MS,
};
const static std::set<std::string> string_options = {
  SUB_BOOKMARK,
  SUB_FILTER,
  SUB_OPTIONS,
};
}

// Supported connection options
namespace ConnectionOptions
{
// Int options
const std::string CONN_FIXED_RETRY_DELAY   = "fixedRetryDelay";
const std::string CONN_FIXED_RETRY_TIMEOUT = "fixedRetryTimeout";
const std::string CONN_PUBSTORE_MEM_RESIZE = "pubStoreMemResize";

// String options
const std::string CONN_PUBSTORE_FILE_NAME  = "pubStoreFileName";
const std::string CONN_BOOKSTORE_FILE_NAME = "bookStoreFileName";
const std::string CONN_DISCONNECT_HANDLER  = "disconnectHandler";

const static std::set<std::string> int_options = {
  CONN_FIXED_RETRY_DELAY,
  CONN_FIXED_RETRY_TIMEOUT,
  CONN_PUBSTORE_MEM_RESIZE,
};

const static std::set<std::string> string_options = {
  CONN_PUBSTORE_FILE_NAME,
  CONN_BOOKSTORE_FILE_NAME,
  CONN_DISCONNECT_HANDLER,
};
}

// Helper class for reading dictionary of options
//
// Dictionary key:    KS
// Dictionary value:  KS or
//                    KJ or
//                    0 of -KS|-KJ|KC
class KdbOptions
{
private:
  std::map<std::string, std::string> string_options;
  std::map<std::string, int64_t> int_options;

  const std::set<std::string>& supported_string_options;
  const std::set<std::string>& supported_int_options;

private:
  const std::string ToUpper(std::string str) const
  {
    std::string upper;
    for (auto i : str)
      upper.push_back((unsigned char)std::toupper(i));
    return upper;
  }

  void ToUpper(std::string& str)
  {
    for (size_t i = 0; i < str.length(); ++i)
      str[i] = (unsigned char)std::toupper(str[i]);
  }

  void PopulateIntOptions(K keys, K values)
  {
    for (auto i = 0ll; i < values->n; ++i) {
      const std::string key = kS(keys)[i];
      if (supported_int_options.find(key) == supported_int_options.end())
        throw InvalidOption(("Unsupported int option '" + key + "'").c_str());
      int_options[key] = kI(values)[i];
    }
  }

  void PopulateLongOptions(K keys, K values)
  {
    for (auto i = 0ll; i < values->n; ++i) {
      const std::string key = kS(keys)[i];
      if (supported_int_options.find(key) == supported_int_options.end())
        throw InvalidOption(("Unsupported int option '" + key + "'").c_str());
      int_options[key] = kJ(values)[i];
    }
  }

  void PopulateStringOptions(K keys, K values)
  {
    for (auto i = 0ll; i < values->n; ++i) {
      const std::string key = kS(keys)[i];
      if (supported_string_options.find(key) == supported_string_options.end())
        throw InvalidOption(("Unsupported string option '" + key + "'").c_str());
      string_options[key] = kS(values)[i];
    }
  }

  void PopulateMixedOptions(K keys, K values)
  {
    for (auto i = 0ll; i < values->n; ++i) {
      const std::string key = kS(keys)[i];
      K value = kK(values)[i];
      switch (value->t) {
      case -KI:
        if (supported_int_options.find(key) == supported_int_options.end())
          throw InvalidOption(("Unsupported int option '" + key + "'").c_str());
        int_options[key] = value->i;
        break;
      case -KJ:
        if (supported_int_options.find(key) == supported_int_options.end())
          throw InvalidOption(("Unsupported int option '" + key + "'").c_str());
        int_options[key] = value->j;
        break;
      case -KS:
        if (supported_string_options.find(key) == supported_string_options.end())
          throw InvalidOption(("Unsupported string option '" + key + "'").c_str());
        string_options[key] = value->s;
        break;
      case KC:
      {
        if (supported_string_options.find(key) == supported_string_options.end())
          throw InvalidOption(("Unsupported string option '" + key + "'").c_str());
        string_options[key] = std::string((char*)kG(value), value->n);
        break;
      }
      case 101:
        // Ignore ::
        break;
      default:
        throw InvalidOption(("option '" + key + "' value not -7|-11|10h").c_str());
      }
    }
  }

public:
  class InvalidOption : public std::invalid_argument
  {
  public:
    InvalidOption(const std::string message) : std::invalid_argument(message.c_str())
    {};
  };

  KdbOptions(K options, const std::set<std::string> supported_string_options_, const std::set<std::string> supported_int_options_) :
    supported_string_options(supported_string_options_), supported_int_options(supported_int_options_)
  {
    if (options != NULL && options->t != 101) {
      if (options->t != 99)
        throw InvalidOption("options not -99h");
      K keys = kK(options)[0];
      if (keys->t != KS)
        throw InvalidOption("options keys not 11h");
      K values = kK(options)[1];
      switch (values->t) {
      case KJ:
        PopulateLongOptions(keys, values);
        break;
      case KI:
        PopulateIntOptions(keys, values);
        break;
      case KS:
        PopulateStringOptions(keys, values);
        break;
      case 0:
        PopulateMixedOptions(keys, values);
        break;
      default:
        throw InvalidOption("options values not 6|7|11|0h");
      }
    }
  }

  bool GetStringOption(const std::string key, std::string& result) const
  {
    const auto it = string_options.find(key);
    if (it == string_options.end())
      return false;
    else {
      result = it->second;
      return true;
    }
  }

  bool GetIntOption(const std::string key, int64_t& result) const
  {
    const auto it = int_options.find(key);
    if (it == int_options.end())
      return false;
    else {
      result = it->second;
      return true;
    }
  }
};


#endif // __KDB_OPTIONS__
