package odalab.jc.fit.jp;

public class SensorObject {
	public SensorObject(String s) {
		in = s;
	}
	String in;
	public String getInputLine() {
		return new String(in);
	}
	public double getX() { // Burstモードで転送されたデータのうちX座標値を得る。
		String[] s = in.split(":");
		String[] s2 = s[2].split(",");
		return Double.parseDouble(s2[0]);
	}
	public double getY() { // Burstモードで転送されたデータのうちX座標値を得る。
		String[] s = in.split(":");
		String[] s2 = s[2].split(",");
		return Double.parseDouble(s2[1]);
	}
	public double getZ() { // Burstモードで転送されたデータのうちX座標値を得る。
		String[] s = in.split(":");
		String[] s2 = s[2].split(",");
		return Double.parseDouble(s2[2]);
	}
	public int getSequenceNumber() { // Burst モード等、センサーからの読み込み順序番号を得る。
		String[] s = in.split(":");
		return Integer.parseInt(s[1]);
	}

	public String getMove() {
		String[] s = in.split(":");
		return (s[1].substring(0, 2));
	}
}
