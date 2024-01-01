const EventEmitter = require('events');
const SerialPort = require('serialport');
const cobs = require('cobs');
const Parser = require('./Parser');

const IMU = path => {
  const eventEmitter = new EventEmitter();
  const requestStartFlag = 0xA6;

  let parser;
  let port;

  /**
   * Init
   * @return {Promise}
   */
  function init() {
    return new Promise((resolve, reject) => {
      if (port) {
        setTimeout(reject, 0);
        return;
      }

      let isReady = false;

      const isReadyTimeout = setTimeout(() => {
        if (!isReady) {
          ready();
        }
      }, 1000);

      port = new SerialPort(path, { baudRate: 115200 });
      parser = new Parser();

      port.pipe(parser);

      port.on('error', error => eventEmitter.emit('error', error));
      port.on('disconnect', () => eventEmitter.emit('disconnect'));
      port.on('close', () => eventEmitter.emit('close'));
      port.on('open', onPortOpen);

      parser.on('data', data => {
        if (isReady) {
          eventEmitter.emit('data', data);
        }
      });

      parser.on('ready', () => {
        clearTimeout(isReadyTimeout);
        isReady = true;
        resolve();
      });
    });
  }

  /**
   * Reset
   * @return {Promise}
   */
  function reset() {
    return new Promise(resolve => {
      writeToSerialPort([requestStartFlag, 0x20]);

      setTimeout(() => {
        resolve();
      }, 500);
    });
  }

  function ready() {
    writeToSerialPort([requestStartFlag, 0x10]);

    return Promise.resolve();
  }

  function writeToSerialPort(data) {
    port.write(cobs.encode(Buffer.from(data), true));
  }

  function onPortOpen() {
    port.flush(error => {
      if (error) {
        eventEmitter.emit('error', error);
      }
    });
  }

  return {
    init,
    reset,
    on: eventEmitter.on.bind(eventEmitter),
    off: eventEmitter.off.bind(eventEmitter),
  };
};

module.exports = IMU;
