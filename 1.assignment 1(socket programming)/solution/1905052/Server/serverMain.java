package Server;

import java.io.*;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.*;

public class serverMain
{
    static Set<String> total = new HashSet<>();//all clients who connected to the server at least once
    static Set<String> offline = new HashSet<>();//all clients who goes offline after connected to the server
    static Set<String> online = new HashSet<>();//all clients who are connected to the server at present

    static int Max_Buffer_Size=100000;
    static int Max_Chunk_Size=100;

    //static int Max_Buffer_Size=100;
    //static int Max_Chunk_Size=20;

    static int Min_Chunk_Size=10;
    static int Current_Buffer_Size=0;

    static int nFile=0;//keep the total number of file and also used as a unique id of a file

    static HashMap<String,String> lFile;//link  file_id with file_name

    static int req=0;//used as a unique id of request from client for a file

    static Map<String,String> map = new HashMap<>();//the key is used for client's username and value is used for client's unread message

    static Map<String,String> req_user = new HashMap<>();


    public static void main(String[] args) throws IOException, ClassNotFoundException
    {

        ServerSocket serversocket = new ServerSocket(3000);

        while(true)
        {
            Socket socket = serversocket.accept();

            ObjectOutputStream out = new ObjectOutputStream(socket.getOutputStream());
            ObjectInputStream in = new ObjectInputStream(socket.getInputStream());
            String uName= (String) in.readObject();

            System.out.println(uName + " is trying to login");

            if(!online.contains(uName))
            {
                System.out.println(uName+" logged in successfully");

                out.writeObject("Login successfull");

                File f1=new File("src/Server/" + uName);
                if(!f1.exists())
                {
                    f1.mkdir();
                }
                File f2=new File("src/Server/" + uName + "/public");
                if(!f2.exists())
                {
                    f2.mkdir();
                }
                File f3=new File("src/Server/" + uName + "/private");
                if(!f3.exists())
                {
                    f3.mkdir();
                }
               if(!total.contains(uName))
               {
                   total.add(uName);
               }

                online.add(uName);
                offline.remove(uName);
            }
            else
            {
                out.writeObject("Login Failed");
                System.out.println("Login Failed!");
                //socket.close();
            }
            // open thread
            Thread worker = new Worker(socket,out,in,uName);
            worker.start();


        }

    }
}
