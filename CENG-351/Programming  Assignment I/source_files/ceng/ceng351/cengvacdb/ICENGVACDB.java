package ceng.ceng351.cengvacdb;

public interface ICENGVACDB {
    
    /**
    * Place your initialization code inside if required.
    * 
    * <p>
    * This function will be called before all other operations. If your implementation
    * need initialization , necessary operations should be done inside this function. For
    * example, you can set your connection to the database server inside this function.
    */
   public void initialize();
   
   /**
    * Should create the necessary tables when called.
    * 
    * @return the number of tables that are created successfully.
    */
   public int createTables();
   
   /**
    * Should drop the tables if exists when called. 
    * 
    * @return the number of tables are dropped successfully.
    */
   public int dropTables();
   
   /**
    * Should insert an array of User into the database.
    * 
    * @return Number of rows inserted successfully.
    */
   public int insertUser(User[] users);

   /**
    * Should insert an array of AllergicSideEffect into the database.
    *
    * @return Number of rows inserted successfully.
    */

   public int insertAllergicSideEffect(AllergicSideEffect[] sideEffects);

   /**
    * Should insert an array of Vaccine into the database.
    *
    * @return Number of rows inserted successfully.
    */
   public int insertVaccine(Vaccine[] vaccines);
   
   /**
    * Should insert an array of Vaccination into the database.
    * 
    * @return Number of rows inserted successfully.
    */
   public int insertVaccination(Vaccination[] vaccinations);

   /**
    * Should insert an array of Seen into the database.
    * 
    * @return Number of rows inserted successfully.
    */

   public int insertSeen(Seen[] seens);
   
   /**
    * Should return vaccines that have not been applied to any user
    * 
    * @return Vaccine[]
    */
   public Vaccine[] getVaccinesNotAppliedAnyUser();
   
   /**
    * Should return the users who have been vaccinated for two doses since the
    * given date
    * 
    * @return UserIDuserNameAddressResult[]
    */
   public QueryResult.UserIDuserNameAddressResult[] getVaccinatedUsersforTwoDosesByDate(String vacdate);
   
   /**
    * List of the two recent vaccines applied to any user that
    * do not contain ‘vac’ in their name
    * 
    * @return Vaccine[]
    */
   public Vaccine[] getTwoRecentVaccinesDoNotContainVac();
   
   /**
    * Should return users who have had at least two doses of vaccine and
    * have experienced at most one side effect
    * 
    * @return UserIDuserNameAddressResult[]
    */
   public QueryResult.UserIDuserNameAddressResult[] getUsersAtHasLeastTwoDoseAtMostOneSideEffect();
   
   /**
    * Should return users who have been vaccinated with all vaccines that
    * can cause the given side effect
    * 
    * @return UserIDuserNameAddressResult[]
    */
   public QueryResult.UserIDuserNameAddressResult[] getVaccinatedUsersWithAllVaccinesCanCauseGivenSideEffect(String effectname);
   
   /**
    * Should return users who have been vaccinated with at least two different
    * types of vaccines within the given time interval
    * 
    * @return UserIDuserNameAddressResult[]
    */
   public QueryResult.UserIDuserNameAddressResult[]  getUsersWithAtLeastTwoDifferentVaccineTypeByGivenInterval(String startdate, String enddate);
   
   /**
    * Should return side effects experienced by users who have two doses of
    * vaccine with less than 20 days between doses
    * 
    * @return AllergicSideEffect[]
    */
   public AllergicSideEffect[] getSideEffectsOfUserWhoHaveTwoDosesInLessThanTwentyDays();
   
   /**
    * Calculate the average number of doses of vaccinated users over the age of 65
    * 
    * @return average
    */
   public double averageNumberofDosesofVaccinatedUserOverSixtyFiveYearsOld();
   
   /**
    * Update his/her status to “eligible” after 120 days have
    * passed since the user was vaccinated last time
    * 
    * @return the row count for SQL Data Manipulation Language (DML) statements
    */
   public int updateStatusToEligible(String givendate);
   
   /**
    * Delete the vaccine for the given vaccineName
    * 
    * @return Vaccine
    */
   public Vaccine deleteVaccine(String vaccineName);
   
}
