Begin
    Function makefunc Paras x
    Begin
        Function func Paras
        Begin
            Return x
        End
        Return func
    End

    Var i sum End
    Assign i 0
    Assign sum 0
    While Lt i 500000
    Begin
        Var c End
        Assign c Apply makefunc Argus i End
        Assign sum Plus sum Apply c Argus End
        Assign i Plus i 1
    End
    Print sum
End
