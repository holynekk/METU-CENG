package ceng.ceng351.cengvacdb;

public class User {
    
    private int userID;
    private String userName;
    private int age;
    private String address;
    private String password;
    private String status;

    public User(int userID, String name, int age, String address, String password, String status) {
        this.userID = userID;
        this.userName = name;
        this.age = age;
        this.address = address;
        this.password = password;
        this.status = status;
    }

    public int getUserID() {
        return userID;
    }

    public void setUserID(int userID) {
        this.userID = userID;
    }

    public String getUserName() {
        return userName;
    }

    public void userName(String name) {
        this.userName = name;
    }

    public int getAge() {
        return age;
    }

    public void setAge(int age) {
        this.age = age;
    }

    public String getAddress() {
        return address;
    }

    public void setAddress(String address) {
        this.address = address;
    }

    public String getPassword() {
        return password;
    }

    public void setPassword(String password) {
        this.password = password;
    }

    public String getStatus() {
        return status;
    }

    public void setStatus(String status) {
        this.status = status;
    }
    
    @Override
    public String toString() {
        return userID + "\t" + userName + "\t" + age + "\t" + address + "\t" + password + "\t" + status;
    }
    
    
}
