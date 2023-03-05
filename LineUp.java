import java.util.*;
import java.io.*;

public class LineUp{
   public static void main(String[] args) 
   {
      Scanner sc = new Scanner(System.in);
      try{
         sc = new Scanner(new File("lineup.in"));
      }
      catch(FileNotFoundException e){
         System.out.println(e);
         
      }
      int num = sc.nextInt();
      ArrayList<String> a = new ArrayList<>();
      for(int i = 0; i<num+1; i++){
         a.add(sc.nextLine());
      }
      a.remove(0);
      System.out.println(a);
      String[] names = {"Bessie", "Buttercup", "Belinda", "Beatrice", "Bella", "Blue", "Betsy","Sue"};
      ArrayList<Pair> pairs = new ArrayList<>();
   
      for(String s:a){
         String[] strA = s.split(" must be milked beside ");
         pairs.add(new Pair(strA[0], strA[1]));
      }
     //  BSTobject<String> tree = null;
   //       tree.add(pairs.get(0).getName1());
   //       tree.add(tree.contains(pairs.get(0).getName1()), pairs.get(0).getName2());
   //   
      String[] sa = checkAllRecursive(names.length, names, pairs);
      try{
         FileWriter myWriter = new FileWriter("lineup.out");
         for(String s: sa)
            myWriter.write(s+"\n");
         myWriter.close();
      }
      catch(IOException e){
      }
   
    
   }
   public static boolean works(String[] line, ArrayList<Pair> pa){
      int num = 0;
   
      for(int i = 1; i<line.length-1; i++){
         for(Pair p : pa){
            if(i<line.length-1&&((line[i].equals(p.getName1())&&line[i-1].equals(p.getName2())) ||(line[i].equals(p.getName2())&&line[i-1].equals(p.getName1()))))
               num++;
            if(i<line.length-1&&((line[i].equals(p.getName1())&&line[i+1].equals(p.getName2()))||(line[i].equals(p.getName2())&&line[i+1].equals(p.getName1()))))
            {
               num++;
               i+=2;
            }
         }
         if(num==pa.size())
            return true;
      }
      return false;
   }
   public static String[]  checkAllRecursive(  int n, String[] elements, ArrayList<Pair> pa) {
      ArrayList<String[]> array = new ArrayList<>();
      int[] indexes = new int[n];
      for (int i = 0; i < n; i++) {
         indexes[i] = 0;
      }
      int i = 0;
      while (i < n) {
         if(works(elements, pa)){
            if(!array.contains(elements))
               array.add(elements);
            
            }
            
         if (indexes[i] < i) {
            elements = swap(elements, i % 2 == 0 ?  0: indexes[i], i);
            indexes[i]++;
            i = 0;
         }
         else {
            indexes[i] = 0;
            i++;
         }
      }
     String first = array.get(0)[0];
         int index = 0;
      for( i = 1; i<array.size()-1; i++){
         int x = 0;
         if(array.get(i)[x].compareTo(first)>0){
            first = array.get(i)[x];
            index = i;
            }
         else if(array.get(i)[x].compareTo(first)==0){
            while(x<7){
               x++;
               if(array.get(i)[x].compareTo(first)>0){
                  first = array.get(i)[x];
                   index = i;}
               else if(array.get(i)[x].compareTo(first)==0)
                  x++;
              }
         }
      }
      return  array.get(index);
   
   }
   public static String[] swap(String[] input, int a, int b) {
      String tmp = input[a];
      input[a] = input[b];
      input[b] = tmp;
      return input;
   }
  
}
class Pair{
   String name1, name2;
   Pair(String n1, String n2){
      this.name1 = n1;
      this.name2 = n2;
   }
   String getName1(){
      return name1;
   }
   String getName2(){
      return name2;
   }
}
