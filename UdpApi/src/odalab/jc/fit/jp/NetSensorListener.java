package odalab.jc.fit.jp;

import java.io.IOException;
import java.net.InetAddress;

public interface NetSensorListener {
	default Object addNetSensorListener(NetSensorListener o,String host,String com) throws IOException {
		if(com.equals("burst accel")) {
			SensorBurstGet sbg = new SensorBurstGet(InetAddress.getByName(host),o);
			return sbg;
		} else if(com.equals("move accel")) {
			SensorMoveGet smg = new SensorMoveGet(InetAddress.getByName(host),o);
			return smg;
		} else if(com.equals("bluemove accel")) {
			SensorMoveGet smg = new SensorMoveGet(InetAddress.getByName(host),o,com);
			return smg;
		}
		return null;
	}
	void sensorMeasured(SensorObject s);
}
