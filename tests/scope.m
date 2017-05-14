Begin
    Var x End
    Assign x 1
    Function f Paras Begin
        Function g Paras Begin
            Print x
        End
        Assign x 2
        Call g Argus End
    End
    Call f Argus End

    Var y End
    Assign y 1
    Function ff Paras Begin
        Function g Paras Begin
            Print y
        End
        Var y End
        Assign y 2
        Call g Argus End
    End
    Call ff Argus End
End