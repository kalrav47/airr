import java.net.*;
import java.util.*;
import java.io.*;

public class AirrServer
{
    static Vector ClientSockets = new Vector();;
    static Vector SwitchIds = new Vector();;
    static List<AcceptClient> Switches = new ArrayList<AcceptClient>();
    private static String message = "";
    private static final String CLIENT_TYPE_SWITCH = "SWITCHBOARD";
    private static final String GET_ID = "TELLMEYOURID";
    private static final Object lock = new Object();

    AirrServer() throws Exception
    {
        ServerSocket soc = new ServerSocket(5000);

        while (true)
        {
            Socket CSoc = soc.accept();
            AcceptClient obClient = new AcceptClient(CSoc);
        }
    }

    public static void main(String args[]) throws Exception
    {

        AirrServer airrServer = new AirrServer();
    }

    class AcceptClient extends Thread
    {
        boolean isThisSwitch = false;
        Socket ClientSocket;
        DataInputStream din;
        DataOutputStream dout;
        String msgFromSwitchBoard = "";
        String myId = "";

        AcceptClient(Socket CSoc) throws Exception
        {

            ClientSocket = CSoc;

            din = new DataInputStream(ClientSocket.getInputStream());
            dout = new DataOutputStream(ClientSocket.getOutputStream());

            start();
        }

        private String getMsg()
        {
            return msgFromSwitchBoard;
        }

        public void run()
        {
            int iCount = 0;
            do
            {

                try
                {
                    msgFromSwitchBoard = din.readUTF();

                    if (msgFromSwitchBoard.equals(CLIENT_TYPE_SWITCH))
                    {
                        isThisSwitch = true;
                        dout.writeUTF(GET_ID);
                        myId = din.readUTF();
                        SwitchIds.add(myId);
                        ClientSockets.add(ClientSocket);
                        Switches.add(this);
                    }
                    else if (msgFromSwitchBoard.equals("AREYOUTHERE"))
                    {
                        dout.writeUTF("YESIMHERE");
                    }
                    else if (msgFromSwitchBoard.equals("SBDETAILSPLS"))
                    {

                        StringBuilder str = new StringBuilder("");

                        for (iCount = 0; iCount < SwitchIds.size(); iCount++)
                        {
                            str.append(SwitchIds.elementAt(iCount).toString().substring(0, 10));
                            str.append(" ");
                        }

                        dout.writeUTF(str.toString());
                    }
                    else if(!isThisSwitch)
                    {
                        for (iCount = 0; iCount < SwitchIds.size(); iCount++)
                        {
                            synchronized (lock)
                            {
                                String id = SwitchIds.elementAt(iCount).toString();
                                if (msgFromSwitchBoard.contains(id.substring(0, 10)))
                                {
                                    AcceptClient ac = Switches.get(iCount);
                                    Socket tSoc = (Socket) ClientSockets.elementAt(iCount);

                                    DataInputStream tdin = new DataInputStream(tSoc.getInputStream());
                                    DataOutputStream tdout = new DataOutputStream(tSoc.getOutputStream());

                                    tdout.writeUTF(msgFromSwitchBoard);

                                    Thread.sleep(10);

                                    dout.writeUTF(ac.getMsg());
                                    continue;
                                }
                            }
                        }

                        if(iCount == SwitchIds.size())
                        {
                            dout.writeUTF("UNKNOWN");
                        }
                    }
                } catch (Exception ex)
                {
                    for (iCount = 0; iCount < SwitchIds.size(); iCount++)
                    {
                        String id = SwitchIds.elementAt(iCount).toString().substring(0, 10);
                        System.out.println(id+" "+myId);
                        if(myId.contains(id))
                        {
                            System.out.println("inside");
                            SwitchIds.remove(iCount);
                            ClientSockets.remove(iCount);
                            Switches.remove(iCount);
                            break;
                        }
                    }
                    break;
                }

            } while (isThisSwitch);
        }
    }
}
