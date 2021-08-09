import java.net.InterfaceAddress;
import java.net.NetworkInterface;
import java.util.Enumeration;

public class Test2 {


   public static void main(String[] args) throws Exception {
    Enumeration<NetworkInterface> nics = NetworkInterface.getNetworkInterfaces();

    while (nics.hasMoreElements()) {
        NetworkInterface nic = nics.nextElement();
        for (InterfaceAddress iaddr : nic.getInterfaceAddresses()) {
//            boolean valid = checkPrefix(iaddr);
//            if (!valid) {
//                passed = false;
//                debug(nic.getName(), iaddr);
//            }
        	try {
            byte[] ia = iaddr.getBroadcast().getAddress();
            if(ia.length == 4 && ia[0] != 127) {
            	System.out.println(String.format("%d.%d.%d.%d",(ia[0]<0?256+ia[0]:ia[0]),
            			(ia[1]<0?256+ia[1]:ia[1]),(ia[2]<0?256+ia[2]:ia[2]),(ia[3]<0?256+ia[3]:ia[3])));
            }
        	} catch(NullPointerException e) {

        	}
        }
    }


	}
}
