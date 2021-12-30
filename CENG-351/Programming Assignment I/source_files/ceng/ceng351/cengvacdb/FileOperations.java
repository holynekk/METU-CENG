package ceng.ceng351.cengvacdb;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;


public class FileOperations {

    public static FileWriter createFileWriter( String path) throws IOException {
        File f = new File( path);

        FileWriter fileWriter = null;

        if( f.isDirectory() && !f.exists())
            f.mkdirs();
        else if( !f.isDirectory() && !f.getParentFile().exists())
            f.getParentFile().mkdirs();

        if( !f.isDirectory() && f.exists())
            f.delete();

        fileWriter = new FileWriter( f, false);

        return fileWriter;
    }

    public static User[] readUserFile(String pathToFile){

        FileReader fileReader = null;
        BufferedReader bufferedReader = null; 

        String strLine;

        List<User> userList = new ArrayList<>();
        User[] userArray = null;
        
        try {

            fileReader = new FileReader(pathToFile);
            bufferedReader = new BufferedReader(fileReader);

            //example strline
            //userID	name    age    address    password    status

            while((strLine = bufferedReader.readLine())!=null) {

                //parse strLine
                String[] words = strLine.split("\t");
                
                if (words.length < 6) {
                    System.out.println("There is a problem in User File Reading phase");
                } 
                else {
                    int userID = Integer.parseInt(words[0]);
                    String name = words[1];
                    int age = Integer.parseInt(words[2]);
                    String address = words[3];
                    String password = words[4];
                    String status = words[5];

                    User user = new User(userID, name, age, address, password, status);
                    userList.add(user);
                }

            }//End of while

            //Close bufferedReader
            bufferedReader.close();

            userArray = new User[userList.size()];
            userList.toArray(userArray);

        } 
        catch (IOException e) {
            e.printStackTrace();
        }
        
        return userArray;
    }
    
    public static Vaccine[] readVaccineFile(String pathToFile) {
        
        FileReader fileReader = null;
        BufferedReader bufferedReader = null; 
        String strLine;
        List<Vaccine> vaccineList = new ArrayList<>();
        Vaccine[] vaccineArray = null;
        
        try {
            fileReader = new FileReader(pathToFile);
            bufferedReader = new BufferedReader(fileReader);

            //example strline
            //code	name    type

            while((strLine = bufferedReader.readLine())!=null){

                //parse strLine
                String[] words = strLine.split("\t");
                
                if (words.length < 3) {
                    System.out.println("There is a problem in Vaccine File Reading phase");
                } 
                else {
                    int code = Integer.parseInt(words[0]);
                    String name = words[1];
                    String type = words[2];

                    Vaccine vaccine = new Vaccine(code, name, type);
                    vaccineList.add(vaccine);
                }

            }//End of while

            //Close bufferedReader
            bufferedReader.close();

            vaccineArray = new Vaccine[vaccineList.size()];
            vaccineList.toArray(vaccineArray);

        } 
        catch (IOException e) {
            e.printStackTrace();
        }

        return vaccineArray;

    }
    
    public static Vaccination[] readVaccinationFile(String pathToFile){

        FileReader fileReader = null;
        BufferedReader bufferedReader = null; 

        String strLine;

        List<Vaccination> vaccinationList = new ArrayList<>();
        Vaccination[] vaccinationArray = null;
        
        try {

            fileReader = new FileReader(pathToFile);
            bufferedReader = new BufferedReader(fileReader);

            //example strline
            //code	userID	dose	vacdate

            while((strLine = bufferedReader.readLine())!=null) {

                //parse strLine
                String[] words = strLine.split("\t");
                
                if (words.length < 4) {
                    System.out.println("There is a problem in Vaccination File Reading phase");
                } 
                else {
                    int code = Integer.parseInt(words[0]);
                    int userID = Integer.parseInt(words[1]);
                    int dose = Integer.parseInt(words[2]);
                    String vacdate = words[3];

                    Vaccination vaccination = new Vaccination(code, userID, dose, vacdate);
                    vaccinationList.add(vaccination);
                }

            }//End of while

            //Close bufferedReader
            bufferedReader.close();

            vaccinationArray = new Vaccination[vaccinationList.size()];
            vaccinationList.toArray(vaccinationArray);

        }
        catch (IOException e) {
            e.printStackTrace();
        }
        
        return vaccinationArray;
    }
    
    public static AllergicSideEffect[] readAllergicSideEffectFile(String pathToFile){

        FileReader fileReader = null;
        BufferedReader bufferedReader = null; 
        String strLine;

        List<AllergicSideEffect> sideEffectList = new ArrayList<>();
        AllergicSideEffect[] sideEffectArray = null;
        
        try {

            fileReader = new FileReader(pathToFile);
            bufferedReader = new BufferedReader(fileReader);

            //example strline
            //code	name
            while((strLine = bufferedReader.readLine())!=null) {

                //parse strLine
                String[] words = strLine.split("\t");
                
                if (words.length < 2) {
                    System.out.println("There is a problem in AllergicSideEffect File Reading phase");
                } 
                else {
                    int code = Integer.parseInt(words[0]);
                    String name = words[1];

                    AllergicSideEffect allergicSideEffect = new AllergicSideEffect(code, name);
                    sideEffectList.add(allergicSideEffect);
                }

            }//End of while

            //Close bufferedReader
            bufferedReader.close();

            sideEffectArray = new AllergicSideEffect[sideEffectList.size()];
            sideEffectList.toArray(sideEffectArray);

        }
        catch (IOException e) {
            e.printStackTrace();
        }
        
        return sideEffectArray;
    }
    
    public static Seen[] readSeenFile(String pathToFile){

        FileReader fileReader = null;
        BufferedReader bufferedReader = null; 
        String strLine;

        List<Seen> seenList = new ArrayList<>();
        Seen[] seenArray = null;
        
        try {

            fileReader = new FileReader(pathToFile);
            bufferedReader = new BufferedReader(fileReader);

            //example strline
            //e_code	code    userID  date   degree
            while((strLine = bufferedReader.readLine())!=null) {

                //parse strLine
                String[] words = strLine.split("\t");
                
                if (words.length < 5) {
                    System.out.println("There is a problem in Seen File Reading phase");
                } 
                else {
                    int e_code = Integer.parseInt(words[0]);
                    int code = Integer.parseInt(words[1]);
                    String userID = words[2];
                    String date = words[3];
                    String degree = words[4];

                    Seen seen = new Seen(e_code, code, userID, date, degree);
                    seenList.add(seen);
                }

            }//End of while

            //Close bufferedReader
            bufferedReader.close();

            seenArray = new Seen[seenList.size()];
            seenList.toArray(seenArray);

        }  
        catch (IOException e) {
            e.printStackTrace();
        }
        
        return seenArray;
    }

}
