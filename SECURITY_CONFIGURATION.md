# Local Wi-Fi configuration

The four sketches read WIFI_SSID and WIFI_PASSWORD from a local `secrets.h` next to the `.ino`. Copy each `secrets.example.h` to `secrets.h` and configure the correct network before compiling. Do not upload that file. LoRa1 creates an access point; the three sensor transmitters connect as stations. Configure matching credentials on the corresponding devices/network.

Use a new password if the old network is still active. Editing GitHub does not rotate passwords in hardware. This patch preserves the WiFi API calls and radio/sensor logic. No physical devices were flashed or tested.

Previously published values remain in Git history. Their originals were privately preserved by the owner; never reuse them for new credentials. History rewriting and physical password rotation require separate owner approval/action.
