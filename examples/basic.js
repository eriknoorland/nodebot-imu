const IMU = require('../src/IMU');
const imu = IMU('/dev/tty.usbmodem14201');

imu.on('data', console.log);
imu.init();
