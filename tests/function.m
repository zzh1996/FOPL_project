Begin
    Var x End
    Var y End
    Function f Paras
    Begin
        Assign x Plus x 1
        Print x
    End
    Function g Paras x
    Begin
        Assign x Plus x 1
        Print x
    End
    Function h Paras
    Begin
        Var x End
        Assign x 100
        Print x
        Call f Argus End
    End
    Function r Paras n
    Begin
        Function s Paras
        Begin
            Assign n Plus n 10
            Assign x Plus x 1
            Return n
            Print 2333
        End
        If Gt n 0 Begin Call r Argus Minus n 1 End End Else Begin End
        Print Apply s Argus End
        Print n
    End

    Assign x 10
    Assign y 20
    Call f Argus End
    Call g Argus 1 End
    Call g Argus x End
    Call g Argus y End
    Call h Argus End
    Print x
    Print y
    Call r Argus 3 End
    Print x
End