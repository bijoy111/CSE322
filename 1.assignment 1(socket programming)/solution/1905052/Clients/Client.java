package Clients;

import java.io.*;
import java.net.Socket;
import java.net.SocketTimeoutException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.Scanner;
import java.util.Set;

public class Client
{


    public static void main(String[] args) throws IOException, ClassNotFoundException, SocketTimeoutException, InterruptedException {



        String req_id=null;

        ObjectOutputStream out ;//used for sending message to the server
        ObjectInputStream in ;//used for collecting message from the server

        DataOutputStream outf ;//it is used for sending file chunk by chunk
        DataInputStream inf ;//it is used for collecting file chunk by chunk

        Socket socket;


        socket = new Socket("localhost", 3000);

        out = new ObjectOutputStream(socket.getOutputStream());
        in = new ObjectInputStream(socket.getInputStream());

        outf = new DataOutputStream(socket.getOutputStream());
        inf = new DataInputStream(socket.getInputStream());


        Scanner sc=new Scanner(System.in);

        System.out.println("Username:");
        String uName=sc.nextLine();
        out.writeObject(uName);

        String msg = (String) in.readObject();
        System.out.println(msg);

        if(msg.equalsIgnoreCase("Login Failed"))
        {
            System.out.println("You use a existing username");
            System.out.println("Try to login with a unique username");
            socket.close();
            return;
        }

        Thread fT = null;


        while(true)
        {
            System.out.println("Available Feature");
            System.out.println("1.For looking up clients list, type: Client");
            System.out.println("2.For looking up own files, type: Uploaded File");
            System.out.println("3.For looking up others’ public files, type: Public Files");
            System.out.println("4.For uploading files, type: Upload");
            System.out.println("5.For downloading files, type: Download");
            System.out.println("6.For requesting file, type: Request");
            System.out.println("7.For seeing unread messages, type: Message");
            System.out.println("8.For uploading files on request, type: Upload on request");
            System.out.println("9.For logging out, type: Log out");

            System.out.println("Task:");
            String capability=sc.nextLine();
            if(capability.equalsIgnoreCase("Client"))
            {
                out.writeObject("List of Clients");
                System.out.println("List of Clients");
                Set<String> list1= (Set<String>) in.readObject();
                Set<String> list2= (Set<String>) in.readObject();
                for(String l:list1)
                {
                    System.out.println(l+" (online)");
                }
                for(String l:list2)
                {
                    System.out.println(l+" (offline)");
                }
            }
            else if(capability.equalsIgnoreCase("Uploaded File"))
            {
                out.writeObject("My files");
                ArrayList<String> files= (ArrayList<String>) in.readObject();
                for(var f:files)
                {
                    System.out.println(f+" (private)");
                }


               files= (ArrayList<String>) in.readObject();
                for(var f:files)
                {
                    System.out.println(f+" (public)");
                }

            }
            else if(capability.equalsIgnoreCase("Public Files"))
            {
                out.writeObject("Others public files");
                int sz= (int) in.readObject();
                for(int i=1;i<sz;i++)
                {
                    String uN= (String) in.readObject();
                    System.out.println(uN+"'s public files:");
                    ArrayList<String> files= (ArrayList<String>) in.readObject();
                    for(var f:files)
                    {
                        System.out.println(f+" (public)");
                    }
                }
            }
            else if(capability.equalsIgnoreCase("Upload"))
            {
                System.out.println("File name:");
                String fName=sc.nextLine();
                System.out.println("Access : Public or Private");
                String acc=sc.nextLine();
                if(!(fT==null)&& fT.isAlive())
                {
                    System.out.println("Try Later!");
                }
                else
                {
                    out.writeObject("Upload Files");
                    try
                    {
                        File f=new File(fName);

                        f.createNewFile();
                        try {

                            System.out.println("Write the code or text you want to add in your file");
                            StringBuilder codeBuilder = new StringBuilder();

                            while (true) {
                                String line = sc.nextLine();
                                if (line.isEmpty()) {
                                    break;
                                }
                                codeBuilder.append(line).append(System.lineSeparator());
                            }

                            FileWriter writer = new FileWriter(f);
                            writer.write(codeBuilder.toString());
                            writer.close();
                        } catch (IOException e) {
                            e.printStackTrace();
                        }
                        out.writeObject(fName);
                        out.writeObject(acc);
                        int len= (int) f.length();
                        out.writeObject( String.valueOf(len));
                        String trans = (String) in.readObject();
                        if (trans.equalsIgnoreCase("Transmission is allowed"))
                        {
                            System.out.println(trans);
                            String cSize = (String) in.readObject();
                            //System.out.println(cSize);
                            int cSizei = Integer.parseInt(cSize);
                            String fId = (String) in.readObject();
                            //System.out.println(fId);
                            System.out.println("Confirmation message ! Your chunk size is: "+cSize+" and file id is: "+fId);
                            int nChunk = (int) Math.ceil((f.length() * 1.0) / cSizei);
                            //System.out.println(nChunk);
                            byte[] text = Files.readAllBytes(Paths.get(f.getPath()));
                            int s = 0;

                            int timeout=0;
                            for (int i = 1; i <= nChunk; i++)
                            {
                                if (i == nChunk)
                                {
                                    outf.write(text, s,  text.length - s);
                                }
                                else
                                {
                                    outf.write(text, s, s + cSizei);
                                    s += cSizei;
                                }
                                outf.flush();
                                socket.setSoTimeout(30000);
                                try
                                {
                                    String ack = (String) in.readObject();
                                    System.out.println(ack);
                                    out.writeObject("Got the acknowledgement message in time");

                                }
                                catch (SocketTimeoutException se)
                                {
                                    out.writeObject("TimeOut");
                                    timeout=1;
                                }
                                socket.setSoTimeout(0);
                                if(timeout==1)
                                {
                                    System.out.println("File transfer is terminated without completed");
                                    break;
                                }

                            }
                            if(timeout==0)
                            {
                                out.writeObject("File transfer is completed");
                                System.out.println("File transfer is completed");
                            }

                        }
                        else
                        {
                            System.out.println(trans);
                        }

                    }catch(Exception e)
                    {

                    }
                }
            }
            else if(capability.equalsIgnoreCase("Download"))
            {
                out.writeObject("Download Files");

                String user,fName,type;
                System.out.println("User name: ");
                user=sc.nextLine();
                System.out.println("File type: ");
                type=sc.nextLine();
                System.out.println("File name: ");
                fName=sc.nextLine();


                out.writeObject(user);
                out.writeObject(type);
                out.writeObject(fName);

                String alw= (String) in.readObject();
                System.out.println(alw);

                if(alw.equalsIgnoreCase("You are allowed to download the files"))
                {
                    String ex= (String) in.readObject();

                    if(ex.equalsIgnoreCase("exists"))
                    {

                        System.out.println(ex);

                        int s=0;
                        String chunk= (String) in.readObject();
                        int chunk_size= Integer.parseInt(chunk);
                        String fsize= (String) in.readObject();
                        int filesize= Integer.parseInt(fsize);

                        System.out.println(filesize);

                        byte[] text=new byte[filesize];
                        int nChunk=(int)Math.ceil(text.length*1.0/chunk_size);
                        for (int i = 1; i <= nChunk; i++)
                        {
                            if (i == nChunk)
                            {
                                System.out.println(i);
                                inf.read(text, s,  text.length - s);
                            }
                            else
                            {
                                System.out.println(i);
                                inf.read(text, s,  chunk_size);
                                s += chunk_size;
                            }
                        }

                        File fil=new File("src/Server/"+uName+"/download/"+fName);
                        if(!fil.exists())
                        {
                            fil.createNewFile();
                        }
                        Path path = Paths.get("src/Server/" + uName + "/download/" +fName);
                        Files.write(path, text);

                        //download is complete msg shown here
                        String com= (String) in.readObject();
                        System.out.println(com);
                    }
                    else
                    {
                        System.out.println(ex);
                    }
                }

            }


            else if(capability.equalsIgnoreCase("Request"))
            {
                out.writeObject("Request for Files");

                String ms1= (String) in.readObject();
                System.out.println(ms1);

                System.out.println("What request you want to do?");
                String ms2= (String) in.readObject();
                int req= Integer.parseInt(ms2);

                String ms3;
                ms3=sc.nextLine();

                out.writeObject(ms3);


            }
            else if(capability.equalsIgnoreCase("Message"))
            {
                out.writeObject("Unread message");
                String ms= (String) in.readObject();

                if(ms!=null)
                {
                    int index=ms.indexOf(" ");

                    if (index != -1)
                    {
                        req_id = ms.substring(0, index);
                    }

                }
                System.out.println(ms);

            }
            else if(capability.equalsIgnoreCase("Upload on Request"))
            {
                System.out.println("File name:");
                String fName=sc.nextLine();
                String acc="Public";

                if(!(fT==null)&& fT.isAlive())
                {
                    System.out.println("Try Later!");
                }

                else
                {
                    out.writeObject("Upload on request");

                    out.writeObject(req_id);

                    try
                    {
                        File f=new File(fName);
                        f.createNewFile();


                        try {

                            System.out.println("Write the code or text you want to add in your file");
                            StringBuilder codeBuilder = new StringBuilder();

                            while (true) {
                                String line = sc.nextLine();
                                if (line.isEmpty()) {
                                    break;
                                }
                                codeBuilder.append(line).append(System.lineSeparator());
                            }

                            FileWriter writer = new FileWriter(f);
                            writer.write(codeBuilder.toString());
                            writer.close();
                            //FileWriter writer = new FileWriter(f);
                            //writer.write("Hello, World!"); // Write the content you want to write
                            //writer.close(); // Close the writer to release resources
                            //System.out.println("Content written to the file successfully.");
                        } catch (IOException e) {
                            e.printStackTrace();
                        }



                        out.writeObject(fName);
                        out.writeObject(acc);
                        int len= (int) f.length();
                        out.writeObject( String.valueOf(len));
                        String trans = (String) in.readObject();
                        if (trans.equalsIgnoreCase("Transmission is allowed"))
                        {
                            System.out.println(trans);
                            String cSize = (String) in.readObject();
                            //System.out.println(cSize);
                            int cSizei = Integer.parseInt(cSize);
                            String fId = (String) in.readObject();
                            //System.out.println(fId);
                            System.out.println("Confirmation message ! Your chunk size is: "+cSize+" and file id is: "+fId);
                            int nChunk = (int) Math.ceil((f.length() * 1.0) / cSizei);
                            //System.out.println(nChunk);
                            byte[] text = Files.readAllBytes(Paths.get(f.getPath()));
                            int s = 0;

                            int timeout=0;
                            for (int i = 1; i <= nChunk; i++)
                            {
                                if (i == nChunk)
                                {
                                    outf.write(text, s,  text.length - s);
                                }
                                else
                                {
                                    outf.write(text, s, s + cSizei);
                                    s += cSizei;
                                }
                                outf.flush();
                                socket.setSoTimeout(30000);
                                try
                                {
                                    String ack = (String) in.readObject();
                                    System.out.println(ack);
                                    out.writeObject("Got the acknowledgement message in time");

                                }
                                catch (SocketTimeoutException se)
                                {
                                    out.writeObject("TimeOut");
                                    timeout=1;
                                }
                                socket.setSoTimeout(0);
                                if(timeout==1)
                                {
                                    break;
                                }

                            }
                            if(timeout==0)
                            {
                                out.writeObject("File transfer is completed");
                                System.out.println("File transfer is completed");
                            }

                        }
                        else
                        {
                            System.out.println(trans);
                        }

                    }catch(Exception e)
                    {
                        System.out.println(e);
                    }
                }

            }
            else if(capability.equalsIgnoreCase("Log Out"))
            {
                out.writeObject("Log out");
                String ms= (String) in.readObject();
                System.out.println(ms);
                socket.close();
                return;
            }
        }
    }
}
