import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.InetAddress;
import java.net.NetworkInterface;
import java.net.Socket;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.Enumeration;
import java.util.List;
import java.util.Properties;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;


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
    private final ExecutorService executor = Executors.newFixedThreadPool(1);

    Thread t = null;
    Thread usageUpdater = null;

    DataOutputStream dout;
    DataInputStream din;

    Properties prop = new Properties();
    Properties UsageProp = new Properties();
    OutputStream output = null;
    InputStream input = null;

    OutputStream UsageOutput = null;
    InputStream UsageInput = null;

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

        try
        {
            File f = new File("stat.properties");
            if (!f.exists())
            {
                output = new FileOutputStream("stat.properties");
                for (int iCount = 0; iCount < 100; iCount++)
                {
                    prop.setProperty("plug".concat(Integer.toString(iCount)), "false");
                }
                prop.store(output, null);
                output.close();
            }
            else
            {
                input = new FileInputStream("stat.properties");
                prop.load(input);
                for (int iCount = 0; iCount < 100; iCount++)
                {
                    Boolean stat = Boolean.parseBoolean(prop.getProperty("plug".concat(Integer.toString(iCount))));
                    plugs.get(iCount).setIsOn(stat);
                }
            }
        }
        catch (Exception e)
        {

        }

        executor.submit(new UsageUpdater());
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
                    else if (query.equals("USAGEREPORT"))
                    {
                        StringBuilder usageStr = new StringBuilder("");
                        for (int iCount = 0; iCount < 100; iCount++)
                        {
                            int usage = Integer.parseInt(UsageProp.getProperty("plug".concat(Integer.toString(iCount))));
                            usageStr.append(Integer.toString(usage));
                            usageStr.append(" ");
                        }

                        dout.writeUTF(usageStr.toString());
                    }
                    else if (query.equals("RESETUSAGE"))
                    {
                        try
                        {
                            executor.shutdown();
                            File f = new File("usage.properties");
                            f.delete();
                            executor.submit(new UsageUpdater());
                        }
                        catch (Exception e)
                        {

                        }

                        dout.writeUTF("USAGERESETED");
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

        public void setIsOn(boolean value)
        {
            isOn = value;
        }

        public void setOnOff(boolean value)
        {
            isOn = value;
            if (isOn)
            {
                new RunCommand("echo ").start();
                System.out.println(plugNumber + " Turned on");
            }
            else
            {
                System.out.println(plugNumber + " Turned off");
            }

            new UpdateProperty(isOn,plugNumber).start();
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

    private class UpdateProperty extends Thread
    {
        private boolean stat;
        private int plugNumber;

        UpdateProperty(Boolean value,int i)
        {
            stat = value;
            plugNumber = i;
        }

        public void run()
        {
            try
            {
                Boolean currentStat = Boolean.parseBoolean(prop.getProperty("plug".concat(Integer.toString(plugNumber))));
                if (stat != currentStat)
                {
                    FileOutputStream out = new FileOutputStream("stat.properties");
                    prop.setProperty("plug".concat(Integer.toString(plugNumber)), Boolean.toString(stat));
                    prop.store(out, null);
                    out.close();
                }
            }
            catch (Exception e)
            {

            }
        }
    }

    private class UsageUpdater implements Runnable
    {

        UsageUpdater()
        {
            try
            {
                File f = new File("usage.properties");
                if (!f.exists())
                {
                    UsageOutput = new FileOutputStream("usage.properties");
                    for (int iCount = 0; iCount < 100; iCount++)
                    {
                        UsageProp.setProperty("plug".concat(Integer.toString(iCount)), "0");
                    }
                    UsageProp.store(UsageOutput, null);
                    UsageOutput.close();
                }
                else
                {
                    UsageInput = new FileInputStream("usage.properties");
                    UsageProp.load(UsageInput);
                }
            }
            catch (Exception e)
            {

            }
        }
        public void run()
        {
            while (true)
            {
                try
                {
                    FileOutputStream out = new FileOutputStream("usage.properties");

                    for (int iCount = 0; iCount < 100; iCount++)
                    {
                        if(plugs.get(iCount).isOn())
                        {
                            int usage = Integer.parseInt(UsageProp.getProperty("plug".concat(Integer.toString(iCount))));
                            UsageProp.setProperty("plug".concat(Integer.toString(iCount)), Integer.toString(usage + 1));
                        }
                    }
                    UsageProp.store(out, null);
                    out.close();
                    Thread.sleep(1000);
                }
                catch (Exception e)
                {

                }
            }

        }

    }
}
