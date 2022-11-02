# PRÁCTICA BINOMIO AGUA Y ENERGIA

En esta práctica se va a dessarrollar una plataforma IoT básica, para aprender los principales conceptos detras de esta tecnologia.
Se va a dividir en tres partes:
1. Sensores: Se configurarán los sensores para enviar datos a la nube .
2. BackEnd: Se configurará un servidor, desarrollado con la herramienta NodeRed, para guardar los datos de los sensores.
3. Forntend: Se configurará una Dashboard, usando NodeRed, para mostrar los datos de los sensores

                                                    Sensores
                                                        |
                                                        |
                                                    BackEnd
                                                        |
                                                        |
                                                    FrontEnd


## Descarga de las herramientas necesarias

En primer lugar se va a descargar NodeJS. Node es un "entorno en tiempo de ejecución" que básicamente permite ejecutar JavaScript fuera del navegador. No es necesario conocer como funciona, ya que directamente no se va a utilizar, pero es la herramienta con la que NodeRed ha sido desarrollado y con la que se pueden desarrollar plataformas profesionales. Para su descarga es necesario seguir los siguientes pasos:

1. Ve a la web de NodeJS (https://nodejs.org/en/)
2. Descarga la versión en la que pone "Recomended for most user"


------

En segundo lugar se va a descargar NodeRed. Como su web dice NodeRed "es una herramienta de programación para conectar dispositivos de hardware, API y servicios en línea de formas nuevas e interesantes". Básicamente permite la crear aplicaciones IoT mediante una interfaz muy intuitiva, sin necesidad de tener que porgramarlas. A continuación se detallan los pasos necesarios para su descarga:

**PARA WINDOWS y MACOS:**
1. Abrir el terminal de vuestro dispositivo (Aplicación simbolo del sistema en windows)
2. Copia y pega el siguiente comando: `npm install -g --unsafe-perm node-red`

## Ejecutar NodeRed
Para iniciar NodeRed en vuestro ordenador es necesario que ejecuteis el siguiente comando en el terminal de vuestro ordenador:
`node-red`

Para acceder a la aplicación tendreis que ir a vuestro navegador favorito y entrar en esta dirección http://127.0.0.1:1880/

**Es importante que no cerreis el terminal, ya que en ese caso se desconectaría la aplicación**

# Primeros pasos con NodeRED

Con NodeRED podemos crear aplicaciones de forma sencilla uniendo distintas "cajas", llamadas nodos, con funcionalidades, usando una interfaz web. Esto nos permite desarrollar servidores de forma rápida y sencilla, sin  necesidad de tener que programarlas con código.

Los nodos se conectan entre si para crear un flujo de información. Por ejemplo podemos conectar los sensores con una base de datos. O bien acceder a la base de datos para representarlos de forma visual.

En esta práctica vamos a crear una pequeña plataforma que va a conectarse con el backend de LoraWAN para obtener los valores que los sensores están enviando y guardarlos en una base de datos en vuestro ordenador. Finalmente se va a crear una pequeña Dasboard para mostrar estos valores.

1. Creamos un nodo "inject"
2. Creamos un nodo llamado "http request"
3. Doble click sobre el y lo configuramos:
Method: GET
URL: https://rtsensors-backend.azurewebsites.net/api/device-app/57574/07488f5e8f54af03
Return: "a parsed JSON object"
4. Creamos un nodo llamado debug
5. Unimos los tres nodos y le damos al botón de "Deploy" (Parte superior derecha)
6. En la barra derecha seleccionamos el boton de debug.

Lo segundo que vamos a hacer es enviar datos desde el servidor de LoraWAN  una base de datos
1. Creamos un nodo llamado "http in"


rts00001


