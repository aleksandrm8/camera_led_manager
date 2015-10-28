#include <string>
namespace ff_tools
{
  enum class Res
  {
    TIMED_OUT,
    OK,
    NOBODY_READ_FIFO
  };

  static const int kMaxCmdLenght = 512;

  Res ReadFromFifo(std::string name, std::string &res, size_t timeout);
  Res WriteToFifo(std::string name, std::string cmd, size_t timeout);
}
