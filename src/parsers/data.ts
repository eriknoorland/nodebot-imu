import { math } from '@eriknoorland/nodebot-utils';
import { Data } from '../types';

export default (data: Data) => {
  const heading = parseInt([
    math.parseDecToBinary(data[0]),
    math.parseDecToBinary(data[1]),
  ].join(''), 2) / 100;

  return heading;
};
