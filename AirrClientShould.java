import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.PrintStream;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;

/**
 * Created by kalrav on 4/9/16.
 */
public class AirrClientShould
{
    private static AirrClientShould onlyInstance;
    private static Socket client;

    private static final String IP = "192.168.1.2";
    private static final int PORT = 5000;
    private static final String TAG = "ClientManager : ";
    private static final String identity = "SWITCHBOARD";
    private static final String OK = "ok";
    private static final String id = "id";
    private static DataOutputStream dout;
    private static DataInputStream din;

    // constructor
    AirrClientShould()
    {

        try
        {
            client = new Socket("127.0.0.1",PORT);
            din = new DataInputStream(client.getInputStream());
            dout = new DataOutputStream(client.getOutputStream());
        }
        catch(IOException exception)
        {
            System.out.println(TAG+"Unable to open data streams");
        }

        new StartCommunication().start();
    }

    private static AirrClientShould getInstance()
    {
        if (onlyInstance == null)
        {
            return new AirrClientShould();
        }

        return onlyInstance;
    }

    public static void main(String args[])
    {
        AirrClientShould.getInstance();
    }

    private class StartCommunication extends Thread
    {
        String response;
        public void run()
        {
            try
            {
                dout.writeUTF("12345678901");
                response = din.readUTF();
                System.out.println("got respnse "+response);

            }
            catch (Exception ex)
            {
                System.out.println(TAG + "Unable to communicate identity data");
                stopCommunication();
                return;
            }
        }
    }

    private void stopCommunication()
    {
        try
        {
            dout.close();
            din.close();
            client.close();
        }
        catch (IOException e)
        {
            System.out.println(TAG+ "Unable to read data");
        }
    }
}