package ceng.ceng351.cengvacdb;

public class Vaccine {
    
    private int code;
    private String vaccinename;
    private String type;

    public Vaccine(int code, String name, String type) {
        this.code = code;
        this.vaccinename = name;
        this.type = type;
    }

    public int getCode() {
        return code;
    }

    public void setCode(int code) {
        this.code = code;
    }

    public String getVaccineName() {
        return vaccinename;
    }

    public void setVaccineName(String name) {
        this.vaccinename = name;
    }

    public String getType() {
        return type;
    }

    public void setType(String type) {
        this.type = type;
    }

    @Override
    public String toString() {
        return code + "\t" + vaccinename + "\t" + type;
    }
}
