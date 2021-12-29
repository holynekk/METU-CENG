package ceng.ceng351.cengvacdb;

public class Seen {
    
    private int effectcode;
    private int code;
    private String userID;
    private String date;
    private String degree;

    public Seen(int e_code, int code, String userID, String date, String degree) {
        this.effectcode = e_code;
        this.code = code;
        this.userID = userID;
        this.date = date;
        this.degree = degree;
    }

    public int getEffectcode() {
        return effectcode;
    }

    public void setEffectcode(int e_code) {
        this.effectcode = e_code;
    }

    public int getCode() {
        return code;
    }

    public void setCode(int code) {
        this.code = code;
    }

    public String getDate(){
        return date;
    }

    public void setUserID(String userID) {
        this.userID = userID;
    }

    public String getUserID() {
        return userID;
    }

    public void setDate(String date) {
        this.date = date;
    }

    public String getDegree() {
        return degree;
    }

    public void setDegree(String degree) {
        this.degree = degree;
    }
    
    @Override
    public String toString() {
        return effectcode + "\t" + code +  "\t" + userID + "\t" + date + "\t" + degree;
    }
}
