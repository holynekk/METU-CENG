package ceng.ceng351.cengvacdb;

public class Vaccination {

    private int code;
    private int userID;
    private int dose;
    private String vacdate;

    public Vaccination(int code, int userID, int dose, String vacdate) {
        this.code=code;
        this.userID = userID;
        this.dose = dose;
        this.vacdate = vacdate;
    }

    public int getCode() {
        return code;
    }

    public void setCode(int code) {
        this.code = code;
    }

    public int getUserID() {
        return userID;
    }

    public void setUserID(int userID) {
        this.userID = userID;
    }

    public int getDose() {
        return dose;
    }

    public void setDose(int dose) {
        this.dose = dose;
    }

    public String getVacdate() {
        return vacdate;
    }

    public void setVacdate(String vacdate) {
        this.vacdate = vacdate;
    }
    
    @Override
    public String toString() {
        return code + "\t" + userID + "\t" + dose + "\t" + vacdate;
    }
    
}
