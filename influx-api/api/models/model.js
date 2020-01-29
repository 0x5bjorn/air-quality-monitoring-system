'use strict';

var Influx = require('influx');
Influx.connect('influx://192.168.0.111:8086');
var influx = new Influx.InfluxDB({
  database: 'test',
  host: 'https://192.168.0.111',
  port: 8086,
  username: 'admin',
  password: 'password',
  schema: [
    {
      measurement: 'test_msrt',
      fields: {
        time: Influx.FieldType.TIME
        device: Influx.FieldType.STRING,
        value: Influx.FieldType.FLOAT,
      }
      // tags: [
      // ]
    }
  ]
});

module.exports = influx;