#include <cstdio>
#include <cstdint>
#include <cstring>

#include <string>

#include "constants.h"

void log_status(float percentage_full, PUMP_STATE state, const char* detail)
{
    std::string json = "{\"level\":";
    json += std::to_string(percentage_full);
    json += ",\"status\":\"";
    json += STATE_STRINGS[state];
    if(strlen(detail))
    {
        json += "\",\"summary\":\"";
        json += detail;
    }
    json += "\"}";

    FILE* f = fopen("/tmp/status.json", "w");
    fwrite(json.c_str(), 1, json.size(), f);
    fclose(f);

    printf("%s\n", json.c_str());
}

void create_status_report(float percentage_full, PUMP_STATE state, const char* detail)
{
    std::string html = R"(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <script>
    setTimeout(() => { location.reload(); }, %d);
  </script>
  <title>Tank Monitor</title>
  <style>
    * {
      margin: 0;
      padding: 0;
      box-sizing: border-box;
    }

    body {
      font-family: Arial, sans-serif;
    }

    .banner {
      background-color: #2c7be5;
      color: white;
      padding: 24px 32px;
      font-size: 1.8rem;
      font-weight: bold;
    }

    .content {
      padding: 32px;
    }

    h2 {
      margin-bottom: 12px;
      color: #333;
    }

    p {
      color: #555;
      line-height: 1.6;
    }
  </style>
</head>
<body>

  <div class="banner">Tank Monitor</div>

  <div class="content">
    <h2>Status</h2>
    <p>Tank is %0.2f%s full</p>
    <p>%s</p>
    <p>%s</p>
  </div>

</body>
</html>)";

    const int32_t BUFFER_SIZE = html.size() + 64;
    char buffer[BUFFER_SIZE];
    memset(buffer, '\0', BUFFER_SIZE);
    snprintf(buffer, BUFFER_SIZE, html.c_str(), PACE_MS/2, percentage_full, "%", STATE_STRINGS[state], detail);

    FILE* f = fopen("/tmp/index.html", "w");
    fwrite(buffer, 1, strlen(buffer), f);
    fclose(f);
}

void update_status(float percentage_full, PUMP_STATE state, const char* detail)
{
    log_status(percentage_full, state, detail);
    create_status_report(percentage_full, state, detail);
}
