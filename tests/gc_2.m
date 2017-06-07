Begin
    Function func Paras
    Begin
        Var a End
        Return 1
    End

    Var i sum End
    Assign i 0
    Assign sum 0
    While Lt i 100000
    Begin
        Var r End
        Assign r Apply func Argus End
        Call func Argus End
        If Eq r 1 Begin
            Var a End
        End Else Begin
            Var a End
        End
        While Eq r 1 Begin
            Var a End
            Assign r 0
        End
        Assign sum Plus sum i
        Assign i Plus i 1
    End
    Print sum
End
