Begin 
    Var x y End
    Print x
    Assign x 1
    Print x
    Function f Paras n
    Begin
        Print n
    End
    Assign x f
    Print x
    Call x Argus 3 End
    Assign x 2
    Print x
    Function g Paras
    Begin
        Function h Paras
        Begin
            Print 4
        End
        Return h
    End
    Assign y Apply g Argus End
    Print y
    Call y Argus End
End
