1. Go to the `containerization` folder and run the following:
    ```
    sudo docker-compose up -d
    ```
    `sudo` if necessary!

2. Run the `curl` command below:
    To create an admin user, replacing:
    * `localhost` with the your IP
    * `admin` with your own username
    * `password` with your own password (note that the password requires single quotes)
    ```
    curl -XPOST "http://localhost:8086/query" --data-urlencode "q=CREATE USER admin WITH PASSWORD 'password' WITH ALL PRIVILEGES"
    ```
    To create test database:
    ```
    curl -i -XPOST http://localhost:8086/query -u admin:password --data-urlencode "q=CREATE DATABASE test"
    ```
    To store some sample of data:
    ```
    curl -i -XPOST 'http://localhost:8086/write?db=test' -u admin:password --data-binary \
    'test_msrt,device=XX-XX-XX-XX-XX-XX value=100.5
    test_msrt,device=33-33-33-33-33-33 value=530.2
    test_msrt,device=1F-1F-1F-1F-1F-1F value=13.1'
    ```

5. Run the following command in `containerization` folder to stop and remove containers and networks:
    ```
    sudo docker-compose down
    ```
    `sudo` if necessary!

6. Go to `influx-api` folder and in the following files: 
    * `server.js` change `admin:password` to the ones that you set for db and `localhost` to the your IP address in `new Influx('http://admin:password@localhost:8086/test');`
    * `swagger.js` change `localhost` in `"host": "localhost:8085"` to the your IP address

7. Run the following command to create docker image of the API:
    ```
    docker build -t tyto/influx-api .
    ```

8. Go to the `containerization` folder and in file `docker-compose.yml` uncomment the follwong:
    ```
    api:
        image: tyto/influx-api:latest
        container_name: influx-api
        build: .
        ports:
            - 8085:8085
        volumes:
            - ./api-volume/ssl:/etc/ssl
        depends_on:
            - influxdb
    ```
    ```
    volumes:
        influxdb-volume:
        ...
        api-volume:
    ```

9. Run the following:
    ```
    sudo docker-compose up -d
    ```
    `sudo` if necessary!

10. Open in the browser https://localhost:8085/api-docs (change localhost to your IP address) for API documentation, it will show the warning because the self-signed certificate is used to provide `https` service. Just click to the `Advanced...` option and  accept the risk and continue.

11. Check if the request returns data.

From now on, every time if you would connect to another WiFi network, you should repeat steps `5`, `6` to change IP address, `7` to generate new docker image and `9`