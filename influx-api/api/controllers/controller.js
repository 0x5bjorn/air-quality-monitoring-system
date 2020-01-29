'use strict';

var Influx = require('influx');
// var influx = influx.model('influx');

exports.list_all_data = function(req, res) {
  influx.query(`
    select * from test_msrt
  `).then(result => {
    res.json(result)
  }).catch(err => {
    res.status(500).send(err.stack)
  })
};

exports.list_n_data = function(req, res) {
  influx.query(`
    select * from test_msrt
  `).then(result => {
    res.json(result)
  }).catch(err => {
    res.status(500).send(err.stack)
  })
};
