import java.io.IOException;

import odalab.jc.fit.jp.NetSensorListener;
import odalab.jc.fit.jp.SensorBurstGet;
import odalab.jc.fit.jp.SensorObject;

public class UDPTest2 implements NetSensorListener{
	SensorBurstGet sen;
	public UDPTest2(String host) {
		try {
			sen = (SensorBurstGet)addNetSensorListener(this,host,"burst accel");
		} catch (IOException e) {
			// TODO 自動生成された catch ブロック
			e.printStackTrace();
		}
	}
	public static void main(String[] args) {
		UDPTest2 test = new UDPTest2("192.168.11.101");
		try {
			Thread.sleep(1*1000);
		} catch (InterruptedException e) {
			// TODO 自動生成された catch ブロック
			e.printStackTrace();
		}
		System.exit(0);
	}

	public void sensorMeasured(SensorObject s) {
		System.out.println("Get: X="+s.getX());
		//System.out.println("Get: "+s.getInputLine());

	}

	public void finish() throws IOException {
		sen.close();
	}

}
