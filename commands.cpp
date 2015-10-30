#include "commands.h"
#include "clients_manager.h"

namespace cmd
{
std::string SetLedState::Run(Server &srv, std::string arg)
{
  if (!arg.compare(ArgsToStr.at(Args::on)))
  {
    srv.SetState(true);
  }
  else if (!arg.compare(ArgsToStr.at(Args::off)))
  {
    Server::Instance().SetState(false);
  }
  else
  {
    throw std::runtime_error("Wrong argument " + arg);
  }
  return "OK\n";
}

std::string GetLedState::Run(Server &srv, std::string arg)
{
  return srv.GetState()
    ? "OK " + ArgsToStr.at(Args::on) + "\n"
    : "OK " + ArgsToStr.at(Args::off) + "\n";
}

std::string SetLedColor::Run(Server &srv, std::string arg)
{
  if (!arg.compare(ArgsToStr.at(Args::red)))
  {
    srv.SetColor(ArgsToStr.at(Args::red));
  }
  else if (!arg.compare(ArgsToStr.at(Args::green)))
  {
    Server::Instance().SetColor(ArgsToStr.at(Args::green));
  }
  else if (!arg.compare(ArgsToStr.at(Args::blue)))
  {
    Server::Instance().SetColor(ArgsToStr.at(Args::blue));
  }
  else
  {
    std::stringstream ss;
    ss << "Wrong argument "
      << arg << ". "
      << "Supported arguments for this command: ";
    for (auto iter: ArgsToStr)
    {
      ss << iter.second;
      if (iter != *ArgsToStr.rbegin())
        ss << ", ";
    }
    throw std::runtime_error(ss.str());
  }
  return "OK\n";
}

std::string GetLedColor::Run(Server &srv, std::string arg)
{
  return "OK " + std::string(srv.GetColor()) + '\n';
}

inline std::string RmTrZr(std::string str)
{
  size_t lenght = str.size();
  for (std::string::reverse_iterator iter = str.rbegin();
        iter != str.rend();
        iter++)
  {
    switch (*iter)
    {
      case '0':
        lenght--;
        continue;
      case '.':
      case ',':
        lenght--;
    }
    break;
  }
  return str.substr(0, lenght);
}

std::string GetLedRate::Run(Server &srv, std::string arg)
{
  return "OK " + RmTrZr(std::to_string(srv.GetRate())) + "\n";
}

std::string SetLedRate::Run(Server &srv, std::string arg)
{
   srv.SetRate(std::stof(arg));
   return "OK\n";
}

} // end of namespace cmd
