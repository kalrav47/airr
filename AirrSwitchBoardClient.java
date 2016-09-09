import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.net.Socket;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;

class AirrSwitchBoardClient implements Runnable
{
    Socket soc;
    private static final Path idPath = Paths.get(".","id");
    private static final String CLIENT_TYPE_SWITCH = "SWITCHBOARD";
    private static final String GET_ID = "TELLMEYOURID";
    private static String myId;

    Thread t = null;

    DataOutputStream dout;
    DataInputStream din;

    AirrSwitchBoardClient() throws Exception
    {
        soc = new Socket("127.0.0.1", 5000);

        din = new DataInputStream(soc.getInputStream());
        dout = new DataOutputStream(soc.getOutputStream());

        t = new Thread(this);
        t.start();

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
        } catch (Exception ex)
        {
            return;
        }

        while (true)
        {
            try
            {
                din.readUTF();
                dout.writeUTF("kalrav is great");
            } catch (Exception ex)
            {
                break;
            }
        }
    }
}
