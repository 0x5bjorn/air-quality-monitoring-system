Run the `curl` command below to create an admin user, replacing:

* `localhost` with the IP or hostname of your InfluxDB instance
* `admin` with your own username
* `password` with your own password (note that the password requires single quotes)

```
curl -XPOST "http://localhost:8086/query" --data-urlencode "q=CREATE USER admin WITH PASSWORD 'password' WITH ALL PRIVILEGES"
```