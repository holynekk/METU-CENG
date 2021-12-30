package ceng.ceng351.cengvacdb;

import ceng.ceng351.cengvacdb.QueryResult.UserIDuserNameAddressResult;
import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;
import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.text.DecimalFormat;
import java.util.Vector;


public class Evaluation {
    
    private static String user = "e1234567"; // TODO: Your userName
    private static String password = "password"; //  TODO: Your password
    private static String host = "144.122.71.57"; // host name
    private static String database = "db1234567"; // TODO: Your database name
    private static int port = 8084; // port
    
    private static Connection connection = null;

    public static void connect() {
		
        String url = "jdbc:mysql://" + host + ":" + port + "/" + database;

        try {
            Class.forName("com.mysql.cj.jdbc.Driver");
            connection =  DriverManager.getConnection(url, user, password);
        } 
        catch (SQLException | ClassNotFoundException e) {
            e.printStackTrace();
        } 
    }
    
    public static void disconnect() {
		
        if (connection != null) {
            try {
                connection.close();
            } catch (SQLException e) {
                e.printStackTrace();
            }
        }
    }
    
    public static void addInputTitle(String title, BufferedWriter bufferedWriter) throws IOException {
        bufferedWriter.write("*** " + title + " ***" + System.getProperty("line.separator"));
    }

    private static final DecimalFormat df = new DecimalFormat("0.00");

    public static void printAllTables(BufferedWriter bufferedWriter) throws IOException {
		
        String sql1 = "show tables";
        String sql2 = "describe ";

        Vector<String> tables = new Vector<>();

        try
        {
            // Execute query
            Statement st = connection.createStatement();
            ResultSet rs = st.executeQuery(sql1);

            // Process the result set
            while(rs.next()) {
                tables.add(rs.getString(1));
            }

            for(int i=0; i < tables.size(); i++) {
                rs = st.executeQuery(sql2 + tables.get(i));

                // Print table name
                bufferedWriter.write("--- " + tables.get(i) + " ---" + System.getProperty("line.separator"));

                // Print field names and types
                while(rs.next()) {
                        bufferedWriter.write(rs.getString(1) + " " + rs.getString(2) + System.getProperty("line.separator"));
                }

                bufferedWriter.write(System.getProperty("line.separator"));
            }

        } catch (SQLException e) {
            printException(e);
        }
    }
    
    private static void printException(SQLException ex) {
        System.out.println(ex.getMessage() + "\n");
    }
	
    public static void printLine(String result, BufferedWriter bufferedWriter) throws IOException {
        bufferedWriter.write(result + System.getProperty("line.separator"));
    }

    public static void addDivider(BufferedWriter bufferedWriter) throws IOException {
        bufferedWriter.write( System.getProperty("line.separator")+ "--------------------------------------------------------------" + System.getProperty("line.separator"));
    }
    

    public static void main(String[] args) {

       int numberofInsertions = 0;
       int numberofTablesCreated = 0;
       int numberofTablesDropped = 0;

       /***********************************************************/
       // TODO While running on your local machine, change musicdbDirectory accordingly
       String cengvacdbDirectory = "src" + System.getProperty("file.separator") +
               "ceng" + System.getProperty("file.separator") +
                       "ceng351" + System.getProperty("file.separator") +
                       "cengvacdb";
       /***********************************************************/

       FileWriter fileWriter = null;
       BufferedWriter bufferedWriter = null;

       //Connect to the database
       connect();

       // Create CENGVACDB object
       CENGVACDB cengvacDB = null;
       
       String vacdate = "2021-10-01";
       String givendate = "2021-12-19";
       String startdate = "2021-01-01";
       String enddate = "2021-12-19";
       String effectname = "loss of speech";
       String vaccineName = "Convidecia";

       try {
            // Create CENGVACDB object and initialize
            cengvacDB = new CENGVACDB();
            cengvacDB.initialize();

            /***********************************************************/
            /*************Create File Writer starts*********************/
            /***********************************************************/
            fileWriter = FileOperations.createFileWriter(cengvacdbDirectory + System.getProperty("file.separator") + "output" + System.getProperty("file.separator") + "Output.txt");
            bufferedWriter =  new BufferedWriter(fileWriter);
            /***********************************************************/
            /*************Create File Writer ends***********************/
            /***********************************************************/

            /***********************************************************/
            /*************Drop tables starts****************************/
            /***********************************************************/
            addDivider(bufferedWriter);
            addInputTitle("Drop tables", bufferedWriter);
            numberofTablesDropped = 0;

            // Drop tables
            try {
                numberofTablesDropped = cengvacDB.dropTables();
            } catch(Exception e) {
                e.printStackTrace();
            }

            // Check if tables are dropped
            printLine("Dropped " + numberofTablesDropped + " tables.", bufferedWriter);

            addDivider(bufferedWriter);
            /***********************************************************/
            /*************Drop tables ends******************************/
            /***********************************************************/

            /***********************************************************/
            /*******************Create tables starts********************/
            /***********************************************************/
            addDivider(bufferedWriter);
            addInputTitle("Create tables",bufferedWriter);
            numberofTablesCreated = 0;

            // Create Tables
            try {
                numberofTablesCreated = cengvacDB.createTables();

                // Check if tables are created
                printLine("Created " + numberofTablesCreated + " tables.", bufferedWriter);

            } catch(Exception e) {
                printLine("Q3.1: Exception occured: \n\n" + e.toString(),bufferedWriter);
            }

            addDivider(bufferedWriter);
            /***********************************************************/
            /*******************Create tables ends**********************/
            /***********************************************************/

            /***********************************************************/
            /*******************Insert INTO User starts**********/
            /***********************************************************/
            addDivider(bufferedWriter);
            addInputTitle("Insert into User",bufferedWriter);
            //insert User
            numberofInsertions = 0;
            User[] users = FileOperations.readUserFile(cengvacdbDirectory +
                            System.getProperty("file.separator") + "data" + System.getProperty("file.separator") +
                            "Test_UserFile.txt");

            numberofInsertions = cengvacDB.insertUser(users);
            printLine( numberofInsertions + " users are inserted.",bufferedWriter);
            addDivider(bufferedWriter);
            /***********************************************************/
            /*******************Insert INTO User ends************/
            /***********************************************************/


           /***********************************************************/
           /*******************Insert INTO Vaccine starts*************/
           /***********************************************************/
           addDivider(bufferedWriter);
           addInputTitle("Insert into Vaccine",bufferedWriter);
           //insert vaccine
           numberofInsertions = 0;
           Vaccine[] vaccines = FileOperations.readVaccineFile(cengvacdbDirectory +
                   System.getProperty("file.separator") + "data" + System.getProperty("file.separator") +
                   "Test_VaccineFile.txt");

           numberofInsertions = cengvacDB.insertVaccine(vaccines);
           printLine(numberofInsertions + " vaccines are inserted.",bufferedWriter);
           addDivider(bufferedWriter);
           /***********************************************************/
           /*******************Insert INTO Vaccine ends***************/
           /***********************************************************/

            /***********************************************************/
            /*******************Insert INTO Vaccination starts*****************/
            /***********************************************************/
            addDivider(bufferedWriter);
            addInputTitle("Insert into Vaccination",bufferedWriter);
            //insert Vaccination
            numberofInsertions = 0;
            Vaccination[] vaccinations = FileOperations.readVaccinationFile( cengvacdbDirectory + System.getProperty("file.separator")
                    + "data" + System.getProperty("file.separator") + "Test_VaccinationFile.txt");

            numberofInsertions = cengvacDB.insertVaccination(vaccinations);
            printLine( numberofInsertions + " vaccinations are inserted.",bufferedWriter);
            addDivider(bufferedWriter);
            /***********************************************************/
            /*******************Insert INTO Vaccination ends*******************/
            /***********************************************************/

            /***********************************************************/
            /*******************Insert INTO AllergicSideEffect starts**********/
            /***********************************************************/
            addDivider(bufferedWriter);
            addInputTitle("Insert into AllergicSideEffect",bufferedWriter);
            //insert AllergicSideEffect
            numberofInsertions = 0;
            AllergicSideEffect[] sideEffects = FileOperations.readAllergicSideEffectFile(cengvacdbDirectory +
                            System.getProperty("file.separator") + "data" + System.getProperty("file.separator") +
                            "Test_AllergicSideEffectFile.txt");

            numberofInsertions = cengvacDB.insertAllergicSideEffect(sideEffects);
            printLine( numberofInsertions + " side effects are inserted.", bufferedWriter);
            addDivider(bufferedWriter);
            /***********************************************************/
            /*******************Insert INTO AllergicSideEffect ends************/
            /***********************************************************/


            /***********************************************************/
            /*******************Insert INTO Seen starts*************/
            /***********************************************************/
            addDivider(bufferedWriter);
            addInputTitle("Insert into Seen",bufferedWriter);
            //insert Seen
            numberofInsertions = 0;
            Seen[] seens = FileOperations.readSeenFile(cengvacdbDirectory +
                            System.getProperty("file.separator") + "data" + System.getProperty("file.separator") +
                            "Test_SeenFile.txt");

            numberofInsertions = cengvacDB.insertSeen(seens);
            printLine(numberofInsertions + " seens are inserted.",bufferedWriter);
            addDivider(bufferedWriter);
            /***********************************************************/
            /*******************Insert INTO Seen ends***************/
            /***********************************************************/
            
            /***********************************************************/
            /***********************************************************/
            /***********************************************************/
            addDivider(bufferedWriter);
            addInputTitle("Find vaccines that have not been applied to any user",bufferedWriter);
            try {

                Vaccine[] VaccineResultArray1 = cengvacDB.getVaccinesNotAppliedAnyUser();

                //Header Line
                printLine("Code" + "\t" + "Vaccinename" + "\t" + "Type",bufferedWriter);

                if(VaccineResultArray1 != null) {
                    for(Vaccine Vaccine : VaccineResultArray1){
                        printLine(Vaccine.toString(),bufferedWriter);
                    }	
                }


            } catch(Exception e) {
                printLine("Q3.3: Exception occured: \n\n" + e.toString(),bufferedWriter);
            }
            addDivider(bufferedWriter);
            /***********************************************************/
            /***********************************************************/
            /***********************************************************/
            
            /***********************************************************/
            /***********************************************************/
            /***********************************************************/
            addDivider(bufferedWriter);
            addInputTitle("List users who have been vaccinated for two doses since the given date",bufferedWriter);
            try {

                QueryResult.UserIDuserNameAddressResult[] UserIDuserNameAddressResultArray1 = cengvacDB.getVaccinatedUsersforTwoDosesByDate(vacdate);

                //Header Line
                printLine("UserID" + "\t" + "UserName" + "\t" + "Address",bufferedWriter);

                if(UserIDuserNameAddressResultArray1 != null) {
                    for(QueryResult.UserIDuserNameAddressResult UserIDuserNameAddressResult : UserIDuserNameAddressResultArray1){
                        printLine(UserIDuserNameAddressResult.toString(),bufferedWriter);
                    }
                }

            } catch(Exception e) {
                printLine("Q3.4: Exception occured: \n\n" + e.toString(),bufferedWriter);
            }
            addDivider(bufferedWriter);
            /***********************************************************/
            /***********************************************************/
            /***********************************************************/
   
            /***********************************************************/
            /***********************************************************/
            /***********************************************************/
            addDivider(bufferedWriter);
            addInputTitle("List of the two recent vaccines applied to any user that do not contain ‘vac’ in their name",bufferedWriter);
            try {

                Vaccine[] VaccineResultArray2 = cengvacDB.getTwoRecentVaccinesDoNotContainVac();

                //Header Line
                printLine("Code" + "\t" + "Vaccinename" + "\t" + "Type",bufferedWriter);

                if(VaccineResultArray2 != null) {
                    for(Vaccine Vaccine : VaccineResultArray2){
                        printLine(Vaccine.toString(),bufferedWriter);
                    }
                }

            } catch(Exception e) {
                printLine("Q3.5: Exception occured: \n\n" + e.toString(),bufferedWriter);
            }

            addDivider(bufferedWriter);
            /***********************************************************/
            /***********************************************************/
            /***********************************************************/
            
            
            /***********************************************************/
            /***********************************************************/
            /***********************************************************/
            addDivider(bufferedWriter);
            addInputTitle("List users who have had at least two doses of vaccine and have experienced at most one side effect",bufferedWriter);
            try {

                QueryResult.UserIDuserNameAddressResult[] UserIDuserNameAddressResultArray2 = cengvacDB.getUsersAtHasLeastTwoDoseAtMostOneSideEffect();

                //Header Line
                printLine("UserID" + "\t" + "UserName" + "\t" + "Address",bufferedWriter);

                if(UserIDuserNameAddressResultArray2 != null) {
                    for(QueryResult.UserIDuserNameAddressResult UserIDuserNameAddressResult : UserIDuserNameAddressResultArray2){
                        printLine(UserIDuserNameAddressResult.toString(),bufferedWriter);
                    }
                }
            } catch(Exception e) {
                printLine("Q3.6: Exception occured: \n\n" + e.toString(),bufferedWriter);
            }
            addDivider(bufferedWriter);
            /***********************************************************/
            /***********************************************************/
            /***********************************************************/

            /***********************************************************/
            /***********************************************************/
            /***********************************************************/
            addDivider(bufferedWriter);
            addInputTitle("List users who have been vaccinated with all vaccines that can cause the given side effect",bufferedWriter);
            try {

                UserIDuserNameAddressResult[] UserIDuserNameAddressResultArray3 = cengvacDB.getVaccinatedUsersWithAllVaccinesCanCauseGivenSideEffect(effectname);

                //Header Line
                printLine("UserID" + "\t" + "UserName" + "\t" + "Address",bufferedWriter);

                if(UserIDuserNameAddressResultArray3 != null) {
                    for(QueryResult.UserIDuserNameAddressResult UserIDuserNameAddressResult : UserIDuserNameAddressResultArray3){
                        printLine(UserIDuserNameAddressResult.toString(),bufferedWriter);
                    }
                }


            } catch(Exception e) {
                printLine("Q3.7: Exception occured: \n\n" + e.toString(),bufferedWriter);
            }
            addDivider(bufferedWriter);
            /***********************************************************/
            /***********************************************************/
            /***********************************************************/
            
            /***********************************************************/
            /***********************************************************/
            /***********************************************************/
            addDivider(bufferedWriter);
            addInputTitle("List users who have been vaccinated with at least two different types of vaccines within the given time interval",bufferedWriter);
            try {

                QueryResult.UserIDuserNameAddressResult[] UserIDuserNameAddressResultArray4  = cengvacDB.getUsersWithAtLeastTwoDifferentVaccineTypeByGivenInterval(startdate, enddate);

                //Header Line
                printLine("UserID" + "\t" + "UserName" + "\t" + "Address",bufferedWriter);

                if(UserIDuserNameAddressResultArray4 != null) {
                    for(QueryResult.UserIDuserNameAddressResult UserIDuserNameAddressResult : UserIDuserNameAddressResultArray4){
                        printLine(UserIDuserNameAddressResult.toString(),bufferedWriter);
                    }
                }

            } catch(Exception e) {
                printLine("Q3.8: Exception occured: \n\n" + e.toString(),bufferedWriter);
            }
            addDivider(bufferedWriter);
            /***********************************************************/
            /***********************************************************/
            /***********************************************************/
            
            /***********************************************************/
            /***********************************************************/
            /***********************************************************/
            addDivider(bufferedWriter);
            addInputTitle("List side effects experienced by users who have two doses of vaccine with less than 20 days between doses",bufferedWriter);

            try {

                AllergicSideEffect[] AllergicSideEffectArray = cengvacDB.getSideEffectsOfUserWhoHaveTwoDosesInLessThanTwentyDays();
                //Header Line
                printLine("EffectCode" + "\t" + "EffectName", bufferedWriter);
                
                if (AllergicSideEffectArray != null) {
                    for(AllergicSideEffect AllergicSideEffect : AllergicSideEffectArray){
                        printLine(AllergicSideEffect.toString(),bufferedWriter);
                    }
                }

            } catch(Exception e) {
                printLine("Q3.9: Exception occured: \n\n" + e.toString(),bufferedWriter);
            }
            addDivider(bufferedWriter);
            /***********************************************************/
            /***********************************************************/
            /***********************************************************/
            
            /***********************************************************/
            /***********************************************************/
            /***********************************************************/
            addDivider(bufferedWriter);
            addInputTitle("Calculate the average number of doses of vaccinated users over the age of 65",bufferedWriter);
            try {

                double average = cengvacDB.averageNumberofDosesofVaccinatedUserOverSixtyFiveYearsOld();

                //Header Line
                printLine("Average", bufferedWriter);
                printLine(df.format(average), bufferedWriter);

            } catch(Exception e) {
                printLine("Q3.10: Exception occured: \n\n" + e.toString(),bufferedWriter);
            }
            addDivider(bufferedWriter);
            /***********************************************************/
            /***********************************************************/
            /***********************************************************/
            
            /***********************************************************/
            /***********************************************************/
            /***********************************************************/
            addDivider(bufferedWriter);
            addInputTitle("Update his/her status to “eligible” after 120 days have passed since the user was vaccinated last time",bufferedWriter);
            try {
                
                int numberofChanged = cengvacDB.updateStatusToEligible(givendate);
		printLine( numberofChanged + " rows are changed.", bufferedWriter);

            } catch(Exception e) {
                printLine("Q3.11: Exception occured: \n\n" + e.toString(),bufferedWriter);
            }
            addDivider(bufferedWriter);
            /***********************************************************/
            /***********************************************************/
            /***********************************************************/

            /***********************************************************/
            /***********************************************************/
            /***********************************************************/
            addDivider(bufferedWriter);
            addInputTitle("Given vaccine name, delete the vaccine(s) from the database",bufferedWriter);
            try {
                
                Vaccine vaccine = cengvacDB.deleteVaccine(vaccineName);

                //Header Line
                printLine("Code" + "\t" + "Vaccinename" + "\t" + "Type", bufferedWriter);

                if (vaccine != null) {
                    printLine(vaccine.toString(),bufferedWriter);
                }
            } catch(Exception e) {
                printLine("Q3.12: Exception occured: \n\n" + e.toString(),bufferedWriter);
            }
            addDivider(bufferedWriter);
            /***********************************************************/
            /***********************************************************/
            /***********************************************************/

       } catch (IOException e) {
               e.printStackTrace();
       } finally {
           try {
                //Close Writer
                if (bufferedWriter != null) {
                    bufferedWriter.close();
                }
            } catch (IOException e) {
                e.printStackTrace();
            }
            //Close Connection
            disconnect();
       }
    }
    
}
