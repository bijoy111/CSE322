package Server;

import java.io.*;
import java.net.Socket;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.*;




public class Worker extends Thread
{
    Socket socket;
    ObjectOutputStream out;
    ObjectInputStream in;
    DataOutputStream outf;
    DataInputStream inf;
    String uName;


    public Worker(Socket socket,ObjectOutputStream out,ObjectInputStream in,String uName) throws IOException
    {
        this.socket = socket;
        this.out = out;
        this.in = in;
        this.uName=uName;
        outf = new DataOutputStream(socket.getOutputStream());
        inf = new DataInputStream(socket.getInputStream());
    }




    @Override
    public void run()
    {
        try
        {
            while (true)
            {
                String task= (String) in.readObject();
                if(task.equalsIgnoreCase("List of Clients"))
                {
                    out.writeObject(serverMain.online);
                    out.writeObject(serverMain.offline);
                }
                else if(task.equalsIgnoreCase("My files"))
                {
                    File[] fs = new File("src/Server/" + uName + "/private").listFiles();
                    ArrayList<String> files = new ArrayList<>();

                    for (var f : fs)
                    {
                        files.add(f.getName());
                    }
                    out.writeObject(files);


                    fs = new File("src/Server/" + uName + "/public").listFiles();
                    files = new ArrayList<>();

                    for (var f : fs)
                    {
                        files.add(f.getName());
                    }
                    out.writeObject(files);

                }
                else if(task.equalsIgnoreCase("Others public files"))
                {
                    out.writeObject(serverMain.online.size());
                    for(String nm:serverMain.online)
                    {
                        if(!uName.equalsIgnoreCase(nm))
                        {
                            out.writeObject(nm);
                            File[] fs = new File("src/Server/" + nm + "/public").listFiles();
                            ArrayList<String> files = new ArrayList<>();

                            for (var f : fs)
                            {
                                files.add(f.getName());
                            }
                            out.writeObject(files);
                        }
                    }
                }
                else if(task.equalsIgnoreCase("Upload Files"))
                {
                    String fName= (String) in.readObject();
                    //System.out.println(fName);
                    String acc= (String) in.readObject();
                    //System.out.println(acc);

                    String fSize= (String) in.readObject();
                    System.out.println(uName+" is trying to upload a "+acc+" file named "+fName +" with size of "+fSize);
                    int fSizei= Integer.parseInt(fSize);
                    if((serverMain.Current_Buffer_Size+fSizei)>serverMain.Max_Buffer_Size)
                    {
                        out.writeObject("Transmission is not allowed");
                        System.out.println("Transmission is not allowed");
                    }
                    else
                    {
                        out.writeObject("Transmission is allowed");
                        System.out.println("Transmission is allowed");
                        int random_chunk = (int)Math.floor(Math.random() * (serverMain.Max_Chunk_Size - serverMain.Min_Chunk_Size + 1) + serverMain.Min_Chunk_Size);
                        out.writeObject(String.valueOf(random_chunk));
                        serverMain.nFile++;
                        out.writeObject(String.valueOf(serverMain.nFile));
                        //System.out.println( serverMain.nFile);
                        //serverMain.lFile.put(String.valueOf(serverMain.nFile),fName);
                        //System.out.println("hey");
                        int nChunk= (int) Math.ceil ((fSizei*1.0)/random_chunk);
                        //System.out.println(nChunk);
                        byte[] text=new byte[fSizei];
                        int s=0;

                        int timeout=0;
                        for(int i=1;i<=nChunk;i++)
                        {
                            if(i==nChunk)
                            {
                                inf.read(text,s,  fSizei-s);
                                out.writeObject("Got the "+"last"+"  chunk of the file");
                                System.out.println("Got the "+"last"+"  chunk of the file");
                            }
                            else
                            {
                                inf.read(text,s,random_chunk);
                                s+=random_chunk;
                                out.writeObject("Got the "+i+"  chunk of the file");
                                System.out.println("Got the "+i+"  chunk of the file");
                            }
                            String msg= (String) in.readObject();
                            if(msg.equalsIgnoreCase("TimeOut"))
                            {
                                System.out.println("Received the timeout message from client! So the chunk files for the file with id "+serverMain.nFile+" are deleted");
                                serverMain.nFile--;
                                timeout=1;
                                break;
                            }


                        }
                        if(timeout==0)
                        {
                            //System.out.println("timeout");
                            String com= (String) in.readObject();
                            System.out.println(com);
                            if(com.equalsIgnoreCase("File transfer is completed"))
                            {
                                Path path = Paths.get("src/Server/" + uName + "/" + acc + "/" + fName);
                                Files.write(path, text);
                                File file = new File("src/Server/" + uName + "/" + acc + "/" + fName);
                                file.createNewFile();
                            }
                        }
                        else
                        {
                            serverMain.lFile.remove(String.valueOf(serverMain.nFile));
                            serverMain.nFile--;
                        }
                    }
                }
                else if(task.equalsIgnoreCase("Download Files"))
                {
                    String user= (String) in.readObject();
                    String type= (String) in.readObject();
                    String fName= (String) in.readObject();


                    if(!user.equalsIgnoreCase(uName)&&type.equalsIgnoreCase("private"))
                    {
                        out.writeObject("You can not download the private file of other clients");
                    }
                    else
                    {
                        out.writeObject("You are allowed to download the files");
                        File f=new File("src/Server/" + user + "/" + type + "/" + fName);
                        if(!f.exists())
                        {
                            out.writeObject(fName + " is not exist");
                        }
                        else
                        {
                            out.writeObject("exists");
                            File f1=new File("src/Server/" + uName + "/download");
                            if(!f1.exists())
                            {
                                f1.mkdir();
                            }

                            File file = new File("src/Server/" + user + "/" + type + "/" + fName);

                            //file.createNewFile();

                            int chunk= serverMain.Max_Chunk_Size;

                            byte[] text=Files.readAllBytes(Paths.get(file.getPath()));
                            int nChunk = (int) Math.ceil(text.length * 1.0 / chunk);

                            int s=0;

                            out.writeObject(String.valueOf(chunk));
                            out.writeObject(String.valueOf(text.length));

                            for(int i=1;i<=nChunk;i++)
                            {
                                if(i==nChunk)
                                {
                                    outf.write(text,s,  text.length-s);
                                }
                                else
                                {
                                    outf.write(text,s,chunk);
                                    s+=chunk;
                                }
                            }


                            out.writeObject("Download is complete");
                        }
                    }
                }


                else if(task.equalsIgnoreCase("Request for Files"))
                {
                    serverMain.req++;

                    serverMain.req_user.put(String.valueOf(serverMain.req),uName);

                    out.writeObject("You request id is "+String.valueOf(serverMain.req));

                    out.writeObject(String.valueOf(serverMain.req));

                    String msg= (String) in.readObject();

                    System.out.println("Request of "+uName+" is: "+msg);

                    msg="Request id is : "+String.valueOf(serverMain.req)+" , "+ uName +" send this message and the message is: "+" "+msg;

                   for(String s:serverMain.total)
                   {
                       //System.out.println(s);
                       if(!s.equalsIgnoreCase(uName))
                       {
                           if(serverMain.map.containsKey(s))
                           {
                               String msg1=serverMain.map.get(s);
                               msg1=msg1+" \n";
                               msg1+=msg;
                               serverMain.map.put(s,msg1);
                               System.out.println(msg1);
                           }
                           else
                           {
                               serverMain.map.put(s,msg);
                               System.out.println(msg);
                               //System.out.println("yes"+s);
                           }
                       }
                   }
                }
                else if(task.equalsIgnoreCase("Unread message"))
                {
                    String msg=serverMain.map.get(uName);
                    out.writeObject(msg);
                    serverMain.map.remove(uName);
                }
                else if(task.equalsIgnoreCase("Upload on request"))
                {
                    String req_id= (String) in.readObject();
                    String u_id=serverMain.req_user.get(req_id);

                    String fName= (String) in.readObject();
                    //System.out.println(fName);
                    String acc= (String) in.readObject();
                    //System.out.println(acc);
                    String fSize= (String) in.readObject();
                    System.out.println(uName+" is trying to upload a "+acc+" file named "+fName +" with size of "+fSize);
                    int fSizei= Integer.parseInt(fSize);
                    if((serverMain.Current_Buffer_Size+fSizei)>serverMain.Max_Buffer_Size)
                    {
                        out.writeObject("Transmission is not allowed");
                    }
                    else
                    {
                        out.writeObject("Transmission is allowed");
                        System.out.println("Transmission is allowed");
                        int random_chunk = (int)Math.floor(Math.random() * (serverMain.Max_Chunk_Size - serverMain.Min_Chunk_Size + 1) + serverMain.Min_Chunk_Size);
                        out.writeObject(String.valueOf(random_chunk));
                        serverMain.nFile++;
                        out.writeObject(String.valueOf(serverMain.nFile));
                        //System.out.println( serverMain.nFile);
                        //serverMain.lFile.put(String.valueOf(serverMain.nFile),fName);
                        //System.out.println("hey");
                        int nChunk= (int) Math.ceil ((fSizei*1.0)/random_chunk);
                        //System.out.println(nChunk);
                        byte[] text=new byte[fSizei];
                        int s=0;

                        int timeout=0;
                        for(int i=1;i<=nChunk;i++)
                        {
                            if(i==nChunk)
                            {
                                inf.read(text,s,  fSizei-s);
                                out.writeObject("Got the "+"last"+"  chunk of the file");
                                System.out.println("Got the "+"last"+"  chunk of the file");
                            }
                            else
                            {
                                inf.read(text,s,random_chunk);
                                s+=random_chunk;
                                out.writeObject("Got the "+i+"  chunk of the file");
                                System.out.println("Got the "+i+"  chunk of the file");
                            }
                            String msg= (String) in.readObject();
                            if(msg.equalsIgnoreCase("TimeOut"))
                            {
                                System.out.println("Received the timeout message from client! So the chunk files for the file with id "+serverMain.nFile+" are deleted");
                                serverMain.nFile--;
                                timeout=1;
                                break;
                            }


                        }
                        if(timeout==0)
                        {
                            //System.out.println("timeout");
                            String com= (String) in.readObject();
                            System.out.println(com);
                            if(com.equalsIgnoreCase("File transfer is completed"))
                            {
                                Path path = Paths.get("src/Server/" + uName + "/" + acc + "/" + fName);
                                Files.write(path, text);
                                File file = new File("src/Server/" + uName + "/" + acc + "/" + fName);
                                file.createNewFile();

                                if(serverMain.map.containsKey(u_id))
                                {
                                    String msg1=serverMain.map.get(u_id);
                                    msg1=msg1+"\n";
                                    msg1+=uName+" uploaded your requested file";
                                    serverMain.map.put(u_id,msg1);
                                    System.out.println(msg1);
                                }
                                else
                                {
                                    serverMain.map.put(u_id,uName+" uploaded your requested file");
                                    System.out.println(uName+" uploaded your requested file");
                                    //System.out.println("yes"+s);
                                }

                            }



                        }
                        else
                        {
                            serverMain.lFile.remove(String.valueOf(serverMain.nFile));
                            serverMain.nFile--;
                        }
                    }
                }
                else if(task.equalsIgnoreCase("Log out"))
                {
                    serverMain.online.remove(uName);
                    serverMain.offline.add(uName);
                    out.writeObject("Your are logged out");
                    System.out.println(uName+" is logged out");
                }

            }
        }
        catch(Exception e)
        {

        }

    }
}
