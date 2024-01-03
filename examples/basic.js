const IMU = require('../dist/index.cjs');
const imu = IMU('/dev/tty.usbmodem14201');

imu.on('data', console.log);
imu.init();
