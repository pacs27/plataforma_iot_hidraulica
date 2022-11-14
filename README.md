# PRÁCTICA BINOMIO AGUA Y ENERGIA

En esta práctica se va a desarrollar una plataforma IoT básica, para aprender los principales conceptos que hay detrás de esta tecnología.
La práctica se va a dividir en tres partes:
1. Sensores: Se configurarán los sensores para enviar datos a la nube.
2. BackEnd: Se configurará un servidor, desarrollado con la herramienta Node-Red, para almacenar los datos de los sensores.
3. Forntend: Se configurará una Dashboard, usando Node-Red, para mostrar los datos de los sensores.


                                                    Sensores
                                                        |
                                                        |
                                                    BackEnd
                                                        |
                                                        |
                                                    FrontEnd


## Descarga de las herramientas necesarias

En primer lugar, se va a descargar NodeJS. Node es un entorno en tiempo de ejecución que básicamente permite ejecutar JavaScript fuera del navegador. No es necesario conocer cómo funciona, ya que no lo utilizaremos directamente, pero es la herramienta con la que NodeRed ha sido desarrollada y con la que se pueden desarrollar plataformas profesionales. Para su descarga es necesario seguir los siguientes pasos:

1. Ve a la web de NodeJS (https://nodejs.org/en/)
2. Descarga la versión en la que pone "Recomended for most user"


------

En segundo lugar, se va a descargar NodeRed. Como su web indica NodeRed "es una herramienta de programación para conectar dispositivos de hardware, API y servicios en línea de formas nuevas e interesantes". Básicamente permite la creación de aplicaciones IoT mediante una interfaz muy intuitiva, sin necesidad de tener que programarlas. A continuación, se detallan los pasos necesarios para su descarga:

**PARA WINDOWS y MACOS:**
1. Abre el terminal de tu dispositivo (Aplicación símbolo del sistema en Windows)
2. Copia y pega el siguiente comando: `npm install -g --unsafe-perm node-red`

## Ejecutar NodeRed
Para iniciar NodeRed en vuestro ordenador es necesario que ejecutes el siguiente comando en el terminal de tu ordenador:
`node-red`

Para acceder a la aplicación tendrás que ir a vuestro navegador favorito y entrar en esta dirección http://127.0.0.1:1880/

**Es importante que no cierres el terminal, ya que en ese caso se desconectaría la aplicación**

# Primeros pasos con NodeRED

Con NodeRED podemos crear aplicaciones usando una interfaz web. Esto permite el desarrollo de aplicaciones de forma rápida y sencilla, sin necesidad de tener que programarlas, mediante la unión de distintas "cajas", llamadas nodos, que tienen funcionalidades específicas.

Los nodos se conectan entre si para crear un flujo de información. Por ejemplo,  podemos conectar los sensores con una base de datos. O bien acceder a la base de datos para representarlos de forma visual.

En esta práctica vamos a crear una pequeña plataforma que va a conectarse con el backend de LoraWAN, usando el protocolo MQTT, para obtener los valores que los sensores están enviando. Estos valores se almacenarán en una base de datos en tu ordenador. Finalmente se va a crear una pequeña Dashboard para mostrar estos valores.

## CONEXION AL BACKEND DE LORAWAN

En primer lugar, vamos a connectarnos al TTN (Backend de LoraWAN) y vamos a crear un flujo de datos
1. Creamos un nodo "mqtt in"
2. Configuramos el broker de mqtt

    En la pantalla servidor pulsamos en editar. Tras esto debemos actualizar los campos para añidir el servidor (eu1.cloud.thethings.network) y el puerto (1883). 

    ![mqtt_broker](https://github.com/pacs27/plataforma_iot_hidraulica/blob/master/imagenes/mqtt_server_config.PNG)
    
    En la pestaña seguridad hay que añadir las credenciales de acceso al sensor. Estas credenciales son: Usuario (rts00001@ttn) y contraseña (<Os la diré en clase>)

    ![mqtt_broker_security](https://github.com/pacs27/plataforma_iot_hidraulica/blob/master/imagenes/mqtt_serveR_config_security.PNG)

    Finalmente, en la pestaña general (la que aparece al abrir el nodo) añadiremos # al campo Topic, para subscribirse a todos.

    ![mqtt_general](https://github.com/pacs27/plataforma_iot_hidraulica/blob/master/imagenes/mqtt_general.PNG)

3. Crear un nodo debug y conectarlo al MQTT broker para ver los datos que el sensor envía

    ![mqtt_debug](https://github.com/pacs27/plataforma_iot_hidraulica/blob/master/imagenes/mqtt_debug.PNG)

4. Pulsamos el botón "Deploy" situado en la parte superior derecha


Ya tendríamos nuestra primera aplicación funcionando. En la pestaña debug deben aparecer los mensajes que el sensor está enviando (cada 5 minutos).

## Creacion de una base de datos para guardar los datos de los sensores.

En esta parte de la práctica se va a utilizar una base de datos (DB) SQLite.  Esta base de datos es una biblioteca en lenguaje de programación C que implementa un motor de base de datos SQL pequeño, rápido, autónomo, de alta fiabilidad y con todas las funciones. Es muy utilizada para pequeñas aplicaciones debido a su facilidad de configuración.

## Descarga y creacion de una tabla SQL
1. Descarga de la librería SQLite. Para ello, pulsamos en el botón superior derecho --> Ajustes --> Paleta -->Instalar --> node-red-node-sqlite
2. Insertamos un nodo inject, sqlite y debug y los conectamos 

    ![creaction_db](https://github.com/pacs27/plataforma_iot_hidraulica/blob/master/imagenes/flujo_creacion_db.PNG)

3. Editamos el nodo SQLite y añadimos una nueva base de datos. En "Database" añadimos una nueva llamada "/tmp/sqlite"
 ![creaction_db](https://github.com/pacs27/plataforma_iot_hidraulica/blob/master/imagenes/create_new_database.PNG)

4. Volvemos a editar el nodo SQLite. En SQL Query (Fixed statement) y en SQL statement añadimos lo siguiente:

```sql
CREATE TABLE sensoreshumedad(id INTEGER PRIMARY KEY AUTOINCREMENT, bateria NUMERIC, humedad30cm NUMERIC, humedad45cm NUMERIC, timestamp DATE)
```

    ![sqlite_create_table](https://github.com/pacs27/plataforma_iot_hidraulica/blob/master/imagenes/sqlite_create_table.PNG)

5. Pulsamos botón "Deploy" y pulsamos el botón de "timestamp" (nodo inject creado anteriormente)

Ya tenemos nuestra base de datos y tabla creada. En el siguiente paso almacenaremos los datos de los sensores
## Almacenar datos de los sensores en la base de datos

Para almacenar los datos de los sensores debemos en primer lugar crear un nodo "funcion" que filtre la información que necesitamos.

1. Creamos un nodo "función" y lo conectamos con el nodo "Broker mqtt" creado anteriormente.

2. Editamos el nodo "función"y en la parte "On Message" añadimos el siguiente script:
 ```js

const mensajeSensor = msg.payload.uplink_message.decoded_payload
const timestamp = msg.payload.uplink_message.received_at
const bateria = mensajeSensor["field1"]/1000
const humedad30cm = mensajeSensor["field2"]/100
const humedad45cm = mensajeSensor["field4"]/100

const payload = {
    timestamp: timestamp,
    bateria: bateria,
    humedad30cm: humedad30cm,
    humedad45cm: humedad45cm
}

const params = {
    $timestamp: timestamp,
    $bateria: bateria,
    $humedad30cm: humedad30cm,
    $humedad45cm: humedad45cm
}

return {params, payload}
 ```

Este script devolverá los valores que los sensores están enviando a través de MQTT

3. Creamos un nodo "SQLite" y lo editamos. En SQL Query (**prepared statement**) y en SQL Statement insertamos lo siguiente:
```sql
INSERT INTO sensoreshumedad(bateria, humedad30cm, humedad45cm, timestamp) values($bateria,$humedad30cm,$humedad45cm, $timestamp)
```

4. Creamos y conectamos un nodo Debug para ver los resultados. 

    ![mqtt_insert_values](https://github.com/pacs27/plataforma_iot_hidraulica/blob/master/imagenes/mqtt_insert_values.PNG)

5. Pulsamos el botón Deploy

En este punto tendríamos un flujo de datos que va almacenando los valores de los sensores en una base de datos.
## Mostrar los valores en una gráfica

El último punto de esta práctica va a ser el despliege de los datos en una gráfica.

1. Seguimos los mismo pasos utilizados para descargar la base de datos y añadimos el plugin node-red-dashboard
2. Insertamos los nodos Inject, SQLite, Debug, Function y Chart

    ![chart_flow](https://github.com/pacs27/plataforma_iot_hidraulica/blob/master/imagenes/chart_flow.PNG)

3. Editamos el nodo SQLite. En SQL Query (Fixed statement) y en SQL statement insertamos lo siguiente:

```sql
select * from sensoreshumedad;
```
4. Editamos el nodo "Función" y añadimos este script:

```js

const mensajeSensor = msg.payload
const numeroDeDatos = mensajeSensor.length
let cm_30_data = [];
let cm_45_data = [];
for(var i=0;i<numeroDeDatos; i++){
    cm_30_data.push({ "x": mensajeSensor[i]["timestamp"], "y": mensajeSensor[i]["humedad30cm"]})
    cm_45_data.push({ "x": mensajeSensor[i]["timestamp"], "y": mensajeSensor[i]["humedad45cm"] })
}
    

return {
    payload: [{
        "series": ["huemdad30cm", "humedad45cm"],
        "data": [cm_30_data, cm_45_data ],
        "labels": [""]
    }] }

```
5. Editamos el nodo "chart". Añadimos un nuevo grupo de gráficas y dentro del grupo de gráficas un nuevo "tab".

![edit_chart](https://github.com/pacs27/plataforma_iot_hidraulica/blob/master/imagenes/edit_chart.png)

6. Pulsamos el botón desplegar
7. Pulsamos el botón timestamp (Inject)
8. Accedemos, a traves de nuestro navegador, a la URL http://localhost:1880/ui

Si todo ha ido bien, tendremos nuestra aplicación IoT funcionando.