def isCovered(cp_bl, cp_tr, t1_bl, t1_tr, t2_bl, t2_tr):
    if t1_bl[0] <= cp_bl[0] and t1_bl[1] <= cp_bl[1] and t1_tr[0] >= cp_tr[0] and t1_tr[1] >= cp_tr[1]:
        return 'COMPLETELY COVERED'
    elif t2_bl[0] <= cp_bl[0] and t2_bl[1] <= cp_bl[1] and t2_tr[0] >= cp_tr[0] and t2_tr[1] >= cp_tr[1]:
        return 'COMPLETELY COVERED'
    elif t2_bl[1] <= cp_bl[1] and t2_tr[1] >= cp_tr[1] and t2_tr[0] >= cp_tr[0]:
        if t1_tr[0] >= t2_bl[0]:
            if t1_bl[0] <= cp_bl[0] and t1_bl[1] <= cp_bl[1] and t1_tr[1] >= cp_tr[1]:
                return 'COMPLETELY COVERED'
            return 'NOT COMPLETELY COVERED'
        return 'NOT COMPLETELY COVERED'
    elif t1_bl[1] <= cp_bl[1] and t1_tr[1] >= cp_tr[1] and t1_tr[0] >= cp_tr[0]:
        if t2_tr[0] >= t1_bl[0]:
            if t2_bl[0] <= cp_bl[0] and t2_bl[1] <= cp_bl[1] and t2_tr[1] >= cp_tr[1]:
                return 'COMPLETELY COVERED'
            return 'NOT COMPLETELY COVERED'
        return 'NOT COMPLETELY COVERED'
    elif t2_bl[0] <= cp_bl[0] and t2_tr[0] >= cp_tr[0] and t2_tr[1] >= cp_tr[1]:
        if t1_tr[1] >= t2_bl[1]:
            if t1_bl[1] <= cp_bl[1] and t1_bl[0] <= cp_bl[0] and t1_tr[0] >= cp_tr[0]:
                return 'COMPLETELY COVERED'
            return 'NOT COMPLETELY COVERED'
        return 'NOT COMPLETELY COVERED'
    elif t1_bl[0] <= cp_bl[0] and t1_tr[0] >= cp_tr[0] and t1_tr[1] >= cp_tr[1]:
        if t2_tr[1] >= t1_bl[1]:
            if t2_bl[1] <= cp_bl[1] and t2_bl[0] <= cp_bl[0] and t2_tr[0] >= cp_tr[0]:
                return 'COMPLETELY COVERED'
            return 'NOT COMPLETELY COVERED'
        return 'NOT COMPLETELY COVERED'
    return 'NOT COMPLETELY COVERED'
