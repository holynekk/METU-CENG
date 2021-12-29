package ceng.ceng351.cengvacdb;

public class QueryResult {
    
        public static class UserIDuserNameAddressResult{
	
        String userID;
        String userName;
        String address;

        public UserIDuserNameAddressResult(
                    String userID,
                    String userName,
                    String address) {

            this.userID = userID;
            this.userName = userName;
            this.address = address;
        }

        public String toString() {
            return userID + "\t" + userName + "\t" + address;
        }	
    }
    

}
