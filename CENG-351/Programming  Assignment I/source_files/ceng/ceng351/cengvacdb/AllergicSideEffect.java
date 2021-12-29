package ceng.ceng351.cengvacdb;

public class AllergicSideEffect {

    private int effectcode;
    private String effectname;

    public AllergicSideEffect(int code, String name) {
        this.effectcode = code;
        this.effectname = name;
    }

    public int getEffectCode() {
        return effectcode;
    }

    public void setEffectCode(int code) {
        this.effectcode = code;
    }

    public String getEffectName() {
        return effectname;
    }

    public void setEffectName(String name) {
        this.effectname = name;
    }

    @Override
    public String toString() {
        return effectcode + "\t" + effectname;
    }
    
}
