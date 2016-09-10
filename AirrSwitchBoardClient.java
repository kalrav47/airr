import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.net.InetAddress;
import java.net.NetworkInterface;
import java.net.Socket;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.Enumeration;
import java.util.List;

class AirrSwitchBoardClient implements Runnable
{
    Socket soc;
    private static final Path idPath = Paths.get(".", "id");
    private static final String CLIENT_TYPE_SWITCH = "SWITCHBOARD";
    private static final String GET_ID = "TELLMEYOURID";

    private static final String ARM_SYSTEM = "ARM";
    private static final String DIS_ARM_SYSTEM = "DISARM";

    private static final String ARM_SYSTEM_REPLY = "SYSTEMISARMED";
    private static final String DIS_ARM_SYSTEM_REPLAY = "SYSTEMISDISARMED";

    private static String myId;
    private static boolean isSystemArmed = false;
    static List<PlugHandler> plugs = new ArrayList<PlugHandler>();

    Thread t = null;

    DataOutputStream dout;
    DataInputStream din;

    AirrSwitchBoardClient() throws Exception
    {
        Enumeration<NetworkInterface> eni = NetworkInterface.getNetworkInterfaces();
        String ipAddr = "";
        while (eni.hasMoreElements())
        {
            NetworkInterface ni = eni.nextElement();
            Enumeration<InetAddress> inetAddresses = ni.getInetAddresses();


            while (inetAddresses.hasMoreElements())
            {
                InetAddress ia = inetAddresses.nextElement();
                if (ni.getName().equals("wlan0"))
                {
                    ipAddr = ia.getHostAddress();
                }
            }
        }

        soc = new Socket(ipAddr, 5000);

        din = new DataInputStream(soc.getInputStream());
        dout = new DataOutputStream(soc.getOutputStream());

        t = new Thread(this);
        t.start();

        for (int iCount = 0; iCount < 100; iCount++)
        {
            PlugHandler plugHandler = new PlugHandler(iCount);
            plugs.add(plugHandler);
        }

    }

    public static void main(String args[]) throws Exception
    {
        AirrSwitchBoardClient Client1 = new AirrSwitchBoardClient();
    }

    public void run()
    {
        try
        {
            myId = new String(Files.readAllBytes(idPath));
            dout.writeUTF(CLIENT_TYPE_SWITCH);
            String reply = din.readUTF();

            if (reply.equals(GET_ID))
            {
                dout.writeUTF(myId);
            }
        }
        catch (Exception ex)
        {
            return;
        }

        while (true)
        {
            try
            {
                String query = din.readUTF();
                query = query.substring(10); // remove id from query


                if (!isSystemArmed)
                {
                    if (query.equals(ARM_SYSTEM))
                    {
                        isSystemArmed = true;
                        dout.writeUTF(ARM_SYSTEM_REPLY);
                    }
                    else if (query.equals("STATUS"))
                    {
                        String status = "";
                        for (int iCount = 0; iCount < 100; iCount++)
                        {
                            PlugHandler ph = plugs.get(iCount);
                            if (ph.isOn())
                            {
                                status += '1';
                            }
                            else
                            {
                                status += '0';
                            }
                        }
                        dout.writeUTF(status);
                    }
                    else
                    {
                        if (query.length() == 5)
                        {
                            // String command = query.substring(query.length() - 5);

                            String plugType = query.substring(0, 1);
                            String forWhom = query.substring(1, 3);
                            String forWhat = query.substring(3);

                            char firstChar = forWhom.charAt(0);
                            if (firstChar == '0')
                            {
                                forWhom = forWhom.substring(forWhom.length() - 1);
                            }

                            char firstCharr = forWhat.charAt(0);
                            if (firstCharr == '0')
                            {
                                forWhat = forWhat.substring(forWhat.length() - 1);
                            }

                            PlugHandler ph = plugs.get(Integer.parseInt(forWhom));

                            if (Integer.parseInt(plugType) == 0)
                            {
                                if (Integer.parseInt(forWhat) >= 50)
                                {
                                    if (!ph.isOn())
                                    {
                                        ph.setOnOff(true);
                                        dout.writeUTF("TURNEDON");
                                    }
                                    else
                                    {
                                        dout.writeUTF("ALREADYON");
                                    }
                                }
                                else
                                {
                                    if (ph.isOn())
                                    {
                                        ph.setOnOff(false);
                                        dout.writeUTF("TURNEDOFF");
                                    }
                                    else
                                    {
                                        dout.writeUTF("ALREADYOFF");
                                    }
                                }
                            }
                        }
                        else
                        {
                            dout.writeUTF("UNKNOWN");
                        }
                    }
                }
                else
                {
                    if (query.equals(DIS_ARM_SYSTEM))
                    {
                        isSystemArmed = false;
                        dout.writeUTF(DIS_ARM_SYSTEM_REPLAY);
                    }
                    else if (query.equals("STATUS"))
                    {
                        String status = "";
                        for (int iCount = 0; iCount < 100; iCount++)
                        {
                            PlugHandler ph = plugs.get(iCount);
                            if (ph.isOn())
                            {
                                status += '1';
                            }
                            else
                            {
                                status += '0';
                            }
                        }
                        dout.writeUTF(status);
                    }
                    else
                    {
                        dout.writeUTF(ARM_SYSTEM_REPLY);
                    }
                }


            }
            catch (Exception ex)
            {
                break;
            }
        }
    }

    private class PlugHandler
    {
        private int plugNumber;
        private boolean isOn = false;
        private boolean previosState = false;

        public void setOnOff(boolean value)
        {
            isOn = value;
            if (isOn)
            {
                new RunCommand("eject").start();
                System.out.println(plugNumber + " Turned on");
            }
            else
            {
                System.out.println(plugNumber + " Turned off");
            }
        }

        public boolean isOn()
        {
            return isOn;
        }

        PlugHandler(int pin)
        {
            plugNumber = pin;
        }
    }

    private class ReadTouchInput
    {
        private int plugNumber;

        ReadTouchInput(int pin)
        {
            plugNumber = pin;
        }
    }

    private class RunCommand extends Thread
    {
        private String command;

        public void run()
        {
            try
            {
                Runtime.getRuntime().exec(command);
            }
            catch (IOException e)
            {
            }
        }

        RunCommand(String arg)
        {
            command = arg;
        }
    }
}
