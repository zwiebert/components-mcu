#pragma once

#include "weather_provider.hh"
#include <string>

class Weather_Provider_Owm final: public Weather_Provider {

public:

  virtual bool fetch_weather_data(weather_data &data) const { ///< fetch weather data using our URL member
    return fetch_owm_data(data, get_url());
  }

/// Get  URL stored in *this -or- configured URL
  const char* get_url() const {
#ifdef CONFIG_WEATHER_OWM_URL_STRING
 if (m_url == "")
   return CONFIG_WEATHER_OWM_URL_STRING;
#endif
    return m_url.c_str();
  }

  bool set_url(const char *url) {
    m_url = url;
    return true;
  }

  /**
   * \brief       fetch data from api.openweathermap.org
   * \param dst   where the fetched data goes
   * \param url   URL string with APPID from api.openweathermap.org
   */
  static bool fetch_owm_data(weather_data &data, const char *url);

public:
  virtual ~Weather_Provider_Owm();
private:
  std::string m_url;
};
