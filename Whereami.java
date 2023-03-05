import java.util.*;
import java.io.*;

public class Whereami{
   public static void main(String[] args) 
   {
      Scanner sc = new Scanner(System.in);
      try{
          sc = new Scanner(new File("whereami.in"));
      }
      catch(FileNotFoundException e){
         System.out.println(e);}
      int k = sc.nextInt();
      String line = "ABCDABC";
     // System.out.println(works(line));
     try{
         FileWriter myWriter = new FileWriter("whereami.out");
         String i= works(line);
         myWriter.write(i);
          myWriter.close();
          }
          catch(IOException e){
          }
   
   }
   public static String works(String line){
      Set<String> boxes = new HashSet<String>();
      int size = 0;
      String s = "";
      for(int x = 0; x<line.length(); x++){
         s = line.substring(0, x+1);
         boolean works = true;
         for(int j = s.length(); j<line.length()-s.length(); j++){
            if(!s.equals(line.substring(j, j+s.length()))){
               works = false;
               break;
            }
         
         }
         if(works)
            break;
      
      }
      return ""+s.length();
   
   }
}
