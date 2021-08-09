import java.io.IOException;

import odalab.jc.fit.jp.NetSensorListener;
import odalab.jc.fit.jp.SensorMoveGet;
import odalab.jc.fit.jp.SensorObject;

public class MoveTest implements NetSensorListener {
	SensorMoveGet sen;
	public MoveTest(String host) {
		try {
			sen = (SensorMoveGet)addNetSensorListener(this,host,"move accel");
		} catch (IOException e) {
			// TODO 自動生成された catch ブロック
			e.printStackTrace();
		}
	}
	public static void main(String[] args) {
		MoveTest test = new MoveTest("192.168.11.255");
		try {
			Thread.sleep(100*1000); // 100ミリ秒間だけ待つ
		} catch (InterruptedException e) {
			// TODO 自動生成された catch ブロック
			e.printStackTrace();
		}
		System.exit(0);
	}

	public void sensorMeasured(SensorObject s) {
		System.out.println("Get: "+s.getInputLine());
	}

	public void finish() throws IOException {
		sen.close();
	}

}
