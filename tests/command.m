Begin
    Function f Paras
    Begin
        Print 100
    End
    Function g Paras a b c
    Begin
        Print a
        Print b
        Print c
    End
    Function h Paras
    Begin
        Print 10
        Return Plus 10 5
        Print 20
    End
    Var x End
    Var y End
    Var z End
    Call f Argus End
    Read x
    Read y
    Read z
    Assign z Plus z 1
    Call g Argus x y z End
    Print Apply f Argus End
    Print Apply h Argus End
End