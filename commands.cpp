#include "commands.h"
#include "clients_manager.h"

namespace cmd
{
std::string SetLedState::Run(Server *srv, std::string arg)
{
  if (!srv)
    throw std::runtime_error("SetLedState::Run - wrong input param");
  if (!arg.compare(ArgsToStr.at(Args::on)))
  {
    std::cout << "turning on LED" << std::endl;
    srv->SetState(true);
  }
  else if (!arg.compare(ArgsToStr.at(Args::off)))
  {
    srv->SetState(false);
  }
  else
  {
    throw std::runtime_error("Wrong argument "
         + arg + " for command " 
        + PrefixesToStr.at(m_prefix));
  }
  return "OK\n";
}

std::string GetLedState::Run(Server *srv, std::string arg)
{
  if (!srv)
    throw std::runtime_error("GetLedState::Run - wrong input param");
  return srv->GetState()
    ? "OK " + ArgsToStr.at(Args::on) + "\n"
    : "OK " + ArgsToStr.at(Args::off) + "\n";
}

std::string SetLedColor::Run(Server *srv, std::string arg)
{
  if (!srv)
    throw std::runtime_error("SetLedColor::Run - wrong input param");
  if (!arg.compare(ArgsToStr.at(Args::red)))
  {
    srv->SetColor(ArgsToStr.at(Args::red));
  }
  else if (!arg.compare(ArgsToStr.at(Args::green)))
  {
    srv->SetColor(ArgsToStr.at(Args::green));
  }
  else if (!arg.compare(ArgsToStr.at(Args::blue)))
  {
    srv->SetColor(ArgsToStr.at(Args::blue));
  }
  else
  {
    throw std::runtime_error("Wrong argument "
         + arg + " for command " 
        + PrefixesToStr.at(m_prefix));
  }
  return "OK\n";
}

std::string GetLedColor::Run(Server *srv, std::string arg)
{
  if (!srv)
    throw std::runtime_error("GetLedColor::Run - wrong input param");

  if (!srv)
    throw std::runtime_error("SetLedColor::Run - wrong input param");
  if (!std::string(srv->GetColor()).compare(ArgsToStr.at(Args::red)))
  {
    return "OK " + std::string(srv->GetColor()) + '\n';
  }
  else if (!std::string(srv->GetColor()).compare(ArgsToStr.at(Args::green)))
  {
    return "OK " + std::string(srv->GetColor()) + '\n';
  }
  else if (!std::string(srv->GetColor()).compare(ArgsToStr.at(Args::blue)))
  {
    return "OK " + std::string(srv->GetColor()) + '\n';
  }
  else
  {
    throw std::runtime_error("Wrong argument "
         + arg + " for command " 
        + PrefixesToStr.at(m_prefix));
  }
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

std::string GetLedRate::Run(Server *srv, std::string arg)
{
  if (!srv)
    throw std::runtime_error("GetLedColor::Run - wrong input param");
  return "OK " + RmTrZr(std::to_string(srv->GetRate())) + "\n";
}

std::string SetLedRate::Run(Server *srv, std::string arg)
{
  if (!srv)
    throw std::runtime_error("GetLedColor::Run - wrong input param");
   srv->SetRate(std::stof(arg));
   return "OK\n";
}

} // end of namespace cmd
