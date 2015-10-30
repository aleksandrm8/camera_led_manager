#include <map>
#include <memory>
#include <set>

#ifndef CAMERA_LED_MANAGER_H_
#define CAMERA_LED_MANAGER_H_

class Server;

namespace cmd
{
enum class Prefixes
{
  set_led_state,
  get_led_state,
  set_led_color,
  get_led_color,
  set_led_rate,
  get_led_rate
};

const std::map<Prefixes, std::string> PrefixesToStr =
{
  {Prefixes::set_led_color, "set-led-color"},
  {Prefixes::get_led_color, "get-led-color"},
  {Prefixes::set_led_state, "set-led-state"},
  {Prefixes::get_led_state, "get-led-state"},
  {Prefixes::set_led_rate, "set-led-rate"},
  {Prefixes::get_led_rate, "get-led-rate"}
};

const std::map<std::string, Prefixes> StrToPrefixes =
{
  {PrefixesToStr.at(Prefixes::set_led_color), Prefixes::set_led_color},
  {PrefixesToStr.at(Prefixes::get_led_color), Prefixes::get_led_color},
  {PrefixesToStr.at(Prefixes::set_led_state), Prefixes::set_led_state},
  {PrefixesToStr.at(Prefixes::get_led_state), Prefixes::get_led_state},
  {PrefixesToStr.at(Prefixes::set_led_rate), Prefixes::set_led_rate},
  {PrefixesToStr.at(Prefixes::get_led_rate), Prefixes::get_led_rate}
};

class BaseCmd
{
public:
  BaseCmd(Prefixes prefix_id): m_prefix(prefix_id)
  {}
  virtual ~BaseCmd() {}
  virtual std::string Run(Server &srv, std::string) = 0;
protected:
  Prefixes m_prefix;
};

class LedState: public BaseCmd
{
public:
  LedState(Prefixes prefix_id):
    BaseCmd(prefix_id)
  {}
protected:
  enum class Args
  {
    on,
    off
  };
  const std::map<Args, std::string> ArgsToStr =
  {
    {Args::on, "on"},
    {Args::off, "off"}
  };
  const std::map<std::string, Args> StrToArgs =
  {
    {"on", Args::on},
    {"off", Args::off}
  };
};

class SetLedState: public LedState
{
public:
  SetLedState(Prefixes prefix_id):
    LedState(prefix_id)
  {}
  std::string Run(Server &srv, std::string arg) override;
};

class GetLedState: public LedState
{
public:
  GetLedState(Prefixes prefix_id):
    LedState(prefix_id)
  {}
  std::string Run(Server &srv, std::string arg) override;
};

class LedColor: public BaseCmd
{
public:
  LedColor(Prefixes prefix_id):
    BaseCmd(prefix_id)
  {}
protected:
  enum class Args
  {
    red,
    green,
    blue
  };
  const std::map<Args, std::string> ArgsToStr =
  {
    {Args::red, "red"},
    {Args::green, "green"},
    {Args::blue, "blue"}
  };
  const std::map<std::string, Args> StrToArgs =
  {
    {ArgsToStr.at(Args::red), Args::red},
    {ArgsToStr.at(Args::green), Args::green},
    {ArgsToStr.at(Args::blue), Args::blue}
  };
};

class SetLedColor: public LedColor
{
public:
  SetLedColor(Prefixes prefix_id):
    LedColor(prefix_id)
  {}
  std::string Run(Server &srv, std::string arg) override;
};

class GetLedColor: public LedColor
{
public:
  GetLedColor(Prefixes prefix_id):
    LedColor(prefix_id)
  {}
  std::string Run(Server &srv, std::string arg) override;
};

class SetLedRate: public BaseCmd
{
public:
  SetLedRate(Prefixes prefix_id):
    BaseCmd(prefix_id)
  {}
  std::string Run(Server &srv, std::string arg) override;
protected:
};

class GetLedRate: public BaseCmd
{
public:
  GetLedRate(Prefixes prefix_id):
    BaseCmd(prefix_id)
  {}
  std::string Run(Server &srv, std::string arg) override;
protected:
};

const std::map<Prefixes, std::shared_ptr<BaseCmd>> Commands =
{
  {Prefixes::get_led_state, std::make_shared<GetLedState>(Prefixes::get_led_state)},
  {Prefixes::set_led_state, std::make_shared<SetLedState>(Prefixes::set_led_state)},
  {Prefixes::get_led_color, std::make_shared<GetLedColor>(Prefixes::get_led_color)},
  {Prefixes::set_led_color, std::make_shared<SetLedColor>(Prefixes::set_led_color)},
  {Prefixes::get_led_rate, std::make_shared<GetLedRate>(Prefixes::get_led_rate)},
  {Prefixes::set_led_rate, std::make_shared<SetLedRate>(Prefixes::set_led_rate)}
};

} //end of namespace cmd

#endif
