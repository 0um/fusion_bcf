#ifndef Timestamp_H
#define Timestamp_H

#include <chrono>
#include <string>

class Timestamp
{
public:
  Timestamp();
  Timestamp(const std::string& timestamp, const std::string& format);
  virtual ~Timestamp() = default;

  bool operator<(const Timestamp& other) const;
  const std::string InUTCTime(const std::string& format) const;
  const std::string InLocalTime(const std::string& format) const;

  static bool IsValid(const std::string& timestamp, const std::string& format);

private:
  std::chrono::system_clock::time_point tp;
};

#endif
