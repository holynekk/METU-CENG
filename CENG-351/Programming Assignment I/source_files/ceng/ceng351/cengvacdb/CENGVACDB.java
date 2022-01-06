package ceng.ceng351.cengvacdb;

import java.sql.Connection;
import java.sql.ResultSet;
import java.sql.DriverManager;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.ArrayList;

public class CENGVACDB implements ICENGVACDB {

    private static String user = "e2381093"; // TODO: Your userName
    private static String password = "oY!kvrj25$V3"; //  TODO: Your password
    private static String host = "144.122.71.121"; // host name
    private static String database = "db2381093"; // TODO: Your database name
    private static int port = 8080; // port

    private static Connection con = null;

    @Override
    public void initialize() {
        String url = "jdbc:mysql://" + host + ":" + port + "/" + database + "?useSSL=false";

        try {
            Class.forName("com.mysql.cj.jdbc.Driver");
            con =  DriverManager.getConnection(url, user, password);
        }
        catch (SQLException | ClassNotFoundException e) {
            e.printStackTrace();
        }
    }

    @Override
    public int createTables() {
        int numberOfTablesCreated = 0;

//        Creating table for User
        String queryCreateUserTable = "create table User (" +
                "userID int not null," +
                "userName varchar(60)," +
                "age int," +
                "address varchar(150)," +
                "password varchar(30)," +
                "status varchar(15)," +
                "primary key (userID))";

        try {
            Statement statement = con.createStatement();
            statement.executeUpdate(queryCreateUserTable);
            numberOfTablesCreated++;
            statement.close();
        } catch (SQLException e) {
            e.printStackTrace();
        }

//        Creating table for Vaccine
        String queryCreateVaccineTable = "create table Vaccine (" +
                "code int not null," +
                "vaccinename varchar(30)," +
                "type varchar(30)," +
                "primary key (code))";

        try {
            Statement statement = con.createStatement();
            statement.executeUpdate(queryCreateVaccineTable);
            numberOfTablesCreated++;
            statement.close();
        } catch (SQLException e) {
            e.printStackTrace();
        }

//        Creating table for Vaccination
        String queryCreateVaccinationTable = "create table Vaccination (" +
                "code int not null," +
                "userID int not null," +
                "dose int not null," +
                "vacdate date," +
                "primary key (code, userID, dose)," +
                "foreign key (code) references Vaccine(code)," +
                "foreign key (userID) references User(userID))";

        try {
            Statement statement = con.createStatement();
            statement.executeUpdate(queryCreateVaccinationTable);
            numberOfTablesCreated++;
            statement.close();
        } catch (SQLException e) {
            e.printStackTrace();
        }

//        Creating table for AllergicSideEffect
        String queryCreateAllergicSideEffectTable = "create table AllergicSideEffect (" +
                "effectcode int not null," +
                "effectname varchar(50)," +
                "primary key (effectcode))";

        try {
            Statement statement = con.createStatement();
            statement.executeUpdate(queryCreateAllergicSideEffectTable);
            numberOfTablesCreated++;
            statement.close();
        } catch (SQLException e) {
            e.printStackTrace();
        }

        String queryCreateSeenTable = "create table Seen (" +
                "effectcode int not null," +
                "code int not null," +
                "userID int not null," +
                "date date," +
                "degree varchar(30)," +
                "primary key (effectcode, code, userID)," +
                "foreign key (effectcode) references AllergicSideEffect(effectcode)," +
                "foreign key (code) references Vaccination(code) on delete cascade," +
                "foreign key (userID) references User(userID))";

        try {
            Statement statement = con.createStatement();
            statement.executeUpdate(queryCreateSeenTable);
            numberOfTablesCreated++;
            statement.close();
        } catch (SQLException e) {
            e.printStackTrace();
        }
        return numberOfTablesCreated;
    }

    @Override
    public int dropTables() {
        int numberOfTablesDropped = 0;

//        Drop Seen table
        String queryDropSeenTable = "drop table if exists Seen";

        try {
            Statement statement = con.createStatement();
            statement.executeUpdate(queryDropSeenTable);
            numberOfTablesDropped++;
            statement.close();
        } catch (SQLException e) {
            e.printStackTrace();
        }

//        Drop Vaccination table
        String queryDropVaccinationTable = "drop table if exists Vaccination";

        try {
            Statement statement = con.createStatement();
            statement.executeUpdate(queryDropVaccinationTable);
            numberOfTablesDropped++;
            statement.close();
        } catch (SQLException e) {
            e.printStackTrace();
        }

//        Drop user table
        String queryDropUserTable = "drop table if exists User";

        try {
            Statement statement = con.createStatement();
            statement.executeUpdate(queryDropUserTable);
            numberOfTablesDropped++;
            statement.close();
        } catch (SQLException e) {
            e.printStackTrace();
        }

//        Drop Vaccine table
        String queryDropVaccineTable = "drop table if exists Vaccine";

        try {
            Statement statement = con.createStatement();

            //User
            statement.executeUpdate(queryDropVaccineTable);
            numberOfTablesDropped++;

            //close
            statement.close();

        } catch (SQLException e) {
            e.printStackTrace();
        }

//        Drop AllergicSideEffect table
        String queryDropAllergicSideEffectTable = "drop table if exists AllergicSideEffect";

        try {
            Statement statement = con.createStatement();
            statement.executeUpdate(queryDropAllergicSideEffectTable);
            numberOfTablesDropped++;
            statement.close();
        } catch (SQLException e) {
            e.printStackTrace();
        }
        return numberOfTablesDropped;
    }

    @Override
    public int insertUser(User[] users) {
        int numberOfRowsInserted = 0;

        for( int k = 0; k < users.length; k++) {

            String query = "insert into User values (\"" +
                    users[k].getUserID()+ "\",\"" +
                    users[k].getUserName()+ "\",\"" +
                    users[k].getAge()+ "\",\"" +
                    users[k].getAddress()+ "\",\"" +
                    users[k].getPassword()+ "\",\"" +
                    users[k].getStatus() + "\")";

            try {
                Statement st = con.createStatement();
                st.executeUpdate(query);
                numberOfRowsInserted++ ;
                st.close();
            } catch (SQLException e) {
                e.printStackTrace();
            }
        }
        return numberOfRowsInserted ;
    }

    @Override
    public int insertAllergicSideEffect(AllergicSideEffect[] sideEffects) {
        int numberOfRowsInserted = 0;

        for( int k = 0; k < sideEffects.length; k++) {

            String query = "insert into AllergicSideEffect values (\"" +
                    sideEffects[k].getEffectCode()+ "\",\"" +
                    sideEffects[k].getEffectName() + "\")";

            try {
                Statement st = con.createStatement();
                st.executeUpdate(query);
                numberOfRowsInserted++ ;
                st.close();
            } catch (SQLException e) {
                e.printStackTrace();
            }
        }
        return numberOfRowsInserted ;
    }

    @Override
    public int insertVaccine(Vaccine[] c) {
        int numberOfRowsInserted = 0;

        for( int k = 0; k < c.length; k++) {

            String query = "insert into Vaccine values (\"" +
                    c[k].getCode()+ "\",\"" +
                    c[k].getVaccineName()+ "\",\"" +
                    c[k].getType() + "\")";

            try {
                Statement st = con.createStatement();
                st.executeUpdate(query);
                numberOfRowsInserted++ ;
                st.close();
            } catch (SQLException e) {
                e.printStackTrace();
            }
        }
        return numberOfRowsInserted ;
    }

    @Override
    public int insertVaccination(Vaccination[] vaccinations) {
        int numberOfRowsInserted = 0;

        for(int k = 0; k < vaccinations.length; k++) {

            String query = "insert into Vaccination values (\"" +
                    vaccinations[k].getCode()+ "\",\"" +
                    vaccinations[k].getUserID()+ "\",\"" +
                    vaccinations[k].getDose()+ "\",\"" +
                    vaccinations[k].getVacdate() + "\")";

            try {
                Statement st = con.createStatement();
                st.executeUpdate(query);
                numberOfRowsInserted++ ;
                st.close();
            } catch (SQLException e) {
                e.printStackTrace();
            }
        }
        return numberOfRowsInserted ;
    }

    @Override
    public int insertSeen(Seen[] seens) {
        int numberOfRowsInserted = 0;

        for( int k = 0; k < seens.length; k++) {

            String query = "insert into Seen values (\"" +
                    seens[k].getEffectcode()+ "\",\"" +
                    seens[k].getCode()+ "\",\"" +
                    seens[k].getUserID()+ "\",\"" +
                    seens[k].getDate()+ "\",\"" +
                    seens[k].getDegree() + "\")";

            try {
                Statement st = con.createStatement();
                st.executeUpdate(query);
                numberOfRowsInserted++ ;
                st.close();
            } catch (SQLException e) {
                e.printStackTrace();
            }
        }
        return numberOfRowsInserted ;
    }

    @Override
    public Vaccine[] getVaccinesNotAppliedAnyUser() {
        ArrayList<Vaccine> rlist = new ArrayList<Vaccine>();
        Vaccine r[];
        Vaccine r_new;
        ResultSet rs;

        String query = "select V.code, V.vaccinename, V.type " +
                "from Vaccine V " +
                "where V.code not in (select distinct V1.code from Vaccination V1)" +
                "order by V.code;";

        try {
            Statement st = con.createStatement();
            rs = st.executeQuery(query);

            while(rs.next()) {
                int r_code= rs.getInt("code");
                String r_vaccinename = rs.getString("vaccinename");
                String r_type = rs.getString("type");
                r_new = new Vaccine(r_code, r_vaccinename, r_type);
                rlist.add(r_new) ;
            }
            st.close();
        } catch (SQLException e) {
            e.printStackTrace();
        }
        r = new Vaccine[rlist.size()];
        r = rlist.toArray(r);
        return r;
    }

    @Override
    public QueryResult.UserIDuserNameAddressResult[] getVaccinatedUsersforTwoDosesByDate(String vacdate) {
        ArrayList<QueryResult.UserIDuserNameAddressResult> rlist = new ArrayList<QueryResult.UserIDuserNameAddressResult>();
        QueryResult.UserIDuserNameAddressResult r[] ;
        QueryResult.UserIDuserNameAddressResult r_new ;
        ResultSet rs ;

        String query = "select U.userID, U.userName, U.address " +
                "from User U " +
                "where 2 = (select count(V1.code) from Vaccination V1 where U.userID = V1.userID and V1.vacdate > \"" + vacdate +"\" ) " +
                "order by U.userID;";

        try {
            Statement st = con.createStatement();
            rs = st.executeQuery(query);

            while( rs.next() ) {
                int r_userID= rs.getInt("userID");
                String r_userName = rs.getString("userName");
                String r_address = rs.getString("address");
                r_new = new QueryResult.UserIDuserNameAddressResult(Integer.toString(r_userID), r_userName, r_address);
                rlist.add(r_new) ;
            }
            st.close();
        } catch (SQLException e) {
            e.printStackTrace();
        }
        r = new QueryResult.UserIDuserNameAddressResult[rlist.size()];
        r = rlist.toArray(r);
        return r;
    }

    @Override
    public Vaccine[] getTwoRecentVaccinesDoNotContainVac() {
        ArrayList<Vaccine> rlist = new ArrayList<Vaccine>();
        Vaccine r[];
        Vaccine r_new;
        ResultSet rs;

        String query =
                "select distinct XX.code, XX.vaccinename, XX.type " +
                "from (" +
                    "select distinct X.code, X.vaccinename, X.type " +
                    "from (" +
                        "select distinct V1.code, V2.vaccinename, V2.type, V1.vacdate " +
                        "from Vaccination V1, Vaccine V2 " +
                        "where V2.vaccinename not like '%vac%' and V1.code = V2.code " +
                        "order by V1.vacdate desc" +
                    ") as X " +
                    "limit 0, 2" +
                ") as XX " +
                "order by XX.code;";

        try {
            Statement st = con.createStatement();
            rs = st.executeQuery(query);

            while(rs.next()) {
                int r_code= rs.getInt("code");
                String r_vaccinename = rs.getString("vaccinename");
                String r_type = rs.getString("type");
                r_new = new Vaccine(r_code, r_vaccinename, r_type);
                rlist.add(r_new) ;
            }
            st.close();
        } catch (SQLException e) {
            e.printStackTrace();
        }
        r = new Vaccine[rlist.size()];
        r = rlist.toArray(r);
        return r;
    }

    @Override
    public QueryResult.UserIDuserNameAddressResult[] getUsersAtHasLeastTwoDoseAtMostOneSideEffect() {
        ArrayList<QueryResult.UserIDuserNameAddressResult> rlist = new ArrayList<QueryResult.UserIDuserNameAddressResult>();
        QueryResult.UserIDuserNameAddressResult r[] ;
        QueryResult.UserIDuserNameAddressResult r_new ;
        ResultSet rs ;

        String query =
                "select distinct U.userID, U.userName, U.address " +
                "from User U, Vaccine V, Vaccination Vt " +
                "where U.userID = Vt.userID and Vt.dose >= 2 and Vt.code = V.code and U.userID not in " +
                "(select U1.userID " +
                "from User U1 " +
                "where 2 <= (select count(S1.userID) from Seen S1, Seen S2 where U1.userID = S1.userID and U1.userID = S2.userID and S1.effectcode <> S2.userID) ) " +
                "order by U.userID;";

        try {
            Statement st = con.createStatement();
            rs = st.executeQuery(query);

            while( rs.next() ) {
                int r_userID= rs.getInt("userID");
                String r_userName = rs.getString("userName");
                String r_address = rs.getString("address");
                r_new = new QueryResult.UserIDuserNameAddressResult(Integer.toString(r_userID), r_userName, r_address);
                rlist.add(r_new) ;
            }
            st.close();
        } catch (SQLException e) {
            e.printStackTrace();
        }
        r = new QueryResult.UserIDuserNameAddressResult[rlist.size()];
        r = rlist.toArray(r);
        return r;
    }

    @Override
    public QueryResult.UserIDuserNameAddressResult[] getVaccinatedUsersWithAllVaccinesCanCauseGivenSideEffect(String effectname) {
        ArrayList<QueryResult.UserIDuserNameAddressResult> rlist = new ArrayList<QueryResult.UserIDuserNameAddressResult>();
        QueryResult.UserIDuserNameAddressResult r[] ;
        QueryResult.UserIDuserNameAddressResult r_new ;
        ResultSet rs ;

        String query =
                "select U.userID, U.userName, U.address " +
                "from User U " +
                "where not exists (" +
                    "select S1.code " +
                    "from Seen S1, AllergicSideEffect A1 " +
                    "where A1.effectname = \"" + effectname + "\" and S1.effectcode = A1.effectcode and " +
                    "S1.code not in (" +
                        "select S2.code " +
                        "from Seen S2, AllergicSideEffect A2 " +
                        "where A2.effectname = \"" + effectname + "\" and S2.effectcode = A2.effectcode and U.userID = S2.userID" +
                    ") )" +
                "order by U.userID;";

        try {
            Statement st = con.createStatement();
            rs = st.executeQuery(query);

            while( rs.next() ) {
                int r_userID= rs.getInt("userID");
                String r_userName = rs.getString("userName");
                String r_address = rs.getString("address");
                r_new = new QueryResult.UserIDuserNameAddressResult(Integer.toString(r_userID), r_userName, r_address);
                rlist.add(r_new) ;
            }
            st.close();
        } catch (SQLException e) {
            e.printStackTrace();
        }
        r = new QueryResult.UserIDuserNameAddressResult[rlist.size()];
        r = rlist.toArray(r);
        return r;
    }

    @Override
    public QueryResult.UserIDuserNameAddressResult[] getUsersWithAtLeastTwoDifferentVaccineTypeByGivenInterval(String startdate, String enddate) {
        ArrayList<QueryResult.UserIDuserNameAddressResult> rlist = new ArrayList<QueryResult.UserIDuserNameAddressResult>();
        QueryResult.UserIDuserNameAddressResult r[] ;
        QueryResult.UserIDuserNameAddressResult r_new ;
        ResultSet rs ;

        String query =
                "select U.userID, U.userName, U.address " +
                "from User U " +
                "where 0 < (" +
                    "select count(V1.code) " +
                    "from Vaccination V1, Vaccination V2 " +
                    "where U.userID = V1.userID and U.userID = V2.userID and V1.code <> V2.code and " +
                    "V1.vacdate > \"" + startdate +"\" and " + "V1.vacdate < \"" + enddate +"\" and " +
                    "V2.vacdate > \"" + startdate +"\" and " + "V2.vacdate < \"" + enddate +"\" " +
                ") " +
                "order by U.userID;";

        try {
            Statement st = con.createStatement();
            rs = st.executeQuery(query);

            while( rs.next() ) {
                int r_userID= rs.getInt("userID");
                String r_userName = rs.getString("userName");
                String r_address = rs.getString("address");
                r_new = new QueryResult.UserIDuserNameAddressResult(Integer.toString(r_userID), r_userName, r_address);
                rlist.add(r_new) ;
            }
            st.close();
        } catch (SQLException e) {
            e.printStackTrace();
        }
        r = new QueryResult.UserIDuserNameAddressResult[rlist.size()];
        r = rlist.toArray(r);
        return r;
    }

    @Override
    public AllergicSideEffect[] getSideEffectsOfUserWhoHaveTwoDosesInLessThanTwentyDays() {
        ArrayList<AllergicSideEffect> rlist = new ArrayList<AllergicSideEffect>();
        AllergicSideEffect r[];
        AllergicSideEffect r_new;
        ResultSet rs;

        String query =
                "select A.effectcode, A.effectname " +
                "from User U, Seen S, AllergicSideEffect A " +
                "where " +
                    "S.userID = U.userID and " +
                    "A.effectcode = S.effectcode and " +
                    "U.userID in (" +
                        "select V1.userID " +
                        "from Vaccination V1, Vaccination V2 " +
                        "where " +
                            "V1.userID = V2.userID and " +
                            "U.userID = V1.userID and " +
                            "V1.dose = 1 and " +
                            "V2.dose = 2 and " +
                            "DATEDIFF(V2.vacdate, V1.vacdate) < 20 " +
                    ")" +
                "order by A.effectcode asc;";

        try {
            Statement st = con.createStatement();
            rs = st.executeQuery(query);

            while(rs.next()) {
                int r_effectcode = rs.getInt("effectcode");
                String r_effectname = rs.getString("effectname");
                r_new = new AllergicSideEffect(r_effectcode, r_effectname);
                rlist.add(r_new) ;
            }
            //Close
            st.close();
        } catch (SQLException e) {
            e.printStackTrace();
        }
        r = new AllergicSideEffect[rlist.size()];
        r = rlist.toArray(r);
        return r;
    }

    @Override
    public double averageNumberofDosesofVaccinatedUserOverSixtyFiveYearsOld() {
        double result = 0 ;
        ResultSet rs;

        String query =
                "select avg(X.maxDoses) as averageDose " +
                "from (select max(V.dose) as maxDoses, U.age from User U, Vaccination V where U.userID = V.userID group by U.userID) as X " +
                "where X.age > 65 " +
                ";";

        try {
            Statement st = con.createStatement();
            rs = st.executeQuery(query);
            rs.next();
            double s_code= rs.getDouble("averageDose");
            result = rs.getDouble("averageDose");
            st.close();
        } catch (SQLException e) {
            e.printStackTrace();
        }

        return result;
    }

    @Override
    public int updateStatusToEligible(String givendate) {
        int result = 0 ;

        String query =
                "UPDATE User U, (select max(V1.vacdate) as maxDate, U1.userID from User U1, Vaccination V1 where U1.userID = V1.userID group by U1.userID) as X "+
                "SET U.status = \"eligible\" " +
                "where U.status <> \"eligible\" and U.userID = X.userID and ADDDATE(X.maxDate, INTERVAL 120 DAY) < \"" + givendate + "\"" +
                ";" ;

        try {
            Statement st = con.createStatement();
            result = st.executeUpdate(query);
            st.close();
        } catch (SQLException e) {
            e.printStackTrace();
        }

        return result;
    }

    @Override
    public Vaccine deleteVaccine(String vaccineName) {
        Vaccine vac = null ;
        ResultSet rs ;

        String query =
                "select * "+
                "from Vaccine V "+
                "where V.vaccinename = \"" +
                vaccineName +
                "\";" ;

        try {
            Statement st = con.createStatement();
            rs = st.executeQuery(query);
            rs.next();
            int s_code= rs.getInt("code");
            String s_name = rs.getString("vaccinename");
            String s_type = rs.getString("type") ;
            vac = new Vaccine(s_code, s_name, s_type);
            //Close
            st.close();
        } catch (SQLException e) {
            e.printStackTrace();
        }

        query = "delete from Vaccine "+
                "where vaccinename = \"" +
                vaccineName +
                "\";" ;

        try {
            Statement st = con.createStatement();
            st.executeUpdate(query);
            st.close();
        } catch (SQLException e) {
            e.printStackTrace();
        }
        return vac;
    }
}
