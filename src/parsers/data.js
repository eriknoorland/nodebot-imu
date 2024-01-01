const robotlib = require('robotlib');

const { parseDecToBinary } = robotlib.utils.math;

module.exports = data => {
  const heading = parseInt([
    parseDecToBinary(data[0]),
    parseDecToBinary(data[1]),
  ].join(''), 2) / 100;

  return heading;
};
