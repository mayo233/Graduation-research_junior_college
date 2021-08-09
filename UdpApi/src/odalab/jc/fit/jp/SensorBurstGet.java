package odalab.jc.fit.jp;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;

public class SensorBurstGet implements Runnable {
	public static int defaultPort = 8181;
	InetAddress inet;
	int portNumber = defaultPort;
	DatagramSocket socket;
	double timing = 0.1;
	String defaultCommand = "burst accel";

	Thread thread;
	NetSensorListener par;
	int pid = 0;
	boolean isActive = true;

	public SensorBurstGet(InetAddress i,NetSensorListener s) throws IOException {
		inet = i;
		par = s;
		setup();
	}

	public void close() throws IOException {
		if(pid > 1) {
			DatagramPacket packet;
			String msg = "stop " + pid;
			byte[] buf = msg.getBytes();//バイト列に変換
			packet = new DatagramPacket(buf, buf.length, inet, portNumber);
			socket.send(packet);
		}
		isActive = false;
	}
	void setup() throws IOException {
		thread = new Thread(this);
		socket = new DatagramSocket();//UDP送信用ソケット
		DatagramPacket packet;
		String msg = defaultCommand + String.format(" %f",timing);
		byte[] buf = msg.getBytes();//バイト列に変換
		packet = new DatagramPacket(buf, buf.length, inet, portNumber);
		socket.send(packet);

		thread.start();
	}

	@Override
	public void run() {
		while(isActive) {
			try {
				byte[] buf = new byte[1024];
				DatagramPacket packet = new DatagramPacket(buf,buf.length);
				socket.receive(packet);	// とりあえず受信
				String s =  new String(packet.getData());
				if(s.substring(0,13).equals("burst process")) {
					pid = getNum(s.substring(15));
					//System.out.println("Sender process is " +pid);
				}
				else par.sensorMeasured(new SensorObject(s));
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
	}

	int getNum(String s) {
		int n = 0;
		int i = 0;
		while(Character.isDigit(s.charAt(i)) ) {
			n = n * 10 + s.charAt(i)-'0';
			i++;
		}
		return n;
	}

}
