#ifndef CAMERA_LED_MANAGER_SERVER_H_
#define CAMERA_LED_MANAGER_SERVER_H_

inline std::string DelTrNl(std::string str)
{
  while (str.size()
      && str.at(str.size() - 1) == '\n')
  {
    str = str.substr(0, str.size()-1);
  }
  return str;
}

inline std::string DelSpaces(std::string str)
{
  str.erase(std::remove_if(str.begin(), str.end(), isspace), str.end());
  return str;
}

class Color
{
public:
  enum class Colors
  {
    RED,
    GREEN,
    BLUE
  };
  std::map<Colors, std::string> to_str = 
  {
    {Colors::RED, "red"},
    {Colors::GREEN, "green"},
    {Colors::BLUE, "blue"}
  };
  std::map<std::string, Colors> to_enum = 
  {
    {to_str.at(Colors::RED), Colors::RED},
    {to_str.at(Colors::GREEN), Colors::GREEN},
    {to_str.at(Colors::BLUE), Colors::BLUE}
  };
  Color(Colors color_): m_color(color_)
  {}
  Color(std::string str)
  {
    if (to_enum.find(str) == to_enum.end())
    {
      std::stringstream ss;
      ss << "Color " << str << " not supported. ";
      ss << " Supported colors are: ";
      for (auto iter: to_str)
      {
        ss << iter.second;
        if (iter.second != to_str.rbegin()->second)
          ss << ", ";
      }
      throw std::runtime_error(ss.str());
    }
    m_color = to_enum.at(str);
  }
  operator std::string()
  {
    return to_str.at(m_color);
  }
private:
  Colors m_color;
};



class Server
{
public:
  static Server* Instance()
  {
    static Server *server =
      new Server(false, Color(Color::Colors::BLUE), 0);
    return server;
  }
  bool GetState()
  {
    std::lock_guard<std::mutex> lock(m_state_m);
    return m_state;
  }
  void SetState(bool state)
  {
    std::lock_guard<std::mutex> lock(m_state_m);
    m_state = state;
    const std::string str_state =
      state ? "on" : "off";
    std::cout << "LED state set to "
      << str_state << std::endl;
  }
  Color GetColor()
  {
    std::lock_guard<std::mutex> lock(m_color_m);
    return m_color;
  }
  void SetColor(Color color)
  {
    std::lock_guard<std::mutex> lock(m_color_m);
    m_color = color;
    const std::string color_str = color;
    std::cout << "LED color set to " << color_str << "."
      << std::endl;
  }
  float GetRate()
  {
    std::lock_guard<std::mutex> lock(m_rate_m);
    return m_rate;
  }
  void SetRate(float rate)
  {
    std::lock_guard<std::mutex> lock(m_rate_m);
    m_rate = rate;
    std::cout << "LED rate set to " << rate << "."
      << std::endl;
  }

private:
  Server(bool state, Color color, int rate):
    m_state(state),
    m_color(color),
    m_rate(rate)
  {}
  ~Server() {}
  Server(const Server&) = delete;
  Server& operator=(const Server) = delete;

  bool        m_state; // true - ON; false - OFF
  std::mutex  m_state_m;
  Color       m_color;
  std::mutex  m_color_m;
  float       m_rate;
  std::mutex  m_rate_m;
};

#endif // CAMERA_LED_MANAGER_SERVER_H_
