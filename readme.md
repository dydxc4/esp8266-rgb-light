# ESP8266 RGB Light

Un proyecto para controlar una tira de luces WS2812 vía web usando un dispositivo ESP8266.

Permite almacenar de manera persistente los ajustes de las luces y de la conectividad WiFi de manera que al apagarse y volverse a encender se inicie con la última configuración establecida.

## Librerías utilizadas
Para este proyecto se utilizaron las siguientes librerias:
* Adafruit NeoPixel v1.10.3
* ArduinoJSON v6.18.5
* ESPAsyncTCP v1.2.2
* ESP Async WebServer v1.2.3
* Bootstrap v5.1

## Hardware requerido
* Módulo ESP-01 ESP8266 con al menos 1MB de memoria flash
* Módulo WS2812 o WS2812B el cual estara conectado a GPIO0 del ESP-01
* Conversor Serial a USB
* Protoboard, resistencias, botones, cables jumper, etc.

## Instrucciones
Para ser utilizado en PlatformIO con cualquier módulo que lleve el chip ESP8266.

1. Clone o descargue en ZIP este repositorio.
2. Abra la carpeta en su editor de código fuente con PlatformIO instalado.
3. Compile y cargue el programa a su placa.

### Archivos para servidor WEB

Los archivos requeridos por el servidor web se encuentran en la carpeta **x-data**. Estos archivos deberán ser comprimidos cada uno en formato GZIP a fin de reducir su tamaño y agilizar su envio a través de la red. Si su SO es Windows y tiene instalado 7-Zip puede emplear el script `compress.bat` para generar la carpeta **data** y comprimir los archivos de forma rápida.

Una vez que tenga la carpeta data simplemente seleccione la opción **Upload Filesystem Image** para cargar la imagen del sistema de archivos al ESP8266.

## Funcionamiento
El funcionamiento es simple: al iniciar el dispositivo ESP8266 por primera vez se activa WiFi en modo AP a la espera de que un usuario se conecte desde un dispositivo (de preferencia móvil) a una red generada de manera predeterminada, en este caso el SSID es **esp8266-light** y la contraseña es **esp32-rocks!**.

En el momento que un usuario se conecte la red generada automáticamente será redireccionado a una página de configuración de WiFi. Aquí el usuario deberá ingresar en los campos correspondientes el nombre de la red deseada, la contraseña (puede permanecer en blanco en caso de no ser requerida), IP local deseada, máscara de red y la puerta de enlace predeterminada. Para aplicar los cambios basta con dar clic en el botón **Actualizar** y en unos instantes el dispositivo ESP8266 se reiniciará.

A partir de ese momento el dispositivo ESP8266 intentará conectarse a la red configurada cada vez que se encienda y en caso de no lograrlo simplemente volverá a activar el modo AP a la espera de que la red sea configurada nuevamente.

**Para acceder a la configuración de las luces o red solamente ingrese desde su navegador preferido a la dirección IP que estableció al momento de configurar la red.**

## Modalidades de luces
De momento solo permite establecer un color sólido o habilitar el modo "arcoíris", encender o apagar las luces, ajustar la intensidad del brillo y la rapidez o frecuencia en la modalidad arcoíris.

## Por hacer
- [ ] Agregar (o terminar) la modalidad de desvanecimiento
- [ ] Agregar programación de encendido y apagado automático
- [ ] Y lo que se me ocurra más adelante xD ...

## Licencia
Públicado bajo la licencia MIT, el proyecto puede ser descargardo y ser usado, modificado o distribuido libremente para el fin que le necesite.
