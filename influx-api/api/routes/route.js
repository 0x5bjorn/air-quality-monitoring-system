'use strict';

module.exports = function(app) {
  var controller = require('../controllers/controller');

  // todoList Routes
  app.route('/testdata')
    .get(controller.list_all_data);

  // app.route('/testdata/:')
  //   .get(controller.read_n_data);
};