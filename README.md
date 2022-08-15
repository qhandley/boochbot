```text
 ____    ___    ___      __  __ __  ____    ___   ______ 
|    \  /   \  /   \    /  ]|  T  T|    \  /   \ |      T
|  o  )Y     YY     Y  /  / |  l  ||  o  )Y     Y|      |
|     T|  O  ||  O  | /  /  |  _  ||     T|  O  |l_j  l_j
|  O  ||     ||     |/   \_ |  |  ||  O  ||     |  |  |  
|     |l     !l     !\     ||  |  ||     |l     !  |  |  
l_____j \___/  \___/  \____jl__j__jl_____j \___/   l__j  
```

### Overview

This project extends the esp-idf [restful server](https://github.com/espressif/esp-idf/tree/master/examples/protocols/http_server/restful_server) example by adding support for the ds18b20 temperature probe. This probe may be used to measure temperatures of kombucha brews via the frontend browser UI served by the ESP32's HTTP server.

Ideal temperature ranges for kombucha brews vary but are usually in the range of 23-24C/73-75F. Future plans for this project include adding a PID-controlled heat element for keeping brews at stable temperatures as well as adding greater web UI support for dialing in brew temperatures and monitoring the temperature of brews over time.

### Build and Flash

```bash
sudo ./docker_build.sh
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

Commands which communicate with the development board, such as `idf.py flash` and `idf.py monitor`, will not work in the container unless the serial port is passed through into the container. For now we can just run these commands outside of the container.

See the README.md in the esp-idf [restful server](https://github.com/espressif/esp-idf/blob/master/examples/protocols/http_server/restful_server/README.md) example for more information on configuration/setup.

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.